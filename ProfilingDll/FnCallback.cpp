#include "pch.h"
#include "FnCallback.h"

EXTERN_C void _stdcall FnEnter(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo)
{
	tracer->FunctionEnter(funcID, clientData, func, argumentInfo);
}

EXTERN_C void _stdcall FnLeave(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo)
{
	tracer->FunctionLeave(funcID, clientData, func, argumentInfo);
}

EXTERN_C void _stdcall FnTail(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func)
{
	tracer->FunctionTail(funcID, clientData, func);
}