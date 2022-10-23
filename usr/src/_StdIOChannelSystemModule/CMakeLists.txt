cmake_minimum_required(VERSION 3.14)

set(PACKAGE_NAME _StdIOChannelSystemModule)

project(StdIOSystem LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
enable_language(C)


message("")
message("******************************************************************************************")
message("Required Environment Variables")
message("")
message("\tUCF_CODE_REPOS = <code repository root> '4-repos'")
message("\tUCF_RELEASE_ROOT <./ucfroot | c:\\ucf\\ucfroot")
message("\tUCF_RELEASE_NAME <name of UCF release> 'UCF-010122'")
message("\tUCF_BIN_NAME <output dir under ./bin/<release name/> 'lx = AppImageSrc/AppDir/bin'")
message("")
message("******************************************************************************************")
message("")

if (MSVC)
    
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_STATIC_DIR}/$<0:>)    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules/$<0:>)
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/$<0:>)
    #message(x-${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

    # warning level 4 and all warnings as errors
	#add_compile_options(/W4 /WX)
else()

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules)

    # lots of warnings and all warnings as errors
    add_compile_options(
        -fpic
        -shared
        -g3
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-result
        -Wno-sign-compare
        -Wno-parentheses
        -Wno-parentheses
        -Wno-parentheses
        -Wno-write-strings
        -Wno-sign-conversion
        -Wno-unused-but-set-variable
        -Wno-unused-but-set-parameter
        )
endif()

if (MSVC)

	include_directories(
    	/usr/include/netinet
		windows.h
		$ENV{UCF_CODE_REPOS}/9-gsl
		$ENV{UCF_CODE_REPOS}/AppEngine_imx
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx
		$ENV{UCF_CODE_REPOS}/ui
		$ENV{UCF_CODE_REPOS}/imx
		$ENV{UCF_CODE_REPOS}/files
		$ENV{UCF_CODE_REPOS}/files/win32
		$ENV{UCF_CODE_REPOS}/string
		$ENV{UCF_CODE_REPOS}/UniConsole
		$ENV{UCF_CODE_REPOS}/iStreamIO
		$ENV{UCF_CODE_REPOS}/UniConsole_imx
		$ENV{UCF_CODE_REPOS}/process-threads
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystem_imx
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters
	)

	add_library(StdIOSystem SHARED

		$ENV{UCF_CODE_REPOS}/StdIOChannelSystem_imx/iStdIOChannelSystem.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystem.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystemModule.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystemModule.cpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystem.cpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/Lping.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/Lping.cpp
		$ENV{UCF_CODE_REPOS}/imx/IOString.hpp
		$ENV{UCF_CODE_REPOS}/imx/IOString.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr/ConnectionID.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/ConnectionID.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iIOChannelDevice.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/IOChannelDevice.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iTrace.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iStreamIO.hpp
		#
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/WTcpBase.cpp 
		$ENV{UCF_CODE_REPOS}/iStreamIO/WRs232_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/WRs232_iStrIO.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/WTcpClient_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/WTcpClient_iStrIO.cpp
		#
		$ENV{UCF_CODE_REPOS}/iStreamIO/WUdpClient_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/WUdpBase.cpp 
		$ENV{UCF_CODE_REPOS}/iStreamIO/WUdpClient_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/WUdpClient_iStrIO.cpp
		#
		$ENV{UCF_CODE_REPOS}/iStreamIO/File_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/files/PathLW.hpp
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters/SqliteIO.hpp
		$ENV{UCF_CODE_REPOS}/process-threads/SimpleAlarm.hpp
		#
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb/SystemCfg.hpp
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb/SystemCfg.cpp
		$ENV{UCF_CODE_REPOS}/files/PathLW.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/File_iStrIO.cpp
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters/SqliteIO.cpp
		$ENV{UCF_CODE_REPOS}/process-threads/ExtExecL.cpp
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src/sqlite3.c 
	)


	target_link_libraries(
   		StdIOSystem
		wsock32
		ws2_32
    	Mswsock
    	AdvApi32
	)

else()

    set(
        AppFiles
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystem_imx/iStdIOChannelSystem.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystem.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystemModule.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystemModule.cpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/StdIOChannelSystem.cpp
        )

    set (
        SrcFiles
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/Lping.hpp
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule/Lping.cpp
		$ENV{UCF_CODE_REPOS}/imx/IOString.hpp
		$ENV{UCF_CODE_REPOS}/imx/IOString.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr/ConnectionID.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/ConnectionID.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iIOChannelDevice.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/IOChannelDevice.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iTrace.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/iStreamIO.hpp
		#
		$ENV{UCF_CODE_REPOS}/iStreamIO/LTTY_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/LTTY_iStrIO.cpp
		#
		$ENV{UCF_CODE_REPOS}/iStreamIO/File_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/files/PathLW.hpp
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters/SqliteIO.hpp
		$ENV{UCF_CODE_REPOS}/process-threads/SimpleAlarm.hpp
		#
		#$ENV{UCF_CODE_REPOS}/process-threads/SerialConsole.hpp
		#$ENV{UCF_CODE_REPOS}/process-threads/SerialConsole.cpp
		#
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb/SystemCfg.hpp
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb/SystemCfg.cpp
		$ENV{UCF_CODE_REPOS}/files/PathLW.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/File_iStrIO.cpp
        #
		$ENV{UCF_CODE_REPOS}/iStreamIO/LSerialIO_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/LSerialIO_iStrIO.cpp
        #
		$ENV{UCF_CODE_REPOS}/iStreamIO/LTcpClient_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/LTcpClient_iStrIO.cpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/LUdpClient_iStrIO.hpp
		$ENV{UCF_CODE_REPOS}/iStreamIO/1-src/LUdpClient_iStrIO.cpp
        #
		$ENV{UCF_CODE_REPOS}/process-threads/ExtExecL.cpp
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src/sqlite3.c 
	)

	include_directories(
        /usr/include/netinet
		$ENV{UCF_CODE_REPOS}/9-gsl
		$ENV{UCF_CODE_REPOS}/AppEngine_imx
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx
		$ENV{UCF_CODE_REPOS}/ui
		$ENV{UCF_CODE_REPOS}/imx
		$ENV{UCF_CODE_REPOS}/files
		$ENV{UCF_CODE_REPOS}/string
		$ENV{UCF_CODE_REPOS}/UniConsole
		$ENV{UCF_CODE_REPOS}/iStreamIO
		$ENV{UCF_CODE_REPOS}/UniConsole_imx
		$ENV{UCF_CODE_REPOS}/process-threads
		$ENV{UCF_CODE_REPOS}/4-Database/SystemCfgDb
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx
		$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystem_imx
		$ENV{UCF_CODE_REPOS}/StdIOChannelSystemModule
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src
		$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters
	)

	add_library(StdIOSystem SHARED
        ${AppFiles}
        ${SrcFiles}
	)

endif()

make_directory($ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/)

foreach(f ${AppFiles})
    add_custom_command(
        TARGET StdIOSystem
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy 
        ${f} 
        $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/
        )

endforeach()


add_custom_command(
    TARGET StdIOSystem
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy 
    ${CMAKE_CURRENT_LIST_FILE}
    $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/
    )


#***************************************************************************************************************************
# Create a Vim Project

file(WRITE xproj "\n")
    
file(APPEND xproj Build="${CMAKE_SOURCE_DIR}"\ CD=.\ flags=r\ {\n)
get_filename_component(xx ${CMAKE_CURRENT_LIST_FILE} NAME)
file(APPEND xproj \t${xx}\n)
file(APPEND xproj \t./build/Makefile\n)
file(APPEND xproj \n\n)

file(APPEND xproj \tApplication="$ENV{UCF_CODE_REPOS}"\ CD=.\ flags=r\ {\n)
foreach(f ${AppFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj \tInclude\ =\ "$ENV{UCF_CODE_REPOS}"\ CD=.\ flags=r\ {\n)
foreach(f ${IncFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj \tSrc\ =\ "$ENV{UCF_CODE_REPOS}"\ CD=.\ flags=r\ {\n)
foreach(f ${SrcFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj "}\n")

