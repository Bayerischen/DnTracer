#include "pch.h"
#include "SigProperty.h"

SigProperty::SigProperty(sig_count param_count, TypeInfo* propertyType) : TypeInfo(SIG_PROPERTY)
{
	this->propertyType = propertyType;
	paramTypes = new TypeInfo *[param_count];
}