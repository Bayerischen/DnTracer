#include "pch.h"
#include "TypeInfo.h"

TypeInfo::TypeInfo(sig_elem_type type) : type(type)
{
}


bool TypeInfo::IsSimpleType()
{
    switch (type)
    {
    case ELEMENT_TYPE_VOID:
    case ELEMENT_TYPE_BOOLEAN:
    case ELEMENT_TYPE_CHAR:
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R4:
    case ELEMENT_TYPE_R8:
    case ELEMENT_TYPE_STRING:
    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
    case ELEMENT_TYPE_OBJECT:
        return true;

    default:
        return false;
    }
}


std::string TypeInfo::GetTypeName()
{
    switch (type)
    {
    case ELEMENT_TYPE_END:
        return std::string("ELEMENT_TYPE_END");

    //========== simple types ==========//
    case ELEMENT_TYPE_VOID:
        return std::string("void");
    case ELEMENT_TYPE_BOOLEAN:
        return std::string("bool");
    case ELEMENT_TYPE_CHAR:
        return std::string("char");
    case ELEMENT_TYPE_I1:
        return std::string("sbyte");
    case ELEMENT_TYPE_U1:
        return std::string("byte");
    case ELEMENT_TYPE_I2:
        return std::string("short");
    case ELEMENT_TYPE_U2:
        return std::string("ushort");
    case ELEMENT_TYPE_I4:
        return std::string("int");
    case ELEMENT_TYPE_U4:
        return std::string("uint");
    case ELEMENT_TYPE_I8:
        return std::string("long");
    case ELEMENT_TYPE_U8:
        return std::string("ulong");
    case ELEMENT_TYPE_R4:
        return std::string("float");
    case ELEMENT_TYPE_R8:
        return std::string("double");
    case ELEMENT_TYPE_STRING:
        return std::string("string");
    case ELEMENT_TYPE_I:
        return std::string("IntPtr");
    case ELEMENT_TYPE_U:
        return std::string("UIntPtr");
    case ELEMENT_TYPE_OBJECT:
        return std::string("object");

    //========== nasty types ==========//
    case ELEMENT_TYPE_PTR:
        if (refType != NULL && refType->IsSimpleType())
            return refType->GetTypeName().append(1, '*');
        else
            return std::string("TYPE_PTR");
    case ELEMENT_TYPE_BYREF:
        return std::string("TYPE_BYREF");
    case ELEMENT_TYPE_VALUETYPE:
        return std::string("TYPE_VALUETYPE");
    case ELEMENT_TYPE_CLASS:
        return std::string("TYPE_CLASS");
    case ELEMENT_TYPE_VAR:
        return std::string("TYPE_VAR");
    case ELEMENT_TYPE_GENERICINST:
        return std::string("TYPE_GENERICINST");
    case ELEMENT_TYPE_TYPEDBYREF:
        return std::string("TYPE_TYPEDBYREF");
    case ELEMENT_TYPE_FNPTR:
        return std::string("TYPE_FNPTR");
    case ELEMENT_TYPE_MVAR:
        return std::string("TYPE_MVAR");
    case ELEMENT_TYPE_CMOD_REQD:
        return std::string("TYPE_CMOD_REQD");
    case ELEMENT_TYPE_CMOD_OPT:
        return std::string("TYPE_CMOD_OPT");
    case ELEMENT_TYPE_INTERNAL:
        return std::string("TYPE_INTERNAL");
    case ELEMENT_TYPE_MODIFIER:
        return std::string("TYPE_MODIFIER");
    case ELEMENT_TYPE_SENTINEL:
        return std::string("TYPE_SENTINEL");
    case ELEMENT_TYPE_PINNED:
        return std::string("TYPE_PINNED");

    //========== array ==========//
    case ELEMENT_TYPE_ARRAY:
        if (refType != NULL && refType->IsSimpleType())
            return refType->GetTypeName().append("[]");
        else
            return std::string("TYPE_ARRAY");
    case ELEMENT_TYPE_SZARRAY:
        if (refType != NULL && refType->IsSimpleType())
            return refType->GetTypeName().append("[]");
        else
            return std::string("TYPE_SZARRAY");

    default:
        return std::string("UNKNOWN");
    }
}