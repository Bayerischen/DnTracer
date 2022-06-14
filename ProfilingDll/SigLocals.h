#pragma once

#include "TypeInfo.h"

class SigLocals : public TypeInfo
{
public:
	TypeInfo** locals = NULL;

	SigLocals(sig_count local_count);
};

