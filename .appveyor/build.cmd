REM Windows build script

subst Z: %APPVEYOR_BUILD_FOLDER%
set SMING_HOME=Z:\Sming

cd /d %SMING_HOME%
gcc -v

set MAKE_PARALLEL=make -j2

REM Move samples and tests into directory outside of the Sming repo.
set SMING_PROJECTS_DIR=%APPVEYOR_BUILD_FOLDER%\..
move ..\samples %SMING_PROJECTS_DIR%
move ..\tests %SMING_PROJECTS_DIR%


set SMING_ARCH=Host
call build1

REM Build a couple of basic applications
%MAKE_PARALLEL% Basic_Serial Basic_ProgMem STRICT=1 V=1 || goto :error

REM Run basic tests
%MAKE_PARALLEL% tests || goto :error


set SMING_ARCH=Esp8266
set ESP_HOME=%UDK_ROOT%
call build1
%MAKE_PARALLEL% Basic_Ssl || goto :error
%MAKE_PARALLEL% Basic_SmartConfig || goto :error


ESP_HOME=%EQT_ROOT%
call build1
%MAKE_PARALLEL% Basic_Ssl || goto :error
%MAKE_PARALLEL% Basic_SmartConfig || goto :error

goto :EOF


:build1

REM This will build the Basic_Blink application and most of the framework Components
cd %SMING_PROJECTS_DIR%/samples/Basic_Blink
make help
make list-config
%MAKE_PARALLEL% || goto :error
cd %SMING_HOME%

goto :EOF


:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
