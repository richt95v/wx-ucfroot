cmake_minimum_required(VERSION 3.1.0)

project(CmakeTeGui VERSION 1.0.0 LANGUAGES CXX)

set(PACKAGE_NAME _TeGUI)

#   compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
    # do MSVC specific things
    
    #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_OPT_DIR}/$<0:>)    
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/OptionalModules/$<0:>)

    # example -> add_compile_options(/W4 /WX)
    # set(CMAKE_USE_WIN32_THREADS_INIT 0)

else()

    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_OPT_DIR})    
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/OptionalModules)

    add_compile_options(
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

#   qt related settins
set(CMAKE_AUTOMOC ON)       # invokes moc as needed
set(CMAKE_AUTORCC ON)       # will automatically handle .qrc files
set(CMAKE_AUTOUIC ON)       # automatically invoke qt uic when necessary (or not)


find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(
    AppFiles
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui/UniTestExec_1uWindow.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui/TestExec1u.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui/UniTestExec_1uWindow.cpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui/TestExec1u.cpp
	$ENV{UCF_CODE_REPOS}/QtComponents/SerialNumberDialog/SerialNumberDialog.hpp
	$ENV{UCF_CODE_REPOS}/QtComponents/SerialNumberDialog/SerialNumberDialog.cpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui/UniTestExec_1uWindow.ui
	$ENV{UCF_CODE_REPOS}/QtComponents/SerialNumberDialog/SerialNumberDialog.ui
)

set(
    SrcFiles
	$ENV{UCF_CODE_REPOS}/imx/IOString.hpp
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr/ConnectionID.hpp
	$ENV{UCF_CODE_REPOS}/files/PathLW.hpp
	$ENV{UCF_CODE_REPOS}/imx/IOString.cpp
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/ConnectionID.cpp
)

add_library(TeGui SHARED
    ${AppFiles}
    ${SrcFiles}
)

target_include_directories(TeGui PUBLIC
    $ENV{UCF_CODE_REPOS}/imx
	$ENV{UCF_CODE_REPOS}/files
	$ENV{UCF_CODE_REPOS}/AppEngine_imx
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx
	$ENV{UCF_CODE_REPOS}/UniConsole_imx
	$ENV{UCF_CODE_REPOS}/1uTestExec/Gui
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr
	$ENV{UCF_CODE_REPOS}/QtUtils
	$ENV{UCF_CODE_REPOS}/QtComponents/SerialNumberDialog
    )

target_link_libraries(TeGui Qt5::Widgets)

make_directory($ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/)

foreach(f ${AppFiles})
    add_custom_command(
        TARGET TeGui 
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy 
        ${f} 
        $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/
        )

endforeach()

add_custom_command(
    TARGET TeGui
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

