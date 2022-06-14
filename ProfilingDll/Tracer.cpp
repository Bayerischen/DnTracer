#include "pch.h"
#include <iostream>
#include "FnCallback.h"
#include "Utils.h"
#include "TypeInfo.h"
#include "Tracer.h"

using namespace std;

Tracer::Tracer(ICorProfilerInfo2* info)
{
	logger = new Logger();
	sigParser = new SigParser();
	iCorProfilerInfo = info;
}


void Tracer::Init()
{
	Utils::RedirectIOToConsole();

	iCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE| COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_MONITOR_ASSEMBLY_LOADS | COR_PRF_MONITOR_MODULE_LOADS);
	iCorProfilerInfo->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&FnEnter, (FunctionLeave2*)FnLeave, (FunctionTailcall2*)FnTail);

	//Sleep(4000);
}


void Tracer::ModuleLoaded(ModuleID moduleID)
{
	wchar_t* name = NULL;
	if (Utils::GetModuleNameById(iCorProfilerInfo, moduleID, &name) <= 0)
		return;

	logger->WriteLog("\nModule loaded: %ls\n", name);

	if ((name[0] == 'C' || name[0] == 'c') &&
		(name[3] == 'W' || name[3] == 'w') &&
		(name[4] == 'I' || name[4] == 'i') &&
		(name[5] == 'N' || name[5] == 'n') &&
		(name[6] == 'D' || name[6] == 'd') &&
		(name[7] == 'O' || name[7] == 'o') &&
		(name[8] == 'W' || name[8] == 'w') &&
		(name[9] == 'S' || name[9] == 's'))
		return;

	//save the main moduleID 
	mainModuleID = moduleID;

	delete name;
}


void Tracer::PrintFuncInfo(FunctionID funcID, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo)
{
	//get function name
	wchar_t* funcName = NULL;
	Utils::GetFunctionNameById(iCorProfilerInfo, funcID, &funcName);

	//whether print prefix tabs or not
	char empty = 0;
	char* prefix = &empty;
	if (callLevel > 0)
	{
		prefix = (char*)_malloca(callLevel + 1);
		if (prefix == NULL)
			return;
		for (int i = 0; i < callLevel; i++)
			prefix[i] = '\t';
		prefix[callLevel] = 0;
	}

	//read function signature and parse it
	PCCOR_SIGNATURE signature;
	ULONG sigSize = Utils::GetFunctionSig(iCorProfilerInfo, funcID, &signature);
	if (sigSize == -1)
		return;

	//parse the method signature
	TypeInfo* typeInfo = sigParser->Parse((sig_byte*)signature, sigSize);
	sigParser->Reset();
	if (typeInfo == NULL || typeInfo->type != ELEMENT_TYPE_FNPTR)
		return;
	TypeMethod* method = (TypeMethod*)typeInfo;

	//create log buffer
	//log: [prefix] [func_name] ( [param_type]:[param_value], ... )
	char* logBuf = new char[callLevel + 1 + wcslen(funcName) + 16];
	sprintf(logBuf, "%s%ls(", prefix, funcName);
	string log;
	log.assign(logBuf);
	delete[] funcName;
	_freea(prefix);

	//get paramter value by type
	char* paramValue = new char[256];
	for (int i = 0; i < method->paramCount; i++)
	{
		if (i >= (int)argumentInfo->numRanges)
			break;
	
		//get paramter value and type name
		TypeInfo* paramType = method->paramTypes[i];
		COR_PRF_FUNCTION_ARGUMENT_RANGE range = argumentInfo->ranges[i];
		Utils::GetParamterValueStr(&range, paramType->type, paramValue, 256);
		string typeName = paramType->GetTypeName();
		
		//append log
		log.append(typeName);
		log.append(1, ':');
		log.append(paramValue);

		if (i != method->paramCount - 1) // last param
			log.append(", ");
	}
	log.append(1, ')');
	logger->WriteLog(log.c_str());

	delete method;
}


void Tracer::FunctionEnter(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo)
{
	callLevel++;

	//wchar_t* funcName = NULL;
	//int nameLen = Utils::GetFunctionNameById(iCorProfilerInfo, funcID, &funcName);
	//printf("%d %d enter %ls\n", callLevel, lastUserSpaceLevel, funcName);

	//get module id
	ModuleID moduleID;
	iCorProfilerInfo->GetFunctionInfo2(funcID, func, NULL, &moduleID, NULL, 0, NULL, NULL);

	if (moduleID == mainModuleID) // in the main module
	{
		lastUserSpaceLevel = callLevel;
		PrintFuncInfo(funcID, argumentInfo);
	}
	else //not in the main module
	{
		int diff = callLevel - lastUserSpaceLevel;
		if(diff == 0 || diff == 1) //calling external function from the main module, print it
			PrintFuncInfo(funcID, argumentInfo);
	}
}


void Tracer::FunctionLeave(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo)
{
	//wchar_t* funcName = NULL;
	//int nameLen = Utils::GetFunctionNameById(iCorProfilerInfo, funcID, &funcName);
	//printf("%d %d leave %ls\n", callLevel, lastUserSpaceLevel, funcName);
	callLevel--;
}


void Tracer::FunctionTail(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func)
{
	//wchar_t* funcName = NULL;
	//int nameLen = Utils::GetFunctionNameById(iCorProfilerInfo, funcID, &funcName);
	//printf("%d %d tail %ls\n", callLevel, lastUserSpaceLevel, funcName);
	callLevel--;
}
