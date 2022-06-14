#pragma once

#include "SigDef.h"

class TypeInfo
{
public:
	sig_elem_type type = ELEMENT_TYPE_UNKNOW;

	//if the type is simple type, ignore this
	//if the type is ptr , this is the ref type
	//if the type is array/szarray, this is the element type
	TypeInfo* refType = NULL;

	TypeInfo(sig_elem_type type);
	bool TypeInfo::IsSimpleType();
	std::string TypeInfo::GetTypeName();

	//since this is virtual, the child's destructor will be called firstly
	//so don't delete refType in child's destructor, or double free might orrcured
	virtual ~TypeInfo()
	{
		delete refType; //delete NULL is safe :D
	}
};
