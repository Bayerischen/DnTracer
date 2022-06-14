#pragma once

/*
Sig ::= MethodDefSig | MethodRefSig | StandAloneMethodSig | FieldSig | PropertySig | LocalVarSig

MethodDefSig ::= [[HASTHIS] [EXPLICITTHIS]] (DEFAULT|VARARG|GENERIC GenParamCount) ParamCount RetType Param*

MethodRefSig ::= [[HASTHIS] [EXPLICITTHIS]] VARARG ParamCount RetType Param* [SENTINEL Param+]

StandAloneMethodSig ::=  [[HASTHIS] [EXPLICITTHIS]] (DEFAULT|VARARG|C|STDCALL|THISCALL|FASTCALL)
                    ParamCount RetType Param* [SENTINEL Param+]

FieldSig ::= FIELD CustomMod* Type

PropertySig ::= PROPERTY [HASTHIS] ParamCount CustomMod* Type Param*

LocalVarSig ::= LOCAL_SIG Count (TYPEDBYREF | ([CustomMod] [Constraint])* [BYREF] Type)+


-------------

CustomMod ::= ( CMOD_OPT | CMOD_REQD ) ( TypeDefEncoded | TypeRefEncoded )

Constraint ::= #define ELEMENT_TYPE_PINNED

Param ::= CustomMod* ( TYPEDBYREF | [BYREF] Type )

RetType ::= CustomMod* ( VOID | TYPEDBYREF | [BYREF] Type )

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
                | GENERICINST (CLASS | VALUETYPE) TypeDefOrRefEncoded GenArgCount Type*
                | VAR Number
                | MVAR Number

ArrayShape ::= Rank NumSizes Size* NumLoBounds LoBound*

TypeDefOrRefEncoded ::= TypeDefEncoded | TypeRefEncoded
TypeDefEncoded ::= 32-bit-3-part-encoding-for-typedefs-and-typerefs
TypeRefEncoded ::= 32-bit-3-part-encoding-for-typedefs-and-typerefs

ParamCount ::= 29-bit-encoded-integer
GenArgCount ::= 29-bit-encoded-integer
Count ::= 29-bit-encoded-integer
Rank ::= 29-bit-encoded-integer
NumSizes ::= 29-bit-encoded-integer
Size ::= 29-bit-encoded-integer
NumLoBounds ::= 29-bit-encoded-integer
LoBounds ::= 29-bit-encoded-integer
Number ::= 29-bit-encoded-integer

*/

#define ELEMENT_TYPE_END 0x00 //Marks end of a list
#define ELEMENT_TYPE_VOID 0x01
#define ELEMENT_TYPE_BOOLEAN 0x02
#define ELEMENT_TYPE_CHAR 0x03
#define ELEMENT_TYPE_I1 0x04
#define ELEMENT_TYPE_U1 0x05
#define ELEMENT_TYPE_I2 0x06
#define ELEMENT_TYPE_U2 0x07
#define ELEMENT_TYPE_I4 0x08
#define ELEMENT_TYPE_U4 0x09
#define ELEMENT_TYPE_I8 0x0a
#define ELEMENT_TYPE_U8 0x0b
#define ELEMENT_TYPE_R4 0x0c
#define ELEMENT_TYPE_R8 0x0d
#define ELEMENT_TYPE_STRING 0x0e
#define ELEMENT_TYPE_PTR 0x0f // Followed by type
#define ELEMENT_TYPE_BYREF 0x10 // Followed by type
#define ELEMENT_TYPE_VALUETYPE 0x11 // Followed by TypeDef or TypeRef token
#define ELEMENT_TYPE_CLASS 0x12 // Followed by TypeDef or TypeRef token
#define ELEMENT_TYPE_VAR 0x13 // Generic parameter in a generic type definition, represented as number
#define ELEMENT_TYPE_ARRAY 0x14 // type rank boundsCount bound1 ¡­ loCount lo1 ¡­
#define ELEMENT_TYPE_GENERICINST 0x15 // Generic type instantiation. Followed by type type-arg-count type-1 ... type-n
#define ELEMENT_TYPE_TYPEDBYREF 0x16
#define ELEMENT_TYPE_I 0x18 // System.IntPtr
#define ELEMENT_TYPE_U 0x19 // System.UIntPtr
#define ELEMENT_TYPE_FNPTR 0x1b // Followed by full method signature
#define ELEMENT_TYPE_OBJECT 0x1c // System.Object
#define ELEMENT_TYPE_SZARRAY 0x1d // Single-dim array with 0 lower bound

#define ELEMENT_TYPE_MVAR 0x1e // Generic parameter in a generic method definition,represented as number
#define ELEMENT_TYPE_CMOD_REQD 0x1f // Required modifier : followed by a TypeDef or TypeRef token
#define ELEMENT_TYPE_CMOD_OPT 0x20 // Optional modifier : followed by a TypeDef or TypeRef token
#define ELEMENT_TYPE_INTERNAL 0x21 // Implemented within the CLI
#define ELEMENT_TYPE_MODIFIER 0x40 // Or¡¯d with following element types
#define ELEMENT_TYPE_SENTINEL 0x41 // Sentinel for vararg method signature
#define ELEMENT_TYPE_PINNED 0x45 // Denotes a local variable that points at a pinned object

#define SIG_METHOD_DEFAULT  0x0 // default calling convention
#define SIG_METHOD_C  0x1       // C calling convention
#define SIG_METHOD_STDCALL  0x2 // Stdcall calling convention
#define SIG_METHOD_THISCALL 0x3 // thiscall  calling convention
#define SIG_METHOD_FASTCALL 0x4 // fastcall calling convention
#define SIG_METHOD_VARARG  0x5  // vararg calling convention
#define SIG_FIELD 0x6           // encodes a field
#define SIG_LOCAL_SIG 0x7       // used for the .locals directive
#define SIG_PROPERTY 0x8        // used to encode a property


#define SIG_GENERIC 0x10 // used to indicate that the method has one or more generic parameters.
#define SIG_HASTHIS 0x20  // used to encode the keyword instance in the calling convention
#define SIG_EXPLICITTHIS  0x40 // used to encode the keyword explicit in the calling convention

#define SIG_INDEX_TYPE_TYPEDEF 0    // ParseTypeDefOrRefEncoded returns this as the out index type for typedefs
#define SIG_INDEX_TYPE_TYPEREF 1    // ParseTypeDefOrRefEncoded returns this as the out index type for typerefs
#define SIG_INDEX_TYPE_TYPESPEC 2  // ParseTypeDefOrRefEncoded returns this as the out index type for typespecs


typedef unsigned char sig_byte;
typedef unsigned char sig_elem_type;
typedef unsigned char sig_index_type;
typedef unsigned int sig_index;
typedef unsigned int sig_count;
typedef unsigned int sig_mem_number;




#define ELEMENT_TYPE_UNKNOW 0xFF