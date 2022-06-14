#pragma once

#include "TypeInfo.h"

class SigField : public TypeInfo
{
public:
	SigField();

	TypeInfo* fieldType = NULL;
};