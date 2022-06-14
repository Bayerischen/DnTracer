#include "pch.h"
#include <io.h>
#include <fcntl.h>
#include "Utils.h"
#include "SigParser.h"

using namespace std;

int Utils::GetClassNameByObjectId(ICorProfilerInfo2* info, ObjectID objectId, wchar_t** output)
{
    ClassID classId;
    if (info->GetClassFromObject(objectId, &classId) != S_OK)
        return -1;

    return Utils::GetClassNameByClassId(info, classId, output);
}


int Utils::GetClassNameByClassId(ICorProfilerInfo2* info, ClassID classId, wchar_t** output)
{
    ModuleID moduleId;
    mdTypeDef typeDefToken;
    IMetaDataImport* metadata;

    HRESULT hresult = info->GetClassIDInfo(classId, &moduleId, &typeDefToken);
    if (hresult != S_OK || moduleId == 0)
        return -1;

    if (info->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, (IUnknown**)&metadata) != S_OK)
        return -1;

    wchar_t* buffer;
    ULONG length = 0;
    metadata->GetTypeDefProps(typeDefToken, NULL, 0, &length, NULL, NULL); //get actual length of name
    buffer = new wchar_t[length + 1];
    hresult = metadata->GetTypeDefProps(typeDefToken, buffer, length, &length, NULL, NULL);
    metadata->Release();

    if (hresult != S_OK)
        return -1;
    
    buffer[length] = 0;
    *output = buffer;
    return length; //no way the length is longer than 0xFFFFFFFF right?
}


int Utils::GetFunctionNameById(ICorProfilerInfo2* info, FunctionID functionId, wchar_t** output)
{
    IMetaDataImport* metadata;
    mdMethodDef methodToken;
    mdTypeDef typeDefToken;

    HRESULT hresult = info->GetTokenAndMetaDataFromFunction(functionId, IID_IMetaDataImport, (IUnknown**)&metadata, &methodToken);
    if (hresult != S_OK)
        return -1;

    wchar_t* buffer;
    ULONG length = 0;
    metadata->GetMethodProps(methodToken, &typeDefToken, NULL, 0, &length, NULL, NULL, NULL, NULL, NULL); //get actual length of name
    buffer = new wchar_t[length + 1];
    metadata->GetMethodProps(methodToken, &typeDefToken, buffer, length, &length, NULL, NULL, NULL, NULL, NULL);
    metadata->Release();

    if (hresult != S_OK)
        return -1;
    
    buffer[length] = 0;
    *output = buffer;
    return length;
}


ULONG Utils::GetFunctionSig(ICorProfilerInfo2* info, FunctionID functionId, PCCOR_SIGNATURE* signature)
{
    IMetaDataImport2* metadata;
    mdMethodDef methodToken;

    HRESULT hresult = info->GetTokenAndMetaDataFromFunction(functionId, IID_IMetaDataImport2, (IUnknown**)&metadata, &methodToken);
    if (hresult != S_OK)
        return -1;

    ULONG sigSize;

    metadata->GetMethodProps(methodToken, NULL, NULL, 0, NULL, NULL, signature, &sigSize, NULL, NULL);
    metadata->Release();

    if (hresult != S_OK)
        return -1;

    return sigSize;
}


int Utils::GetModuleNameById(ICorProfilerInfo2* info, ModuleID moduleID, wchar_t** output)
{
    wchar_t* buffer;
    ULONG length;
    info->GetModuleInfo(moduleID, NULL, 0, &length, NULL, NULL); //get actual length of name
    buffer = new wchar_t[length + 1];
    if (info->GetModuleInfo(moduleID, NULL, length, &length, buffer, NULL) != S_OK)
        return -1;

    buffer[length] = 0;
    *output = buffer;
    return length;
}


//return value is the length of output
void Utils::GetParamterValueStr(COR_PRF_FUNCTION_ARGUMENT_RANGE* range, sig_elem_type type, char* outBuf, int bufSize )
{
    char charValue;
    short shortValue;
    int intValue;
    INT64 longValue;
    float floatValue;
    double doubleValue;

    int cpyCount;
    long strLen = 0;
    byte** ptr = NULL;
    byte* strPtr = NULL;

    switch (type)
    {
    case ELEMENT_TYPE_BOOLEAN: //bool
        intValue = *(int*)range->startAddress;
        
        if (intValue == 0)
        {
            cpyCount = bufSize > 5 ? 5 : bufSize - 1;
            strncpy(outBuf, "false", cpyCount);
            outBuf[cpyCount] = 0;
            return;
        }
        else
        {
            cpyCount = bufSize > 4 ? 4 : bufSize - 4;
            strncpy(outBuf, "true", cpyCount);
            outBuf[cpyCount] = 0;
            return;
        }

    case ELEMENT_TYPE_CHAR: //char
        charValue = *(char*)range->startAddress;
        outBuf[0] = charValue;
        outBuf[1] = 0;
        return;

    case ELEMENT_TYPE_I1: //sbyte
        charValue = *(char*)range->startAddress;
        snprintf(outBuf, bufSize, "%d", charValue);
        return;

    case ELEMENT_TYPE_U1: //byte
        charValue = *(char*)range->startAddress;
        snprintf(outBuf, bufSize, "%u", charValue);
        return;

    case ELEMENT_TYPE_I2: //short
        shortValue = *(short*)range->startAddress;
        snprintf(outBuf, bufSize, "%d", shortValue);
        return;

    case ELEMENT_TYPE_U2: //ushort
        shortValue = *(short*)range->startAddress;
        snprintf(outBuf, bufSize, "%u", shortValue);
        return;

    case ELEMENT_TYPE_I4: //int
        intValue = *(int*)range->startAddress;
        snprintf(outBuf, bufSize, "%d", intValue);
        return;

    case ELEMENT_TYPE_U4: //uint
        intValue = *(int*)range->startAddress;
        snprintf(outBuf, bufSize, "%u", intValue);
        return;

    case ELEMENT_TYPE_I8: //long
        longValue = *(INT64*)range->startAddress;;
        snprintf(outBuf, bufSize, "%lld", longValue);
        return;

    case ELEMENT_TYPE_U8: //ulong
        longValue = *(INT64*)range->startAddress;
        snprintf(outBuf, bufSize, "%llu", longValue);
        return;

    case ELEMENT_TYPE_R4: //float
        floatValue = *(float*)range->startAddress;
        snprintf(outBuf, bufSize, "%f", floatValue);
        return;

    case ELEMENT_TYPE_R8: //double
        doubleValue = *(double*)range->startAddress;
        snprintf(outBuf, bufSize, "%f", doubleValue);
        return;

    case ELEMENT_TYPE_STRING: //string
        ptr = (byte**)range->startAddress;
        strPtr = *ptr;
        strPtr = strPtr + sizeof(int*);
        strLen = *(long*)strPtr;
        strPtr += 4;
        snprintf(outBuf, bufSize, "%ls", (wchar_t*)strPtr);
        return;

    default:
        cpyCount = bufSize > 11 ? 11 : bufSize - 1;
        strncpy(outBuf, "not_support", cpyCount);
        outBuf[cpyCount] = 0;
        return;
    }
}


const char* Utils::GetMethodTypeName(sig_elem_type type)
{
    switch (type)
    {
    case SIG_METHOD_DEFAULT:
        return "METHOD_DEFAULT";
    case SIG_METHOD_C:
        return "METHOD_C";
    case SIG_METHOD_STDCALL:
        return "METHOD_STDCALL";
    case SIG_METHOD_THISCALL:
        return "METHOD_THISCALL";
    case SIG_METHOD_FASTCALL:
        return "METHOD_FASTCALL";
    case SIG_METHOD_VARARG:
        return "METHOD_VARARG";

    default:
        return "UNKNOWN";
    }
}


void Utils::RedirectIOToConsole()
{
    int hConHandle;
    intptr_t lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE* fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 500;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (intptr_t)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console
    lStdHandle = (intptr_t)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    lStdHandle = (intptr_t)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    ios::sync_with_stdio();
}
