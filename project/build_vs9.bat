echo off

echo Common tools directory: %VS90COMNTOOLS%

call "%VS90COMNTOOLS%/vsvars32.bat"

echo [cleaning build tree]
rd /S /Q build
mkdir build

cd build
IF %ERRORLEVEL% NEQ 0 (
	echo "Could not change to build directory"
	goto :error
)

echo [calling CMake]

cmake -G "NMake Makefiles" %* ..
IF %ERRORLEVEL% NEQ 0 (
	echo [CMake error]
	goto :error
)

echo [building project]

nmake
IF %ERRORLEVEL% NEQ 0 (
	echo [build error]
	goto :error
)

echo [running tests]
ctest -V

echo [build successful and tests executed]

REM kill remaining spread instances to not block subsequent builds
taskkill /F /IM spread.exe

echo [installing project]

nmake install
IF %ERRORLEVEL% NEQ 0 (
	echo [install error]
	goto :error
)

cd ..

exit /b 0

:error
cd ..
exit /b 1
