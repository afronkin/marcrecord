if exist "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
if exist "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
nmake /f Makefile %*
