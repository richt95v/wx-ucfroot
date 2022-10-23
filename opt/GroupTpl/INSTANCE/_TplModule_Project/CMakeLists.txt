cmake_minimum_required(VERSION 3.14)

set(PACKAGE_NAME _TplModule)

project(UcfTpl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
enable_language(C)

message("")
message("******************************************************************************************")
message("Required Environment Variables")
message("")
message("\tUCF_RELEASE_ROOT <./ucfroot | c:\\ucf\\ucfroot")
message("\tUCF_API = ./ucfroot/usr/<release name>/Api")
message("\tUCF_CODE_REPOS=/home/dev/1-projects/UniConsole-Framework/4-repo")
message("")
message("******************************************************************************************")
message("")

if (MSVC)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/$<0:>)

else()

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/)

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

set(
    AppFiles
    $ENV{UCF_CODE_REPOS}/UniConsole/Template/TplModule.hpp
    $ENV{UCF_CODE_REPOS}/UniConsole/Template/TplModule.cpp
    #${CMAKE_SOURCE_DIR}/Src/TplModule.hpp
    #${CMAKE_SOURCE_DIR}/Src/TplModule.cpp
    )

set(
    SrcFiles
	$ENV{UCF_API}/imx/IOString.cpp
	$ENV{UCF_API}/imx/TokenString.cpp
	$ENV{UCF_API}/imx/StringBuffer.cpp
    $ENV{UCF_API}/1uTestExec_imx/iUcfTestExec.cpp
    $ENV{UCF_API}/process-threads/SerialConsole.hpp
	$ENV{UCF_API}/process-threads/SerialConsole.cpp
	$ENV{UCF_API}/UniConsole_imx/GuardedNotification.cpp
    )

set(
    IncFiles
	$ENV{UCF_API}/imx/IOString.hpp
	$ENV{UCF_API}/imx/TokenString.hpp
	$ENV{UCF_API}/imx/StringBuffer.hpp
    $ENV{UCF_API}/1uTestExec_imx/iUcfTestExec.hpp
    $ENV{UCF_API}/process-threads/SerialConsole.hpp
	$ENV{UCF_API}/process-threads/SerialConsole.hpp
	$ENV{UCF_API}/UniConsole_imx/GuardedNotification.hpp
    )

include_directories(
    $ENV{UCF_CODE_REPOS}/UniConsole/Template
	$ENV{UCF_API}/imx
	$ENV{UCF_API}/status
	$ENV{UCF_API}/Utility
	$ENV{UCF_API}/AppEngine_imx
	$ENV{UCF_API}/iStreamIO_imx
	$ENV{UCF_API}/process-threads
	$ENV{UCF_API}/iStreamIO_imx/1-hdr
	$ENV{UCF_API}/StdIOChannelSystem_imx
    $ENV{UCF_API}/1uTestExec_imx
	$ENV{UCF_API}/UniConsole_imx
)

add_library(UcfTpl SHARED
    ${AppFiles}
    ${SrcFiles}
    ${IncFiles}
)

make_directory($ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/${PACKAGE_NAME}_Project/)
#make_directory($ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/)

foreach(f ${AppFiles})
    add_custom_command(
        TARGET UcfTpl 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy 
        ${f} 
        $ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/${PACKAGE_NAME}_Project/
        #$ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/
        )

endforeach()

add_custom_command(
    TARGET UcfTpl 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy 
    ${CMAKE_CURRENT_LIST_FILE}
    $ENV{UCF_RELEASE_ROOT}/opt/GroupTpl/INSTANCE/${PACKAGE_NAME}_Project/
    )


#***************************************************************************************************************************
# Create a Vim Project

file(WRITE xproj "\n")
    
file(APPEND xproj Build="${CMAKE_SOURCE_DIR}"\ CD=.\ flags=r\ {\n)
get_filename_component(xx ${CMAKE_CURRENT_LIST_FILE} NAME)
file(APPEND xproj \t${xx}\n)
file(APPEND xproj \t./build/Makefile\n)
file(APPEND xproj \n\n)

file(APPEND xproj \tApplication="$ENV{UCF_API}"\ CD=.\ flags=r\ {\n)
foreach(f ${AppFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj \tInclude\ =\ "$ENV{UCF_API}"\ CD=.\ flags=r\ {\n)
foreach(f ${IncFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj \tSrc\ =\ "$ENV{UCF_API}"\ CD=.\ flags=r\ {\n)
foreach(f ${SrcFiles})
    file(APPEND xproj \t${f}\n)
endforeach()
file(APPEND xproj \t}\n\n)

file(APPEND xproj "}\n")

