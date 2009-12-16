#include "stdafx.h"
#include "DebugHelp.h"
#include "DebugFx.h"
#include "MemLeakDetector.h"

namespace DbgLib
{

extern CMemLeakDetector g_MemLeakDetector;

//////////////////////////////////////////////////////////////////////////
EXCEPTION_CALLBACK CDebugFx::m_ExceptionCallback = NULL;
void* CDebugFx::m_Param = NULL;

#if defined(PLATFORM_WINDOWS)
LPTOP_LEVEL_EXCEPTION_FILTER CDebugFx::m_PrevUnhandledExceptionHandler = NULL;
#endif

CSymbolResolver g_SymbolResolver(CSymbolResolver::GetDefaultSymbolSearchPath().c_str());

//////////////////////////////////////////////////////////////////////////
size_t CDebugFx::GenerateStackTrace(tstring* p_StackTrace, const size_t p_BufferSize)
{
	uintx* callstack = new uintx[p_BufferSize];
	#ifdef _DEBUG
	const int numCallsToSkip = 2;
	#else
	const int numCallsToSkip = 0;
	#endif
	uintx callstackSize = CDebugHelp::DoStackWalk(callstack, p_BufferSize, numCallsToSkip);

	for (uintx stackIndex=0; stackIndex<callstackSize; ++stackIndex)
	{
		// we have not cached any module names, so we set them null
		const tchar* moduleName = NULL;
		ModuleHandle hModule = NULL;

		// resolve debug symbols
		tstring symFunc;
		tstring symFile;
		uintx symLine;
		g_SymbolResolver.ResolveSymbol(callstack[stackIndex], hModule, moduleName, symFunc, symFile, symLine);

		// set unknown symbols
		if(symFunc.empty())
			symFunc = _T("Unknown Function");
		if(symFile.empty())
			symFile = _T("Unknown File");	

		// print symbols
		tostringstream stackInfoStream;
		stackInfoStream << symFunc;

		// align the file name a bit
		const int fileNamePosition = 45; // number of characters before the file name (alignment from left)
		stackInfoStream.fill(_T(' '));
		if(stackInfoStream.tellp() < fileNamePosition)
			stackInfoStream << std::setw(fileNamePosition - stackInfoStream.tellp()) << _T(" ") << std::setw(1);

		#if defined(PLATFORM_WINDOWS)
		const char pathSeparator = '\\';
		#else
		const char pathSeparator = '/';
		#endif

		// write the file name and line
		stackInfoStream << _T("(") << symFile.substr(symFile.find_last_of(pathSeparator)+1) << _T(":") << std::dec << symLine << _T(")");

		// save the string to the buffer
		p_StackTrace[stackIndex].assign(stackInfoStream.str());
	}

	delete[] callstack;
	return callstackSize;
}

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SetExceptionHandler(bool p_Enable)
{
#if defined(PLATFORM_WINDOWS)
	if(p_Enable)
		m_PrevUnhandledExceptionHandler = SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	else
		SetUnhandledExceptionFilter(m_PrevUnhandledExceptionHandler);
#else
	// set alternative stack to have some space left in case of a stack overflow
	struct sigaltstack sas;
	if(p_Enable)
	{
		// we have to disable the memleak detector here temporarily so that our altstack memory 
		// won't be reported as a memory leak
		g_MemLeakDetector.Disable();
		sas.ss_sp = malloc(56200);
		g_MemLeakDetector.Enable();
		sas.ss_size = 56200;
		sas.ss_flags = 0;
	}
	else
		sas.ss_flags = SS_DISABLE;
	if(sigaltstack(&sas, NULL) < 0)
		printf("sigaltstack failed: %d\n", errno);

	// set signal handler for abort signal
	struct sigaction sa;
	sa.sa_handler = (p_Enable ? SigAbortHandler : SIG_DFL);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGABRT, &sa, 0) == -1)
		printf("sigaltstack failed: %d\n", errno);

	// set signal handler for segmentation fault signal
	sa.sa_handler = (p_Enable ? SigSegvHandler : SIG_DFL);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_ONSTACK;
	if(sigaction(SIGSEGV, &sa, 0) == -1)
		printf("sigaltstack failed: %d\n", errno);
#endif
}

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SetExceptionCallback(EXCEPTION_CALLBACK p_Func, void* p_Param)
{
	m_ExceptionCallback = p_Func;
	m_Param = p_Param;
}

//////////////////////////////////////////////////////////////////////////
CMemLeakDetector* CDebugFx::GetMemLeakDetector()
{
	return &g_MemLeakDetector;
}

#if defined(PLATFORM_WINDOWS)
//////////////////////////////////////////////////////////////////////////
LONG WINAPI CDebugFx::UnhandledExceptionHandler(EXCEPTION_POINTERS* p_ExcInfo)
{
	// if a stack overflow occured - run our handler in a thread
	if(p_ExcInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
	{
		HANDLE hThread = CreateThread(NULL, 102400, UnhandledExceptionHandlerFunc, p_ExcInfo, 0, NULL);
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}
	else
		UnhandledExceptionHandlerFunc(p_ExcInfo);

	return EXCEPTION_CONTINUE_SEARCH;
}

//////////////////////////////////////////////////////////////////////////
DWORD WINAPI CDebugFx::UnhandledExceptionHandlerFunc(LPVOID p_Param)
{
	EXCEPTION_POINTERS* excInfo = reinterpret_cast<EXCEPTION_POINTERS*>(p_Param);

	tstring dumpFilePath;

#if defined(USE_WER)
	// create a new report
	dumpFilePath = CDebugHelp::CreateWERReport(excInfo);	
#else
	// create a dump
	dumpFilePath = CDebugHelp::CreateMiniDump(excInfo, (excInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) ? true : false);
#endif

	// call the user-defined callback method
	if(m_ExceptionCallback)
	{
		m_ExceptionCallback(dumpFilePath, m_Param);
	}

	return 0;
}

#else

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SigAbortHandler(int p_Signal)
{
	// create a dump
	tstring dumpFilePath = CDebugHelp::CreateMiniDump(false);

	// call the user-defined callback method
	if(m_ExceptionCallback)
		m_ExceptionCallback(dumpFilePath, m_Param);

	// reset to defaucmalt signal handler and core dump
	signal(SIGABRT, SIG_DFL);
}

//////////////////////////////////////////////////////////////////////////
void CDebugFx::SigSegvHandler(int p_Signal)
{
	// create a dump
	tstring dumpFilePath = CDebugHelp::CreateMiniDump(false);

	// call the user-defined callback method
	if(m_ExceptionCallback)
		m_ExceptionCallback(dumpFilePath, m_Param);

	// reset to default signal handler and core dump
	signal(SIGSEGV, SIG_DFL);
}


#endif

} // namespace DbgLib
