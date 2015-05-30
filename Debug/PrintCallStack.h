#ifndef PRINT_CALL_STACK_48879b1829b443cabb6c4ecdc8f157a1
#define PRINT_CALL_STACK_48879b1829b443cabb6c4ecdc8f157a1

#if ENABLE_PRINT_CALLSTACK

#   ifndef _WIN32
#       error "Platform not supported"
#   endif

#   define PRINT_CALLSTACK zs::debug::PrintCallstack()


#include <windows.h>
#include <dbghelp.h>
#include <mutex>

#pragma comment(lib, "dbghelp.lib")

namespace zs
{
    namespace debug
    {
        // TODO : ZS : Make this SymInitialize only once and cleanup when done, possibly though a static variable
        // TODO : ZS : Serialize writes with a critical section
        inline void PrintCallstack ()
        {
            static const unsigned MAX_NUM_FRAMES  = 100;
            static const unsigned MAX_NAME_LENGTH = 255;
            static struct SymInitializer
            {
                SymInitializer () { SymSetOptions(SYMOPT_LOAD_LINES); SymInitialize(GetCurrentProcess(), NULL, TRUE); }
                ~SymInitializer () { SymCleanup(GetCurrentProcess()); }
            } symInitializer;
            static std::mutex lock;

            lock.lock();
            HANDLE process = GetCurrentProcess();
            
            void * stack[MAX_NUM_FRAMES];
            unsigned frames = CaptureStackBackTrace(1, MAX_NUM_FRAMES, stack, NULL);

            char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_NAME_LENGTH + 1];
            SYMBOL_INFO * symbol = reinterpret_cast<SYMBOL_INFO *>(symbolBuffer);
            symbol->MaxNameLen   = MAX_NAME_LENGTH;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(line);

            printf("%d frames\n");
            for(unsigned i = 0; i < frames; ++i)
            {
                symbol->NameLen = 0;
                SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
                DWORD displacement = 0;
                SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &displacement, &line);
                    
                if (symbol->NameLen)
                    printf("%2d: %s:%d - %s\n", frames - i - 1, line.FileName, line.LineNumber, symbol->Name);
                else
                    printf("%2d: No symbols loaded (typically an external module)\n", frames - i - 1);
            }
            lock.unlock();
        }
    }
}

#else
#   define PRINT_CALLSTACK ((void)0)
#endif

#endif