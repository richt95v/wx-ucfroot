cmake_minimum_required(VERSION 3.14)

set(PACKAGE_NAME _DbgModule)

project(DbgModule LANGUAGES CXX)

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

find_package(Qt5 COMPONENTS Widgets REQUIRED)	
# this is for a dialog that the module may display


if (MSVC)

    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_STATIC_DIR}/$<0:>)     
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules/$<0:>)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules/$<0:>)

    # warning level 4 and all warnings not as errors
    add_compile_options(/W4 /WX-)
    
    # no wide char bs
    add_compile_options(/Zc:wchar_t)
else()

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules)
    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/StaticModules)

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
    $ENV{UCF_CODE_REPOS}/DbgModule/DbgModule.cpp
    $ENV{UCF_CODE_REPOS}/DbgModule/DbgModule.hpp
    )

set (
    SrcFiles
	$ENV{UCF_CODE_REPOS}/imx/IOString.cpp
	$ENV{UCF_CODE_REPOS}/imx/StringBuffer.cpp
    #$ENV{UCF_CODE_REPOS}/AppEngine/src/MethodServer.cpp
	$ENV{UCF_CODE_REPOS}/imx/IOString.hpp
	$ENV{UCF_CODE_REPOS}/imx/StringBuffer.hpp
	$ENV{UCF_CODE_REPOS}/status/StatusOptional.hpp
    #$ENV{UCF_CODE_REPOS}/AppEngine/MethodServer.hpp
)

include_directories(
    $ENV{UCF_CODE_REPOS}/DbgModule
	$ENV{UCF_CODE_REPOS}/AppEngine
	$ENV{UCF_CODE_REPOS}/9-gsl
	$ENV{UCF_CODE_REPOS}/imx
	$ENV{UCF_CODE_REPOS}/status
	$ENV{UCF_CODE_REPOS}/files
	$ENV{UCF_CODE_REPOS}/AppEngine_imx
	$ENV{UCF_CODE_REPOS}/process-threads
	$ENV{UCF_CODE_REPOS}/UniConsole_imx
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr
)

add_library(DbgModule SHARED
    ${AppFiles}
    ${SrcFiles}
)

target_link_libraries(DbgModule Qt5::Widgets)

make_directory($ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/)

foreach(f ${AppFiles})
    add_custom_command(
        TARGET DbgModule 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy 
        ${f} 
        $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/
        )

endforeach()

add_custom_command(
    TARGET DbgModule 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy 
    ${CMAKE_CURRENT_LIST_FILE}
    $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/
    )

