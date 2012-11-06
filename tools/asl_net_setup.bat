@ECHO OFF

set ADOBE_ROOT=adobe_root
set APL_DIR_NAME=%ADOBE_ROOT%\adobe_platform_libraries
set APL_NAME=apl_1.0.39
set APL_TAR_NAME=%APL_NAME%.tar
set APL_DISTRO_NAME=%APL_NAME%.tgz
set ASL_DIR_NAME=%ADOBE_ROOT%\adobe_source_libraries
set ASL_NAME=asl_1.0.39
set ASL_TAR_NAME=%ASL_NAME%.tar
set ASL_DISTRO_NAME=%ASL_NAME%.tgz
set ASL_DISTRO_HOST=easynews.dl.sourceforge.net
set BOOST_DIR_NAME=boost_1_37_0
set BOOST_TAR_NAME=boost_1_37_0.tar
set BOOST_DISTRO_NAME=%BOOST_DIR_NAME%.tar.gz
set CURL_DISTRO_NAME=curl-7.19.0-win32-nossl.zip
set CURL_EXE_NAME=curl-7.19.0\curl.exe
set GZIP_DISTRO_NAME=gzip-1.3.12-1-bin.zip
set GZIP_EXE_NAME=gnuwin32\bin\gzip.exe
set INTEL_TBB_DIR_NAME=tbb21_009oss
set INTEL_TBB_TAR_NAME=%INTEL_TBB_DIR_NAME%_src.tar
set INTEL_TBB_DISTRO_NAME=%INTEL_TBB_DIR_NAME%_src.tgz
set LECONV=%ADOBE_ROOT%\adobe_platform_libraries\tools\leconv.exe
set PATCH_DISTRO_NAME=patch-2.5.9-7-bin.zip
set PATCH_EXE_NAME=gnuwin32\bin\patch.exe
set TAR_DISTRO_NAME=tar-1.13-1-bin.zip
set TAR_EXE_NAME=gnuwin32\bin\tar.exe
set TAR_DEP_DISTRO_NAME=tar-1.13-1-dep.zip
set TAR_DEP_DLL_NAME=gnuwin32\bin\libiconv-2.dll
set UNZIP_DISTRO_NAME=unz552xN.exe
set UNZIP_EXE_NAME=unzip\unzip.exe



if NOT EXIST %UNZIP_EXE_NAME% (
    echo Getting unzip executable...

    echo cd pub/unix/archiving/info-zip/win32 > cmds.txt
    echo bin >> cmds.txt
    echo get %UNZIP_DISTRO_NAME% >> cmds.txt
    echo bye >> cmds.txt

    %WINDIR%\system32\ftp -v -s:cmds.txt -A sunsite.icm.edu.pl

    del cmds.txt
)



if NOT EXIST %CURL_DISTRO_NAME% (
    echo Getting curl distribution...

    echo cd utils/archivers/curl > cmds.txt
    echo bin >> cmds.txt
    echo get %CURL_DISTRO_NAME% >> cmds.txt
    echo bye >> cmds.txt

    %WINDIR%\system32\ftp -v -s:cmds.txt -A gd.tuwien.ac.at

    del cmds.txt
)



if NOT EXIST %UNZIP_EXE_NAME% (
    echo Setting up unzip...

    mkdir unzip

    move unz552xN.exe unzip

    PUSHD unzip

    unz552xN.exe

    POPD
)

if NOT EXIST %CURL_EXE_NAME% (
    echo Extracting curl...
    %UNZIP_EXE_NAME% %CURL_DISTRO_NAME%
)

set CURL_EXE_NAME=%CURL_EXE_NAME% -L


if NOT EXIST %TAR_DISTRO_NAME% (
    echo Getting tar distribution...
    %CURL_EXE_NAME% http://internap.dl.sourceforge.net/sourceforge/gnuwin32/%TAR_DISTRO_NAME% -o %TAR_DISTRO_NAME%
)


if NOT EXIST %TAR_DEP_DISTRO_NAME% (
    echo Getting tar dependencies distribution...
    %CURL_EXE_NAME% http://internap.dl.sourceforge.net/sourceforge/gnuwin32/%TAR_DEP_DISTRO_NAME% -o %TAR_DEP_DISTRO_NAME%
)

if NOT EXIST %GZIP_DISTRO_NAME% (
    echo Getting gzip distribution...
    %CURL_EXE_NAME% http://internap.dl.sourceforge.net/sourceforge/gnuwin32/%GZIP_DISTRO_NAME% -o %GZIP_DISTRO_NAME%
)

if NOT EXIST %PATCH_DISTRO_NAME% (
    echo Getting patch distribution...
    %CURL_EXE_NAME% http://internap.dl.sourceforge.net/sourceforge/gnuwin32/%PATCH_DISTRO_NAME% -o %PATCH_DISTRO_NAME%
)

if NOT EXIST %TAR_EXE_NAME% (
    echo Extracting tar...
    %UNZIP_EXE_NAME% -o -d gnuwin32 %TAR_DISTRO_NAME%
)

if NOT EXIST %TAR_DEP_DLL_NAME% (
    echo Extracting tar dependencies...
    %UNZIP_EXE_NAME% -o -d gnuwin32 %TAR_DEP_DISTRO_NAME%
)

if NOT EXIST %GZIP_EXE_NAME% (
    echo Extracting gzip...
    %UNZIP_EXE_NAME% -o -d gnuwin32 %GZIP_DISTRO_NAME%
)

if NOT EXIST %PATCH_EXE_NAME% (
    echo Extracting patch...
    %UNZIP_EXE_NAME% -o -d gnuwin32 %PATCH_DISTRO_NAME%
)

if NOT "_%1%_" == "__" (
    set ASL_DISTRO_HOST=%1%
)


if NOT EXIST %ASL_DISTRO_NAME% (
    echo Getting ASL distribution...
    %CURL_EXE_NAME% http://%ASL_DISTRO_HOST%/sourceforge/adobe-source/%ASL_DISTRO_NAME% -o %ASL_DISTRO_NAME%
)

if NOT EXIST %APL_DISTRO_NAME% (
    echo Getting APL distribution...
    %CURL_EXE_NAME% http://%ASL_DISTRO_HOST%/sourceforge/adobe-source/%APL_DISTRO_NAME% -o %APL_DISTRO_NAME%
)

if NOT EXIST %BOOST_DISTRO_NAME% (
if NOT EXIST %BOOST_TAR_NAME% (
    echo Getting Boost distribution...
    %CURL_EXE_NAME% http://easynews.dl.sourceforge.net/sourceforge/boost/%BOOST_DISTRO_NAME% -o %BOOST_DISTRO_NAME%
)
)

if NOT EXIST %INTEL_TBB_DISTRO_NAME% (
if NOT EXIST %INTEL_TBB_TAR_NAME% (
    echo Getting Intel Thread Building Blocks distribution...
    %CURL_EXE_NAME% http://www.threadingbuildingblocks.org/uploads/78/122/2.1/%INTEL_TBB_DISTRO_NAME% -o %INTEL_TBB_DISTRO_NAME%
)
)


if NOT EXIST %ADOBE_ROOT% (
  echo Creating %ADOBE_ROOT% dir
  md %ADOBE_ROOT%
)



if NOT EXIST %ASL_DIR_NAME% (
    echo Extracting ASL distribution...
    if NOT EXIST %ASL_TAR_NAME% (
        %GZIP_EXE_NAME% -d %ASL_DISTRO_NAME%
    )
    %TAR_EXE_NAME% -xf %ASL_TAR_NAME% 
    move source_release "%ASL_DIR_NAME%"
)

if NOT EXIST %APL_DIR_NAME% (
   echo Extracting APL distribution...
   if NOT EXIST %APL_TAR_NAME% (
       %GZIP_EXE_NAME% -d %APL_DISTRO_NAME%
   )
   %TAR_EXE_NAME% -xf %APL_TAR_NAME%
   move platform_release "%APL_DIR_NAME%"
)
)

if NOT EXIST %ADOBE_ROOT%\boost_libraries\INSTALL (
    echo Expanding Boost directory, which could take 10 minutes...

    if EXIST %ADOBE_ROOT%\boost_libraries (
       rmdir /S /Q %ADOBE_ROOT%\boost_libraries
    )

    if NOT EXIST %BOOST_TAR_NAME% (
        %GZIP_EXE_NAME% -d %BOOST_DISTRO_NAME%
    )
    %TAR_EXE_NAME% -xf %BOOST_TAR_NAME%
    rename %BOOST_DIR_NAME% boost_libraries
    move boost_libraries %ADOBE_ROOT%\boost_libraries
)

if NOT EXIST %ADOBE_ROOT%\intel_tbb_libraries\README (
    echo Expanding Intel TBB directory...

    if EXIST %ADOBE_ROOT%\intel_tbb_libraries (
       rmdir /S /Q %ADOBE_ROOT%\intel_tbb_libraries
    )

    if NOT EXIST %INTEL_TBB_TAR_NAME% (
        %GZIP_EXE_NAME% -d %INTEL_TBB_DISTRO_NAME%
    )
    %TAR_EXE_NAME% -xf %INTEL_TBB_TAR_NAME%
    rename %INTEL_TBB_DIR_NAME% intel_tbb_libraries
    move intel_tbb_libraries %ADOBE_ROOT%\intel_tbb_libraries
)


echo Converting Line Endings for patch file since gnuwin32 patch needs CRLF...

%LECONV% %ADOBE_ROOT%\adobe_source_libraries\tools\%BOOST_DIR_NAME%_patches.txt

echo Patching Boost...

PUSHD %ADOBE_ROOT%\boost_libraries

..\..\%PATCH_EXE_NAME% -uN -p0 -g0 < ..\adobe_source_libraries\tools\%BOOST_DIR_NAME%_patches.txt

POPD

echo Building ASL...

PUSHD %ADOBE_ROOT%\adobe_platform_libraries

..\adobe_source_libraries\tools\build.bat

POPD

PAUSE
