#pragma once
//see https://docs.microsoft.com/en-us/archive/blogs/davbr/sample-a-signature-blob-parser-for-your-profiler?WT.mc_id=DT-MVP-5003325

#include "SigDef.h"
#include "TypeMethod.h"
#include "TypeArray.h"
#include "SigField.h"
#include "SigLocals.h"
#include "SigProperty.h"

class SigParser
{
private:
    sig_byte* pbBase;
    sig_byte* pbCur;
    sig_byte* pbEnd;

public:
    TypeInfo* Parse(sig_byte* blob, sig_count len);
    void Reset();

private:
    bool ParseByte(sig_byte* pbOut);
    bool ParseNumber(sig_count* pOut);
    bool ParseTypeDefOrRefEncoded(sig_index_type* pOutIndexType, sig_index* pOutIndex);

    TypeMethod* ParseMethod(sig_elem_type);
    TypeInfo* ParseField(sig_elem_type);
    SigProperty* ParseProperty(sig_elem_type);
    SigLocals* ParseLocals(sig_elem_type);
    TypeInfo* ParseLocal();
    bool ParseOptionalCustomMods();
    bool ParseOptionalCustomModsOrConstraint();
    bool ParseCustomMod();
    TypeInfo* ParseRetType();
    TypeInfo* ParseType();
    TypeInfo* ParseParam();
    bool ParseArrayShape(TypeArray* typeArray);
};