#include "pch.h"
#include "TypeMethod.h"


TypeMethod::TypeMethod(sig_elem_type methodType) : TypeInfo(ELEMENT_TYPE_FNPTR)
{
	this->methodType = methodType;
}