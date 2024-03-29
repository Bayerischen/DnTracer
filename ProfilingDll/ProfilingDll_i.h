

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0626 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for ProfilingDll.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0626 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ProfilingDll_i_h__
#define __ProfilingDll_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if _CONTROL_FLOW_GUARD_XFG
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __INetProfiler_FWD_DEFINED__
#define __INetProfiler_FWD_DEFINED__
typedef interface INetProfiler INetProfiler;

#endif 	/* __INetProfiler_FWD_DEFINED__ */


#ifndef __NetProfiler_FWD_DEFINED__
#define __NetProfiler_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetProfiler NetProfiler;
#else
typedef struct NetProfiler NetProfiler;
#endif /* __cplusplus */

#endif 	/* __NetProfiler_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __INetProfiler_INTERFACE_DEFINED__
#define __INetProfiler_INTERFACE_DEFINED__

/* interface INetProfiler */
/* [object][version][uuid] */ 


EXTERN_C const IID IID_INetProfiler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("82db5741-32f0-4e2c-b2d4-16c821d03422")
    INetProfiler : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct INetProfilerVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetProfiler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetProfiler * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetProfiler * This);
        
        END_INTERFACE
    } INetProfilerVtbl;

    interface INetProfiler
    {
        CONST_VTBL struct INetProfilerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetProfiler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetProfiler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetProfiler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetProfiler_INTERFACE_DEFINED__ */



#ifndef __ProfilingDllLib_LIBRARY_DEFINED__
#define __ProfilingDllLib_LIBRARY_DEFINED__

/* library ProfilingDllLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ProfilingDllLib;

EXTERN_C const CLSID CLSID_NetProfiler;

#ifdef __cplusplus

class DECLSPEC_UUID("b45048d5-6f44-4fbe-ae88-b468a5e4927a")
NetProfiler;
#endif
#endif /* __ProfilingDllLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


