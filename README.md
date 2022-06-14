# DnTracer #

DnTracer can print info about functions called by a specific .net executable, which can help reverse enginners to analyse .net malware.

During the execution, DnTracer will try to resolve the function's paramter values, currently only simple types are supported(byte, int, long, string....)

There are still a lot of works need to be done here, issues and pull requests are welcomed. :D


## How to use ##

DnTracer makes use of .net profiling apis, so you have to set up environment variables to run it. I recommand you to create a bat file for convenience.

```bat
SET COR_ENABLE_PROFILING=1
SET COR_PROFILER={b45048d5-6f44-4fbe-ae88-b468a5e4927a}
SET COR_PROFILER_PATH=ProfilingDll_x86.dll 
@rem use ProfilingDll_x64.dll if the target framework is x64
SET COMPLUS_ProfAPI_ProfilerCompatibilitySetting=EnableV2Profiler
START path_to_executable.exe
```

A log file will be created in the working directory. Check the information there.

If the target executable has a console window, log info will be printed too.


## Notice ##

The target framework of a .net executable can be x64 while the PE processor architecture is 32bit. So check it out and choose the right dll.

If you encounterd any problem, you can check out the Windows Event Viewer. Also, feel free to submit an issue (no solutions guaranteed :D)

## Reference ##

[Create a .NET profiler with the Profiling API - Start of an unexpected journey](https://dev.to/gabbersepp/create-a-net-profiler-with-the-profiling-api-start-of-an-unexpected-journey-198n)


[Profiling (Unmanaged API Reference)](https://docs.microsoft.com/en-us/dotnet/framework/unmanaged-api/profiling/)

[Decyphering methods signature with .NET profiling APIs](https://chnasarre.medium.com/decyphering-method-signature-with-clr-profiling-api-8328a72a216e)


## Screenshots ##

![screenshot](https://i.imgur.com/NUMbGjS.png)