#ifndef HASH_HOPSCOTCH_13edac895edc44679cfb79840139b3cf
#define HASH_HOPSCOTCH_13edac895edc44679cfb79840139b3cf

/****************************************************************************
*
*  Description:
*   Easy interface for bit operations. Allows all sorts of fun masking junk.
*
*  Author: Zachary Stiers (2015-05-29)
*  Owner:  Zachary Stiers
*
****/

#include "../Bits/BitMask.h"
#include "../Debug/Assert.h"
#include "../Hash/Hash.h"
#include "../Utility/Common.h"

namespace Baseless
{
    namespace Container
    {
        // These need to be removed. They do not belong here. The first two should be in a math file and the last I'm not sure of yet.
        template <typename T>
        int Log2 (T n)
        {
            return n ? Bits::FindHighestSetBit(n) : 0;
        }
        template <typename T>
        inline T pow2roundup (T x)
        {
            return (T(1) << (Log2(x - 1) + 1));
        }
        namespace Mem
        {
            template <typename T> T * Alloc (size_t num)     { return reinterpret_cast<T *>(std::malloc(num * sizeof(T))); }
            template <typename T> T * AllocZero (size_t num) { return reinterpret_cast<T *>(std::calloc(num, sizeof(T))); }

            void Free (void * data)         { std::free(data); }
        }



        // Beginning
        template <typename Key, typename Hasher, typename Notifier>
        class THashHopscotchHelper :
            private Notifier
        {
        public: // Types and constants
            static const auto MIN_CAPACITY  = sizeof(size_t) * CHAR_BIT;
            static const auto MAX_CAPACITY  = size_t(-1);
            static const auto FALLBACK_HASH = Hash::HashCode(sizeof(Hash::HashCode) > sizeof(uint32_t) ? 18446744073709551557 : 4294967291); // The numbers are large primes.

            static const auto MAX_HOP = sizeof(size_t) * CHAR_BIT;
            typedef Bits::TBitMask<MAX_HOP> Hop;

            typedef size_t SlotIndex;
            static const SlotIndex INVALID_INDEX = MAX_CAPACITY;
    
            typedef typename Notifier::UserData UserData;

        public: // Enumerators
            struct AccessorKeyConst   { typedef const Key & AccessType; AccessType Access (const THashHopscotchHelper & map, SlotIndex slot) { return map.m_keys[slot]; }   };

            template <typename Access>
            class TIterator : private Access
            {
                const THashHopscotchHelper & m_map;
                SlotIndex                      m_slot;
            public:
                TIterator (const THashHopscotchHelper & map, SlotIndex slot) : m_map(map), m_slot(slot) { --m_slot; ++*this; }
                bool operator!= (const TIterator & rhs) const         { return m_slot != rhs.m_slot; }
                const TIterator & operator++ ()                       { for (const auto stop = m_map.m_capacity; ++m_slot < stop && m_map.IsSlotEmpty(m_slot);); return *this; }
                typename Access::AccessType operator * ()             { return Access::Access(m_map, m_slot); }
            };

            template <typename Access>
            class TEnumerable
            {
                const THashHopscotchHelper & m_map;
            public:
                TEnumerable (const THashHopscotchHelper & map) : m_map(map) { }
                TIterator<Access> begin () const { return TIterator<Access>(m_map, 0);                }
                TIterator<Access> end () const   { return TIterator<Access>(m_map, m_map.m_capacity); }
            };

        public: // Data
            size_t           m_count = 0;
            size_t           m_capacity = 0;
            size_t           m_indexMask = 0;
            Key *            m_keys = nullptr;
            Hash::HashCode * m_hashes = nullptr;
            Hop *            m_hops = nullptr;

        public: // Basic helpers
            size_t         FindIndex (const Key & key) const;
            size_t         FindIndex (const Key & key, Hash::HashCode hash, size_t hashIndex) const;
            SlotIndex      FindNextEmpty (SlotIndex startIndex) const;
            Hash::HashCode GetHash (const Key & key) const { const auto result = Hasher::Hash(key); return result ? result : FALLBACK_HASH; }
            size_t         GetSlotDifference (size_t high, size_t low) const;
            void           Grow ();
            void           GrowToCapacity (size_t capacity);
            size_t         HashToIndex (Hash::HashCode hash) const { return PutInRange(hash); }
            bool           IsSlotOccupied (size_t index) const;
            bool           IsSlotEmpty (size_t index) const;
            size_t         PutInRange (size_t slotIndex) const { return slotIndex & m_indexMask; }
            void           MoveToSlot (SlotIndex oldSlot, SlotIndex newSlot);
            SlotIndex      GetEmptyNeighbor (Hash::HashCode neighborHoodHash, SlotIndex neighborHoodSlot);
            SlotIndex      TryEmptyCloser (SlotIndex desiredSlot, SlotIndex closestSlot);

        public: // Templated helpers
            template <typename KeyFrom, typename UserDataFrom = UserData>
            void SetInternal (const Key & key, UserData userData);
            template <typename KeyFrom, typename UserDataFrom = UserData>
            void SetInternal (const Key & key, Hash::HashCode hash, UserData userData);
            template <typename KeyFrom, typename UserDataFrom = UserData>
            void SetInternalNew (const Key & key, Hash::HashCode hash, UserData userData);

        public: // Construct
            THashHopscotchHelper () { }

        public: // Init functions
            void InitCapacity (size_t startCapacity);
            void InitCount (size_t startCount);

        public: // Destruct
            ~THashHopscotchHelper ();

        public: // Access
            size_t Capacity () const { return m_capacity; }
            size_t Count () const    { return m_count;    }

        public: // Commands
            void Erase (const Key & key);

        public: // Queries
            bool     Contains (const Key & key) const;
            UserData GetUserData (const Key & key) const { const auto index = FindIndex(key); ASSERT(index != INVALID_INDEX)(index); return OnRequestUserData(index); }

        public: // Neat accessors. Especially cool because of the iterator stuff they provide.
            TEnumerable<AccessorKeyConst> Keys () const { return TEnumerable<AccessorKeyConst>(*this);   }

        public: // Friendlies
            friend void Swap (THashHopscotchHelper & lhs, THashHopscotchHelper & rhs)
            {
                using Baseless::Utility::Swap;
                Swap(lhs.m_capacity, rhs.m_capacity);
                Swap(lhs.m_count, rhs.m_count);
                Swap(lhs.m_indexMask, rhs.m_indexMask);
                Swap(lhs.m_keys, rhs.m_keys);
                Swap(lhs.m_hashes, rhs.m_hashes);
                Swap(lhs.m_hops, rhs.m_hops);
                Swap(static_cast<Notifier &>(lhs), static_cast<Notifier &>(rhs));
            }
        };

        // Implementation
        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::InitCapacity (size_t startCapacity)
        {
            startCapacity = pow2roundup(startCapacity > MIN_CAPACITY ? startCapacity : MIN_CAPACITY);

            m_capacity  = startCapacity;
            m_indexMask = startCapacity - 1,
            m_keys      = Mem::Alloc<Key>(startCapacity);
            m_hashes    = Mem::AllocZero<Baseless::Hash::HashCode>(startCapacity);
            m_hops      = Mem::AllocZero<Hop>(startCapacity);
            OnInitCapacity(startCapacity);
        }

        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::InitCount (size_t startCount)
        {
            InitCapacity(startCount * 5 / 4);
        }

        template <typename Key, typename Hasher, typename Notifier>
        THashHopscotchHelper<Key, Hasher, Notifier>::~THashHopscotchHelper ()
        {
            //if (!std::is_trivially_destructible<Key>::value)
            {
                for (auto index = m_capacity; index--; )
                {
                    if (IsSlotOccupied(index))
                    {
                        Utility::Destruct(m_keys[index]);
                        Notifier::OnDestroy(index);
                    }
                }
            }

            Mem::Free(m_keys);
            Mem::Free(m_hashes);
            Mem::Free(m_hops);
        }

        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::Erase (const Key & key)
        {
            const auto hash      = GetHash(key);
            const auto hashIndex = HashToIndex(hash);
            const auto index     = FindIndex(key, hash, hashIndex);
            ASSERT_MSG(index < m_capacity, "Key not found")(index, m_capacity);
            ASSERT(!IsSlotEmpty(index))(index);

            // Unset the hop
            auto &     hops      = m_hops[hashIndex];
            const auto hopOffset = GetSlotDifference(index, hashIndex);
            ASSERT(hopOffset < MAX_HOP)(hopOffset, MAX_HOP);
            ASSERT(hops.CheckBit(hopOffset))(hopOffset);
            hops.ClearBit(hopOffset);
    
            // Destruct the values
            Utility::Destruct(m_keys[index]);

            // Let other stuff handle the erase
            Notifier::OnErase(index);

            // Clear out the hash
            m_hashes[index] = 0;

            // Update the count
            --m_count;
        }

        template <typename Key, typename Hasher, typename Notifier>
        typename THashHopscotchHelper<Key, Hasher, Notifier>::SlotIndex THashHopscotchHelper<Key, Hasher, Notifier>::TryEmptyCloser (SlotIndex desiredSlot, SlotIndex closestSlot)
        {
            // TODO : ZS : Fix this weird nonsense that is making this always be above the range as it could be wonky at high capacities.
            static_assert(MAX_HOP > 0, "Subtracting from zero here would be very bad.");
            const auto testIndexEnd   = closestSlot + m_capacity;
            const auto testIndexBegin = testIndexEnd - (MAX_HOP - 1);
            ASSERT(testIndexBegin < testIndexEnd)(testIndexBegin, testIndexEnd);
            for (auto testIndex = testIndexBegin; testIndex < testIndexEnd; ++testIndex)
            {
                const auto testSlot   = PutInRange(testIndex);
                auto &     testHops   = m_hops[testSlot];
                if (!testHops)
                    continue;
        
                const auto testOffset   = Bits::FindLowestSetBit(testHops);
                const auto testSwapSlot = testIndex + testOffset;
                if (testSwapSlot >= testIndexEnd)
                    continue;

                const auto swappableSlot = PutInRange(testSwapSlot);
                MoveToSlot(swappableSlot, closestSlot);
                return swappableSlot;
            }

            return INVALID_INDEX;
        }

        template <typename Key, typename Hasher, typename Notifier>
        typename THashHopscotchHelper<Key, Hasher, Notifier>::SlotIndex THashHopscotchHelper<Key, Hasher, Notifier>::GetEmptyNeighbor (Hash::HashCode neighborHoodHash, SlotIndex neighborHoodSlot)
        {
            auto index = FindNextEmpty(neighborHoodSlot);
            if (index != INVALID_INDEX)
            {
                if (GetSlotDifference(index, neighborHoodSlot) < MAX_HOP)
                    return index;

                auto displacements = Log2(m_capacity - m_count);
                for (; index != INVALID_INDEX && displacements--; )
                {
                    index = TryEmptyCloser(neighborHoodSlot, index);
                    const auto slotDifference = GetSlotDifference(index, neighborHoodSlot);
                    if (slotDifference < MAX_HOP)
                        return index;
                }
            }

            Grow();
            return GetEmptyNeighbor(neighborHoodHash, HashToIndex(neighborHoodHash));
        }

        template <typename Key, typename Hasher, typename Notifier>
        template <typename KeyFrom, typename UserDataFrom>
        void THashHopscotchHelper<Key, Hasher, Notifier>::SetInternal (const Key & key, UserData userData)
        {
            SetInternal<KeyFrom, UserDataFrom>(key, GetHash(key), userData);
        }

        template <typename Key, typename Hasher, typename Notifier>
        template <typename KeyFrom, typename UserDataFrom>
        void THashHopscotchHelper<Key, Hasher, Notifier>::SetInternal (const Key & key, Hash::HashCode hash, UserData userData)
        {
            // If the slot already exists
            const auto existingIndex = FindIndex(key, hash, HashToIndex(hash));
            if (existingIndex == INVALID_INDEX)
                SetInternalNew<KeyFrom, UserDataFrom>(key, hash, userData);
            else
                OnSetExisting(existingIndex, userData);
        }

        template <typename Key, typename Hasher, typename Notifier>
        template <typename KeyFrom, typename UserDataFrom>
        void THashHopscotchHelper<Key, Hasher, Notifier>::SetInternalNew (const Key & key, Hash::HashCode hash, UserData userData)
        {
            const auto newIndex  = GetEmptyNeighbor(hash, HashToIndex(hash));
            const auto hashIndex = HashToIndex(hash); // hashIndex may have changed as GetEmptyNeighbor can cause growth.
            m_hashes[newIndex] = hash;
            Utility::ConstructFrom(m_keys[newIndex], KeyFrom(key));
            Notifier::OnSetNew(newIndex, UserDataFrom(userData));
            ++m_count;
            const auto slotDifference = GetSlotDifference(newIndex, hashIndex);
            m_hops[hashIndex].SetBit(slotDifference);
        }

        template <typename Key, typename Hasher, typename Notifier>
        size_t THashHopscotchHelper<Key, Hasher, Notifier>::FindIndex (const Key & key) const
        {
            const auto hash      = GetHash(key);
            const auto hashIndex = HashToIndex(hash);
            return FindIndex(key, hash, hashIndex);
        }

        template <typename Key, typename Hasher, typename Notifier>
        size_t THashHopscotchHelper<Key, Hasher, Notifier>::FindIndex (
            const Key &    key, 
            Hash::HashCode hash, 
            size_t         hashIndex
        ) const {

            static const auto TEST_KEY_ONLY = std::is_fundamental<Key>::value || std::is_pointer<Key>::value || std::is_enum<Key>::value;

            if (m_hops)
            {
                const auto hop = m_hops[hashIndex];
                for (const auto index : hop)
                {
                    const auto testIndex = PutInRange(hashIndex + index);
                    if ((TEST_KEY_ONLY || m_hashes[testIndex] == hash) && m_keys[testIndex] == key)
                        return testIndex;
                }
            }

            return INVALID_INDEX;
        }

        template <typename Key, typename Hasher, typename Notifier>
        typename THashHopscotchHelper<Key, Hasher, Notifier>::SlotIndex THashHopscotchHelper<Key, Hasher, Notifier>::FindNextEmpty (SlotIndex startIndex) const
        {
            const auto beginHash = m_hashes + startIndex;
            const auto endHash   = m_hashes + m_capacity;

            // Look forward from the start
            for (auto hash = beginHash; hash != endHash; ++hash) if (!*hash) return hash - m_hashes;
    
            // Wasn't found at the start, so jump back to the front and start looking.
            for (auto hash = m_hashes; hash != beginHash; ++hash) if (!*hash) return hash - m_hashes;

            // No empty slot was found at all.
            return INVALID_INDEX;
        }

        template <typename Key, typename Hasher, typename Notifier>
        size_t THashHopscotchHelper<Key, Hasher, Notifier>::GetSlotDifference (size_t high, size_t low) const
        {
            const auto falseIndex = high >= low ? high : high + m_capacity;
            return falseIndex - low;
        }

        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::Grow ()
        {
            GrowToCapacity(m_capacity * 2);
        }

        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::GrowToCapacity (size_t capacity)
        {
            THashHopscotchHelper builder;
            builder.InitCapacity(capacity);

            for (auto index = m_capacity; index--; )
            {
                if (IsSlotOccupied(index))
                    builder.SetInternalNew<Key &&, Notifier::UserDataGrow>(m_keys[index], m_hashes[index], Notifier::OnRequestUserData(index));
            }
            Swap(*this, builder);
        }

        template <typename Key, typename Hasher, typename Notifier>
        bool THashHopscotchHelper<Key, Hasher, Notifier>::IsSlotOccupied (size_t index) const
        {
            return m_hashes[index] != 0;
        }

        template <typename Key, typename Hasher, typename Notifier>
        bool THashHopscotchHelper<Key, Hasher, Notifier>::IsSlotEmpty (size_t index) const
        {
            return m_hashes[index] == 0;
        }

        template <typename Key, typename Hasher, typename Notifier>
        bool THashHopscotchHelper<Key, Hasher, Notifier>::Contains (const Key & key) const
        {
            return FindIndex(key) != INVALID_INDEX;
        }

        template <typename Key, typename Hasher, typename Notifier>
        void THashHopscotchHelper<Key, Hasher, Notifier>::MoveToSlot (SlotIndex oldSlot, SlotIndex newSlot)
        {
            ASSERT(oldSlot != newSlot)(oldSlot, newSlot);
            ASSERT(!IsSlotEmpty(oldSlot))(oldSlot);
            ASSERT(IsSlotEmpty(newSlot))(newSlot);
    
            // Update the hops
            const auto hashIndex    = HashToIndex(m_hashes[oldSlot]);
            auto &     hops         = m_hops[hashIndex];
            const auto hopOffsetOld = GetSlotDifference(oldSlot, hashIndex);
            const auto hopOffsetNew = GetSlotDifference(newSlot, hashIndex);
            ASSERT(hopOffsetOld < MAX_HOP)(hopOffsetOld, MAX_HOP);
            ASSERT(hopOffsetNew < MAX_HOP)(hopOffsetNew, MAX_HOP);
            hops.ClearBit(hopOffsetOld);
            hops.SetBit(hopOffsetNew);

            Utility::ConstructFrom(m_keys[newSlot], std::move(m_keys[oldSlot]));
            Utility::Destruct(m_keys[oldSlot]);

            Notifier::OnMove(oldSlot, newSlot);

            m_hashes[newSlot] = m_hashes[oldSlot];
            m_hashes[oldSlot] = 0;
        }

        // TSet implementation
        template <typename Key, typename Hasher>
        class THashSetHopscotch
        {
        private: // Internal management type
            class Notifier
            {
            public: // Types
                typedef int UserData;
                typedef int UserDataGrow;
    
            public: // Functions
                void OnDestroy (size_t index)                        { REF(index); }
                void OnErase (size_t index)                          { REF(index); }
                void OnInitCapacity (size_t startCapacity)           { REF(startCapacity); }
                void OnMove (size_t oldIndex, size_t newIndex)       { REF(oldIndex, newIndex); }
                void OnSetExisting (size_t index, UserData userData) { REF(index, userData); }
                void OnSetNew (size_t index, UserData userData)      { REF(index, userData); }
                int  OnRequestUserData (size_t index) const          { REF(index); return 0; }
                friend void Swap (Notifier & lhs, Notifier & rhs)    { REF(lhs, rhs); }
            };

        private: // Internal data.
            typedef THashHopscotchHelper<Key, Hasher, Notifier> SetType;
            SetType m_set;

        public: // Basic access
            size_t Capacity () const                { return m_set.Capacity();    }
            size_t Count () const                   { return m_set.Count();       }
            bool   Contains (const Key & key) const { return m_set.Contains(key); }

        public: // Erase
            void Erase (const Key & key) { m_set.Erase(key); }

        public: // Set
            void Add (const Key & key)  { m_set.SetInternal<decltype(key)>(key, 0); }
            void Add (Key && key)       { m_set.SetInternal<decltype(key)>(key, 0); }
        };

        // TMap implementation
        template <typename Key, typename Value, typename Hasher>
        class THashMapHopscotch
        {
        private: // Internal management type
            class Notifier
            {
                Value * m_values = nullptr;

            public:
                ~Notifier () { Mem::Free(m_values); }

            public: // Types
                typedef const Value & UserData;
                typedef Value &&      UserDataGrow;

            public: // Functions
                void     OnDestroy (size_t index)                                 { Utility::Destruct(m_values[index]); }
                void     OnErase (size_t index)                                   { Utility::Destruct(m_values[index]); }
                void     OnInitCapacity (size_t startCapacity)                    { m_values = Mem::Alloc<Value>(startCapacity); }
                void     OnMove (size_t oldIndex, size_t newIndex)                { Utility::ConstructFrom(m_values[newIndex], std::move(m_values[oldIndex])); Utility::Destruct(m_values[oldIndex]); }
                void     OnSetExisting (size_t index, const Value & value)        { m_values[index] = value; }
                void     OnSetExisting (size_t index, Value && value)             { m_values[index] = std::forward<Value>(userData); }
                void     OnSetNew (size_t index, const Value & userData)          { Utility::ConstructFrom(m_values[index], userData); }
                void     OnSetNew (size_t index, Value && userData)               { Utility::ConstructFrom(m_values[index], std::forward<Value>(userData)); }
                Value &  OnRequestUserData (size_t index) const                   { return m_values[index]; }
                friend void Swap (Notifier & lhs, Notifier & rhs)                 { Utility::Swap(lhs.m_values, rhs.m_values); }
            };

        private: // Internal data.
            typedef THashHopscotchHelper<Key, Hasher, Notifier> SetType;
            SetType m_set;

        public: // Basic access
            size_t Capacity () const                { return m_set.Capacity();    }
            size_t Count () const                   { return m_set.Count();       }
            bool   Contains (const Key & key) const { return m_set.Contains(key); }

        public: // Erase
            void Erase (const Key & key) { m_set.Erase(key); }

        public: // Get
            const Value & Get (const Key & key) const { return m_set.GetUserData(key); }

        public: // Set
            void Set (const Key & key, const Value & value) { m_set.SetInternal<decltype(key), decltype(value)>(key, value); }
            void Set (Key && key, const Value & value)      { m_set.SetInternal<decltype(key), decltype(value)>(key, value); }
            void Set (const Key & key, Value && value)      { m_set.SetInternal<decltype(key), decltype(value)>(key, value); }
            void Set (Key && key, Value && value)           { m_set.SetInternal<decltype(key), decltype(value)>(key, value); }
        };
    }
}

#endif // Include guard