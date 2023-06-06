#pragma warning (disable : 4996)

#pragma once

#include "pch.h"

typedef unsigned char sig_elem_type;

class Utils 
{
	//Get*Name* function returns -1 if failed, otherwise returns the actual length of output
public:
	static ULONG strLenOffset;
	static ULONG strBufOffset;

	static int GetClassNameByClassId(ICorProfilerInfo2* info, ClassID classId, wchar_t* buffer, ULONG bufSize);
	static int GetFunctionNameById(ICorProfilerInfo2* info, FunctionID functionId, wchar_t** output);
	static int GetModuleNameById(ICorProfilerInfo2* info, ModuleID moduleID, wchar_t** output);

	//get signature of function, returns -1 if failed. otherwise returns the size of signature
	static ULONG GetFunctionSig(ICorProfilerInfo2* info, FunctionID functionId, PCCOR_SIGNATURE* signature);

	static void GetParamterValueStr(COR_PRF_FUNCTION_ARGUMENT_RANGE* range, sig_elem_type type, char* outBuf, int bufSize);

	static const char* GetMethodTypeName(sig_elem_type type);

	//return -1 if it is not a valid string, otherwise return length of string in bytes
	static int TryGetStrFromRetRange(COR_PRF_FUNCTION_ARGUMENT_RANGE* retRange, wchar_t* buffer, ULONG bufSize);

	static void RedirectIOToConsole();
};