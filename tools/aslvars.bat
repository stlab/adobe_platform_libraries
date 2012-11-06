@SET APLTOOLDIR=%~dp0
@if "%APLTOOLDIR%"=="" goto error_no_APLTOOLDIR
@SET ASLROOTDIR=%APLTOOLDIR%..\..
@if "%ASLROOTDIR%"=="" goto error_no_ASLROOTDIR
@SET ASLTOOLDIR=%ASLROOTDIR%\adobe_source_libraries\tools
@if "%ASLTOOLDIR%"=="" goto error_no_ASLTOOLDIR
@if "_%HOME%_"=="__" SET HOME=%APLTOOLDIR%
@if "%HOME%"=="" goto error_no_HOME


@echo Setting environment for using ASL tools.

@set PATH=%APLTOOLDIR%;%ASLTOOLDIR%;%PATH%
@chdir %APLTOOLDIR%\..
@goto end

:error_no_APLTOOLDIR
@echo ERROR: APLTOOLDIR variable is not set. 
@goto end

:error_no_ASLROOTDIR
@echo ERROR: ASLROOTDIR variable is not set. 
@goto end

:error_no_ASLTOOLDIR
@echo ERROR: ASLTOOLDIR variable is not set. 
@goto end

:error_no_HOME
@echo ERROR: HOME variable is not set. 
@goto end


:end
