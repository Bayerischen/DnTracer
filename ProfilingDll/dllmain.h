// dllmain.h: 模块类的声明。

class CProfilingDllModule : public ATL::CAtlDllModuleT< CProfilingDllModule >
{
public :
	DECLARE_LIBID(LIBID_ProfilingDllLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROFILINGDLL, "{82db5741-32f0-4e2c-b2d4-16c821d03422}")
};

extern class CProfilingDllModule _AtlModule;
