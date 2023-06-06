#include "pch.h"
#include "FnCallback.h"

EXTERN_C void _stdcall FnEnter(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	tracer->FunctionEnter(funcID, eltInfo);
}

EXTERN_C void _stdcall FnLeave(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	tracer->FunctionLeave(funcID, eltInfo);
}

EXTERN_C void _stdcall FnTail(FunctionID funcID, COR_PRF_ELT_INFO eltInfo)
{
	tracer->FunctionTail(funcID, eltInfo);
}