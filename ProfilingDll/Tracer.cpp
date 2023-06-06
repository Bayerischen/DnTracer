#include "pch.h"
#include <iostream>
#include "FnCallback.h"
#include "Utils.h"
#include "TypeInfo.h"
#include "Tracer.h"

using namespace std;

Tracer::Tracer(ICorProfilerInfo3* info)
{
	logger = new Logger();
	sigParser = new SigParser();
	iCorProfilerInfo = info;
}


void Tracer::Init()
{
	Utils::RedirectIOToConsole();

	iCorProfilerInfo->SetEventMask(COR_PRF_MONITOR_ENTERLEAVE| COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL | COR_PRF_ENABLE_FRAME_INFO | COR_PRF_MONITOR_ASSEMBLY_LOADS | COR_PRF_MONITOR_MODULE_LOADS);
	iCorProfilerInfo->SetEnterLeaveFunctionHooks3WithInfo((FunctionEnter3WithInfo*)&FnEnter, (FunctionLeave3WithInfo*)FnLeave, (FunctionTailcall3WithInfo*)FnTail);

	iCorProfilerInfo->GetStringLayout2(&Utils::strLenOffset, &Utils::strBufOffset);
}


void Tracer::ModuleLoaded(ModuleID moduleID)
{
	wchar_t* name = NULL;
	if (Utils::GetModuleNameById(iCorProfilerInfo, moduleID, &name) <= 0)
		return;

	logger->WriteFormat("\nModule loaded: %ls\n\n", name);

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


void Tracer::FunctionEnter(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	callLevel++;

	//get id of module and class
	ClassID classID;
	ModuleID moduleID;
	iCorProfilerInfo->GetFunctionInfo(funcID, &classID, &moduleID, NULL);

	int levelDiff = callLevel - lastUserSpaceLevel;
	if (moduleID == mainModuleID || levelDiff == 0 || levelDiff == 1) // in the main module
	{
		//get argument info
		COR_PRF_FRAME_INFO frameInfo;
		ULONG argInfoSize = 0;
		COR_PRF_FUNCTION_ARGUMENT_INFO* argInfo = NULL;
		HRESULT result = iCorProfilerInfo->GetFunctionEnter3Info(funcID, eltInfo, &frameInfo, &argInfoSize, argInfo); //get pArgumentInfo size
		argInfo = (COR_PRF_FUNCTION_ARGUMENT_INFO*)malloc(argInfoSize);
		if (argInfo == NULL)
			return;
		HRESULT result2 = iCorProfilerInfo->GetFunctionEnter3Info(funcID, eltInfo, &frameInfo, &argInfoSize, argInfo);

		//get method token and metadata
		mdMethodDef methodToken;
		IMetaDataImport* metadata;
		iCorProfilerInfo->GetTokenAndMetaDataFromFunction(funcID, IID_IMetaDataImport, (IUnknown**)&metadata, &methodToken);

		//get class name, method name, method signature, signature size
		wchar_t className[128] = { 0 };
		wchar_t methodName[128] = { 0 };
		PCCOR_SIGNATURE signature;
		ULONG sigSize;
		ULONG methodNameLen;
		HRESULT methodPropResult = metadata->GetMethodProps(methodToken, NULL, methodName, sizeof(methodName) / sizeof(wchar_t), &methodNameLen, NULL, &signature, &sigSize, NULL, NULL);
		if (methodPropResult != S_OK)
			return;
		ULONG  classNameLen = Utils::GetClassNameByClassId(iCorProfilerInfo, classID, className, sizeof(className) / sizeof(wchar_t));
		if (classNameLen == -1)
			return;

		//parse the method signature
		TypeInfo* typeInfo = sigParser->Parse((sig_byte*)signature, sigSize);
		sigParser->Reset();
		if (typeInfo == NULL || typeInfo->type != ELEMENT_TYPE_FNPTR)
			return;
		TypeMethod* typeMethod = (TypeMethod*)typeInfo;

		//whether print prefix tabs or not
		char prefix[64] = { '\t' };
		if(callLevel >= 0)
			prefix[callLevel] = 0;

		//initialize log
		size_t logBufSize = callLevel + 1 + classNameLen + 1 + methodNameLen + 16;
		char* logBuf = new char[logBufSize];
		snprintf(logBuf, logBufSize, "%s%ls.%ls(", prefix, className, methodName);
		string log = logBuf;

		if (argInfo->numRanges > 0)
		{
			//get parameter tokens
			//HCORENUM hEnum = NULL;
			//ULONG paramTokenCount;
			//mdParamDef* paramTokens = new mdParamDef[argInfo->numRanges];
			//metadata->EnumParams(&hEnum, methodToken, paramTokens, argInfo->numRanges, &paramTokenCount); //get parameter token count
			/*for (int i = 0; i < argInfo->numRanges && i < paramTokenCount; i++)
			{
				COR_PRF_FUNCTION_ARGUMENT_RANGE range = argInfo->ranges[i];

				ULONG paramIdx;
				wchar_t paramName[128];
				metadata->GetParamProps(paramTokens[i], NULL, &paramIdx, paramName, 128, NULL, NULL, NULL, NULL, NULL);
				printf("%d:%s, ", paramIdx, paramName);
			}*/

			for (ULONG i = 0; i < argInfo->numRanges && i < typeMethod->paramCount; i++)
			{
				TypeInfo* paramType = typeMethod->paramTypes[i];
				COR_PRF_FUNCTION_ARGUMENT_RANGE argRange = argInfo->ranges[i];

				string typeName = paramType->GetTypeName();
				char argValue[128] = { 0 };
				Utils::GetParamterValueStr(&argRange, paramType->type, argValue, sizeof(argValue));
				
				log.append(typeName);
				log.append(1, ':');
				log.append(argValue);
				if (i != typeMethod->paramCount - 1)
					log.append(", ");
			}

			delete[] logBuf;
		}
		
		log.append(1, ')');
		logger->WriteLine(log.c_str(), log.length());

		metadata->Release();
		free(argInfo);
	}

	if (moduleID == mainModuleID)
		lastUserSpaceLevel = callLevel;	
}


void Tracer::FunctionLeave(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	//get id of module and class
	ClassID classID;
	ModuleID moduleID;
	iCorProfilerInfo->GetFunctionInfo(funcID, &classID, &moduleID, NULL);

	int levelDiff = callLevel - lastUserSpaceLevel;
	if (moduleID == mainModuleID || levelDiff == 0 || levelDiff == 1) // in the main module
	{
		//get return value info
		COR_PRF_FRAME_INFO frameInfo;
		COR_PRF_FUNCTION_ARGUMENT_RANGE retRange;
		HRESULT result2 = iCorProfilerInfo->GetFunctionLeave3Info(funcID, eltInfo, &frameInfo, &retRange);

		//get method token and metadata
		mdMethodDef methodToken;
		IMetaDataImport* metadata;
		iCorProfilerInfo->GetTokenAndMetaDataFromFunction(funcID, IID_IMetaDataImport, (IUnknown**)&metadata, &methodToken);

		//get class name, method name
		wchar_t className[128] = { 0 };
		wchar_t methodName[128] = { 0 };
		PCCOR_SIGNATURE signature;
		ULONG sigSize;
		ULONG methodNameLen;
		HRESULT methodPropResult = metadata->GetMethodProps(methodToken, NULL, methodName, sizeof(methodName) / sizeof(wchar_t), &methodNameLen, NULL, &signature, &sigSize, NULL, NULL);
		if (methodPropResult != S_OK)
			return;
		ULONG  classNameLen = Utils::GetClassNameByClassId(iCorProfilerInfo, classID, className, sizeof(className) / sizeof(wchar_t));
		if (classNameLen == -1)
			return;

		//whether print prefix tabs or not
		char prefix[64] = { '\t' };
		if (callLevel >= 0)
			prefix[callLevel] = 0;

	
		//try read string from return value
		char* logBuf = NULL;
		int logLen = 0;
		wchar_t retStr[128] = { 0 };
		int bytesRead = Utils::TryGetStrFromRetRange(&retRange, retStr, sizeof(retStr));
		if (bytesRead != -1)
		{
			size_t logBufSize = callLevel + 1 + classNameLen + 1 + methodNameLen + 16 + bytesRead;
			logBuf = new char[logBufSize];
			logLen = snprintf(logBuf, logBufSize, "%s%ls.%ls returns %ls", prefix, className, methodName, retStr);
		}
		else
		{
			if (retRange.length > 0)
			{
				size_t logBufSize = callLevel + 1 + classNameLen + 1 + methodNameLen + 16 + retRange.length * 3;
				logBuf = new char[logBufSize];
				char* retHexStr = new char[retRange.length * 3];
				for (ULONG i = 0; i < retRange.length; i++)
					sprintf(retHexStr + i * 3, "%02x ", *(BYTE*)(retRange.startAddress + i));
				logLen = snprintf(logBuf, logBufSize, "%s%ls.%ls returns %s", prefix, className, methodName, retHexStr);
				delete[] retHexStr;
			}
		}

		if(logLen > 0)
			logger->WriteLine(logBuf, logLen);

		delete[] logBuf;

		metadata->Release();
	}


	callLevel--;
}


void Tracer::FunctionTail(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	/*wchar_t* funcName = NULL;
	int nameLen = Utils::GetFunctionNameById(iCorProfilerInfo, funcID, &funcName);
	printf("%d %d tail %ls\n", callLevel, lastUserSpaceLevel, funcName);
	callLevel--;*/
}
