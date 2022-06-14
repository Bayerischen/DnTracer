#pragma once
#include "TypeInfo.h"

class SigProperty : public TypeInfo
{
public:
	TypeInfo* propertyType = NULL;
	TypeInfo** paramTypes = NULL;

	SigProperty(sig_count param_count, TypeInfo* propertyType);
};

