#pragma once

#include "pch.h"
#include "Logger.h"
#include "SigParser.h"

class Tracer
{
public:
	ModuleID mainModuleID = -1;

	Tracer(ICorProfilerInfo3* info);
	void Init();

	void ModuleLoaded(ModuleID moduleID);

	void FunctionEnter(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);
	void FunctionLeave(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);
	void FunctionTail(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);

private:
	ICorProfilerInfo3* iCorProfilerInfo;

	Logger* logger;
	SigParser* sigParser;

	int callLevel = -1;
	int lastUserSpaceLevel = -10;
	bool mainModuleEntryCalled = false;
};