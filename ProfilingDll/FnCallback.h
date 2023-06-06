#pragma once

#include "pch.h"
#include "Tracer.h"

extern Tracer* tracer;


//EXTERN_C void _stdcall FnEnter(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO* argumentInfo);
//EXTERN_C void _stdcall FnLeave(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE* argumentInfo);
//EXTERN_C void _stdcall FnTail(FunctionID funcID, UINT_PTR clientData, COR_PRF_FRAME_INFO func);

EXTERN_C void _stdcall FnEnter(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);
EXTERN_C void _stdcall FnLeave(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);
EXTERN_C void _stdcall FnTail(FunctionID funcID, COR_PRF_ELT_INFO eltInfo);