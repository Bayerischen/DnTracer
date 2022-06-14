#pragma once

#include "pch.h"
#include "Logger.h"
#include "SigParser.h"

class Tracer
{
public:
	ModuleID mainModuleID = -1;

	Tracer(ICorProfilerInfo2* info);
	void Init();

	void ModuleLoaded(ModuleID moduleID);

	void PrintFuncInfo(FunctionID funcID, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);
	void FunctionEnter(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);
	void FunctionLeave(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);
	void FunctionTail(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func);

private:
	ICorProfilerInfo2* iCorProfilerInfo;

	Logger* logger;
	SigParser* sigParser;

	int callLevel = -1;
	int lastUserSpaceLevel = -10;
	bool mainModuleEntryCalled = false;
};