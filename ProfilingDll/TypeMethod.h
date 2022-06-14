#pragma once

#include "TypeInfo.h"

class TypeMethod : public TypeInfo
{
public:
    sig_elem_type methodType = ELEMENT_TYPE_UNKNOW;
    
    TypeInfo* returnType = NULL;
    TypeInfo **paramTypes = NULL;

    int paramCount = 0;
    int genericParamCount = 0;

    bool isGeneric = false;

    TypeMethod(sig_elem_type methodType);

    ~TypeMethod()
    {
            delete returnType;
            delete[] paramTypes;
    }
};