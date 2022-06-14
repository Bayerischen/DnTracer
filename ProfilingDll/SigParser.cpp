//see https://docs.microsoft.com/en-us/archive/blogs/davbr/sample-a-signature-blob-parser-for-your-profiler?WT.mc_id=DT-MVP-5003325
#include "pch.h"
#include "SigParser.h"

TypeInfo* SigParser::Parse(sig_byte* pb, sig_count cbBuffer)
{
    pbBase = pb;
    pbCur = pb;
    pbEnd = pbBase + cbBuffer;

    sig_elem_type elem_type;

    if (!ParseByte(&elem_type))
        return false;

    switch (elem_type & 0xf)
    {
    case SIG_METHOD_DEFAULT:  // default calling convention
    case SIG_METHOD_C:    // C calling convention
    case SIG_METHOD_STDCALL: // Stdcall calling convention
    case SIG_METHOD_THISCALL: // thiscall  calling convention
    case SIG_METHOD_FASTCALL: // fastcall calling convention
    case SIG_METHOD_VARARG:   // vararg calling convention
        return ParseMethod(elem_type);

    case SIG_FIELD:           // encodes a field
        return ParseField(elem_type);

    case SIG_LOCAL_SIG:       // used for the .locals directive
        return ParseLocals(elem_type);

    case SIG_PROPERTY:        // used to encode a property
        return ParseProperty(elem_type);

    default:
        // unknown signature
        break;
    }

    return NULL;
}


bool SigParser::ParseByte(sig_byte* pbOut)
{
    if (pbCur < pbEnd)
    {
        *pbOut = *pbCur;
        pbCur++;
        return true;
    }

    return false;
}


TypeMethod* SigParser::ParseMethod(sig_elem_type elem_type)
{
    // MethodDefSig ::= [[HASTHIS] [EXPLICITTHIS]] (DEFAULT|VARARG|GENERIC GenParamCount)
    //                    ParamCount RetType Param* [SENTINEL Param+]

    bool isGeneric = false;
    sig_count gen_param_count = 0;
    sig_count param_count = 0;

    //is generic method
    if (elem_type & SIG_GENERIC)
    {
        if (!ParseNumber(&gen_param_count))
            return NULL;

        isGeneric = true;
    }

    //parsing parameter count
    if (!ParseNumber(&param_count))
        return NULL;

    //parsing return type
    TypeInfo* retType = ParseRetType();
    if (retType == NULL)
        return NULL;

    bool fEncounteredSentinal = false;


    TypeMethod* method = new TypeMethod(elem_type);
    method->type = ELEMENT_TYPE_FNPTR;
    method->isGeneric = isGeneric;
    method->paramCount = param_count;
    method->genericParamCount = gen_param_count;
    method->returnType = retType;
    method->paramTypes = new TypeInfo * [param_count];

    for (sig_count i = 0; i < param_count; i++)
    {
        if (pbCur >= pbEnd)
            return NULL;

        if (*pbCur == ELEMENT_TYPE_SENTINEL)
        {
            if (fEncounteredSentinal)
                return NULL;

            fEncounteredSentinal = true;
            pbCur++;
        }

        TypeInfo* paramType = ParseParam();
        if(paramType == NULL)
            return NULL;
        method->paramTypes[i] = paramType;
    }

    return method;
}


TypeInfo* SigParser::ParseField(sig_elem_type elem_type)
{
    // FieldSig ::= FIELD CustomMod* Type

    if (!ParseOptionalCustomMods())
        return NULL;

    return ParseType();
}


SigProperty* SigParser::ParseProperty(sig_elem_type elem_type)
{
    // PropertySig ::= PROPERTY [HASTHIS] ParamCount CustomMod* Type Param*

    sig_count param_count;

    if (!ParseNumber(&param_count))
        return NULL;

    if (!ParseOptionalCustomMods())
        return NULL;

    TypeInfo* propertyType = ParseType();
    if(propertyType == NULL)
        return NULL;

    SigProperty* sigProperty = new SigProperty(param_count, propertyType);

    for (sig_count i = 0; i < param_count; i++)
    {
        TypeInfo* paramType = ParseParam();
        if(paramType == NULL)
            return NULL;
        sigProperty->paramTypes[i] = paramType;
    }

    return sigProperty;
}


SigLocals* SigParser::ParseLocals(sig_elem_type elem_type)
{
    //   LocalVarSig ::= LOCAL_SIG Count (TYPEDBYREF | ([CustomMod] [Constraint])* [BYREF] Type)+ 

    sig_count local_count;

    if (!ParseNumber(&local_count))
        return NULL;

    SigLocals* locals = new SigLocals(local_count);

    for (sig_count i = 0; i < local_count; i++)
    {
        TypeInfo* local = ParseLocal();
        if (local == NULL)
            return NULL;
        locals->locals[i] = local;
    }

    return locals;
}


TypeInfo* SigParser::ParseLocal()
{
    //TYPEDBYREF | ([CustomMod] [Constraint])* [BYREF] Type

    if (pbCur >= pbEnd)
        return NULL;

    if (*pbCur == ELEMENT_TYPE_TYPEDBYREF)
    {
        pbCur++;
        return new TypeInfo(ELEMENT_TYPE_TYPEDBYREF);
    }

    if (!ParseOptionalCustomModsOrConstraint())
        return NULL;

    if (pbCur >= pbEnd)
        return NULL;

    if (*pbCur == ELEMENT_TYPE_BYREF)
    {
        pbCur++;
        TypeInfo* local = new TypeInfo(ELEMENT_TYPE_BYREF);
        TypeInfo* refType = ParseType();
        if (refType == NULL)
            return NULL;
        local->refType = refType;
        return local;
    }

    return NULL;
}


bool SigParser::ParseOptionalCustomModsOrConstraint()
{
    for (;;)
    {
        if (pbCur >= pbEnd)
            return true;

        switch (*pbCur)
        {
        case ELEMENT_TYPE_CMOD_OPT:
        case ELEMENT_TYPE_CMOD_REQD:
            if (!ParseCustomMod())
                return false;
            break;

        case ELEMENT_TYPE_PINNED:
            //NotifyConstraint(*pbCur);
            pbCur++;
            break;

        default:
            return true;
        }
    }

    return false;
}


bool SigParser::ParseOptionalCustomMods()
{
    for (;;)
    {
        if (pbCur >= pbEnd)
            return true;

        switch (*pbCur)
        {
        case ELEMENT_TYPE_CMOD_OPT:
        case ELEMENT_TYPE_CMOD_REQD:
            if (!ParseCustomMod())
                return false;
            break;

        default:
            return true;
        }
    }

    return false;
}



bool SigParser::ParseCustomMod()
{
    sig_elem_type cmod = 0;
    sig_index index;
    sig_index_type indexType;

    if (!ParseByte(&cmod))
        return false;

    if (cmod == ELEMENT_TYPE_CMOD_OPT || cmod == ELEMENT_TYPE_CMOD_REQD)
    {
        if (!ParseTypeDefOrRefEncoded(&indexType, &index))
            return false;

        return true;
    }

    return false;
}


TypeInfo* SigParser::ParseParam()
{
    // Param ::= CustomMod* ( TYPEDBYREF | [BYREF] Type )

    if (!ParseOptionalCustomMods())
        return false;

    if (pbCur >= pbEnd)
        return false;

    if (*pbCur == ELEMENT_TYPE_TYPEDBYREF)
    {
        pbCur++;
        return new TypeInfo(ELEMENT_TYPE_TYPEDBYREF);
    }

    if (*pbCur == ELEMENT_TYPE_BYREF)
    {
        pbCur++;
        TypeInfo* typeInfo = new TypeInfo(ELEMENT_TYPE_BYREF);
        typeInfo->refType = ParseType();
        return typeInfo;
    }

    return ParseType();
}


TypeInfo* SigParser::ParseRetType()
{
    // RetType ::= CustomMod* ( VOID | TYPEDBYREF | [BYREF] Type )

    if (!ParseOptionalCustomMods())
        return NULL;

    if (pbCur >= pbEnd)
        return NULL;

    TypeInfo* typeInfo = NULL;

    switch (*pbCur)
    {
    case ELEMENT_TYPE_TYPEDBYREF:
    case ELEMENT_TYPE_VOID:
        typeInfo = new TypeInfo(*pbCur);
        pbCur++;
        return typeInfo;

    case ELEMENT_TYPE_BYREF:
        pbCur++;
        typeInfo = new TypeInfo(ELEMENT_TYPE_BYREF);
        typeInfo->refType = ParseType();
        if (typeInfo->refType == NULL)
            return NULL;
        return typeInfo;
    }

    return ParseType();
}

bool SigParser::ParseArrayShape(TypeArray* typeArray)
{
    sig_count rank;
    sig_count numsizes;
    sig_count size;

    // ArrayShape ::= Rank NumSizes Size* NumLoBounds LoBound*
    if (!ParseNumber(&rank))
        return false;
    typeArray->rank = rank;

    //parsing numsizes and each size
    if (!ParseNumber(&numsizes))
        return false;
    typeArray->numsizes = numsizes;
    typeArray->sizes = new sig_count[numsizes];

    for (sig_count i = 0; i < numsizes; i++)
    {
        if (!ParseNumber(&size))
            return false;
        typeArray->sizes[i] = size;
    }

    //parsing numLoBounds and each loBound
    if (!ParseNumber(&numsizes))
        return false;
    typeArray->numLoBounds = numsizes;
    typeArray->loBounds = new sig_count[numsizes];
    for (sig_count i = 0; i < numsizes; i++)
    {
        if (!ParseNumber(&size))
            return false;
        typeArray->loBounds[i] = size;
    }

    return true;
}

TypeInfo* SigParser::ParseType()
{
    /*
    Type ::= ( BOOLEAN | CHAR | I1 | U1 | U2 | U2 | I4 | U4 | I8 | U8 | R4 | R8 | I | U |
                    | VALUETYPE TypeDefOrRefEncoded
                    | CLASS TypeDefOrRefEncoded
                    | STRING
                    | OBJECT
                    | PTR CustomMod* VOID
                    | PTR CustomMod* Type
                    | FNPTR MethodDefSig
                    | FNPTR MethodRefSig
                    | ARRAY Type ArrayShape
                    | SZARRAY CustomMod* Type
                    | GENERICINST (CLASS | VALUETYPE) TypeDefOrRefEncoded GenArgCount Type *
                    | VAR Number
                    | MVAR Number

    */

    sig_elem_type elem_type;
    sig_index index;
    sig_mem_number number;
    sig_index_type indexType;

    TypeInfo* typeInfo;
    TypeInfo* arrayType;
    TypeArray* typeArray;

    if (!ParseByte(&elem_type))
        return false;

    switch (elem_type)
    {
    case  ELEMENT_TYPE_BOOLEAN:
    case  ELEMENT_TYPE_CHAR:
    case  ELEMENT_TYPE_I1:
    case  ELEMENT_TYPE_U1:
    case  ELEMENT_TYPE_U2:
    case  ELEMENT_TYPE_I2:
    case  ELEMENT_TYPE_I4:
    case  ELEMENT_TYPE_U4:
    case  ELEMENT_TYPE_I8:
    case  ELEMENT_TYPE_U8:
    case  ELEMENT_TYPE_R4:
    case  ELEMENT_TYPE_R8:
    case  ELEMENT_TYPE_I:
    case  ELEMENT_TYPE_U:
    case  ELEMENT_TYPE_STRING:
    case  ELEMENT_TYPE_OBJECT:
        // simple types
        return new TypeInfo(elem_type);

    case  ELEMENT_TYPE_PTR:
        // PTR CustomMod* VOID
        // PTR CustomMod* Type
        if (!ParseOptionalCustomMods())
            return NULL;

        if (pbCur >= pbEnd)
            return NULL;

        if (*pbCur == ELEMENT_TYPE_VOID)
        {
            pbCur++;
            return new TypeInfo(ELEMENT_TYPE_VOID);
        }

        typeInfo = new TypeInfo(ELEMENT_TYPE_PTR);
        typeInfo->refType = ParseType();
        if (typeInfo->refType == NULL)
            return NULL;
        return typeInfo;

    case  ELEMENT_TYPE_CLASS: //WIP
        // CLASS TypeDefOrRefEncoded
        if (!ParseTypeDefOrRefEncoded(&indexType, &index))
            return NULL;

        return new TypeInfo(ELEMENT_TYPE_CLASS);

    case  ELEMENT_TYPE_VALUETYPE: //WIP
        //VALUETYPE TypeDefOrRefEncoded
        if (!ParseTypeDefOrRefEncoded(&indexType, &index))
            return NULL;

        return new TypeInfo(ELEMENT_TYPE_VALUETYPE);

    case  ELEMENT_TYPE_FNPTR:
        // FNPTR MethodDefSig
        // FNPTR MethodRefSig
        if (!ParseByte(&elem_type))
            return NULL;

        return ParseMethod(elem_type);

    case  ELEMENT_TYPE_ARRAY:
        // ARRAY Type ArrayShape
        arrayType = ParseType();
        if (arrayType == NULL)
            return NULL;

        typeArray = new TypeArray();
        typeArray->refType = arrayType;
        if (!ParseArrayShape(typeArray))
            return NULL;
        return typeArray;

    case  ELEMENT_TYPE_SZARRAY:
        // SZARRAY CustomMod* Type
        if (!ParseOptionalCustomMods())
            return NULL;

        arrayType = ParseType();
        if (arrayType == NULL)
            return NULL;

        typeArray = new TypeArray();
        typeArray->isSzArray = true;
        typeArray->refType = arrayType;
        return typeArray;

    case  ELEMENT_TYPE_GENERICINST: //WIP
        // GENERICINST (CLASS | VALUETYPE) TypeDefOrRefEncoded GenArgCount Type *

        if (!ParseByte(&elem_type))
            return NULL;

        if (elem_type != ELEMENT_TYPE_CLASS && elem_type != ELEMENT_TYPE_VALUETYPE)
            return NULL;

        if (!ParseTypeDefOrRefEncoded(&indexType, &index))
            return NULL;

        if (!ParseNumber(&number))
            return NULL;

        //WIP:
        //NotifyTypeGenericInst(elem_type, indexType, index, number);
        //for (sig_mem_number i = 0; i < number; i++)
        //{
        //    if (!ParseType())
        //        return false;
        //}

        return new TypeInfo(ELEMENT_TYPE_GENERICINST);

    case  ELEMENT_TYPE_VAR: //WIP
        // VAR Number
        if (!ParseNumber(&number))
            return NULL;
        
        //WIP:
        //NotifyTypeGenericTypeVariable(number);

        return new TypeInfo(ELEMENT_TYPE_VAR);

    case  ELEMENT_TYPE_MVAR: //WIP
        // MVAR Number
        if (!ParseNumber(&number))
            return false;

        //WIP:
        //NotifyTypeGenericMemberVariable(number);

        return new TypeInfo(ELEMENT_TYPE_MVAR);
    }

    return NULL;
}

bool SigParser::ParseTypeDefOrRefEncoded(sig_index_type* pIndexTypeOut, sig_index* pIndexOut)
{
    // parse an encoded typedef or typeref

    sig_count encoded = 0;

    if (!ParseNumber(&encoded))
        return false;

    *pIndexTypeOut = (sig_index_type)(encoded & 0x3);
    *pIndexOut = (encoded >> 2);
    return true;
}

bool SigParser::ParseNumber(sig_count* pOut)
{
    // parse the variable length number format (0-4 bytes)

    sig_byte b1 = 0, b2 = 0, b3 = 0, b4 = 0;

    // at least one byte in the encoding, read that

    if (!ParseByte(&b1))
        return false;

    if (b1 == 0xff)
    {
        // special encoding of 'NULL'
        // not sure what this means as a number, don't expect to see it except for string lengths
        // which we don't encounter anyway so calling it an error
        return false;
    }

    // early out on 1 byte encoding
    if ((b1 & 0x80) == 0)
    {
        *pOut = (int)b1;
        return true;
    }

    // now at least 2 bytes in the encoding, read 2nd byte
    if (!ParseByte(&b2))
        return false;

    // early out on 2 byte encoding
    if ((b1 & 0x40) == 0)
    {
        *pOut = (((b1 & 0x3f) << 8) | b2);
        return true;
    }

    // must be a 4 byte encoding

    if ((b1 & 0x20) != 0)
    {
        // 4 byte encoding has this bit clear -- error if not
        return false;
    }

    if (!ParseByte(&b3))
        return false;

    if (!ParseByte(&b4))
        return false;

    *pOut = ((b1 & 0x1f) << 24) | (b2 << 16) | (b3 << 8) | b4;
    return true;
}


void SigParser::Reset()
{
    pbBase = NULL;
    pbCur = NULL;
    pbEnd = NULL;
}