@echo off

setlocal
set dbg_out=%~dp0strat\bin\msvc-8.0\debug
set opt_out=%~dp0strat\bin\msvc-8.0\release
set path_loop=%PATH%
set path_rem=

:loop
if "%path_loop%"=="" goto done
for /f "tokens=1,* delims=;" %%i in ("%path_loop%") do (
    set path_loop=%%j
    if %dbg_out%==%%i goto loop
    if %opt_out%==%%i goto loop
    if "%path_rem%"=="" (
       set path_rem=%%i
    ) else (
       set path_rem=%path_rem%;%%i
    )
)
goto loop
endlocal
:done
set PATH=%path_rem%
echo PATH=%PATH%

