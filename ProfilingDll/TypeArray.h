#pragma once

#include "TypeInfo.h"

class TypeArray : public TypeInfo
{
public:
	bool isSzArray = false;

	sig_count rank = 0;

	sig_count numsizes = 0;
	sig_count* sizes = NULL;
	
	sig_count numLoBounds = 0;
	sig_count* loBounds = NULL;

	TypeArray();


	~TypeArray()
	{
		delete[] sizes;
		delete[] loBounds;
	}
};

