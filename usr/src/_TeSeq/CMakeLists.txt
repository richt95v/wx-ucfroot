
cmake_minimum_required(VERSION 3.14)

set(PACKAGE_NAME _TeSeq)

project(CmakeTestExecSequencer LANGUAGES CXX)

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

if (MSVC)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/OptionalModules/$<0:>)

    # warning level 4 and all warnings not errors
    add_compile_options(/W4 /WX-)

else()

    #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_OPT_DIR})    
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $ENV{UCF_RELEASE_ROOT}/bin/$ENV{UCF_RELEASE_NAME}/$ENV{UCF_BIN_NAME}/OptionalModules)

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


include_directories(
	$ENV{UCF_CODE_REPOS}/9-gsl
	$ENV{UCF_CODE_REPOS}/imx
	$ENV{UCF_CODE_REPOS}/files
	$ENV{UCF_CODE_REPOS}/status
	$ENV{UCF_CODE_REPOS}/QtUtils
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx
	$ENV{UCF_CODE_REPOS}/AppEngine_imx
	$ENV{UCF_CODE_REPOS}/UniConsole_imx
	$ENV{UCF_CODE_REPOS}/1uTestExec_imx
	$ENV{UCF_CODE_REPOS}/process-threads
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr
	$ENV{UCF_CODE_REPOS}/1uTestExec/Sequencer
	$ENV{UCF_CODE_REPOS}/1uTestExec/Databases
	$ENV{UCF_CODE_REPOS}/StdIOChannelSystem_imx
	$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters
	$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src
)

set(
    AppFiles
	$ENV{UCF_CODE_REPOS}/1uTestExec/Sequencer/TestSlots.cpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Sequencer/TestExec1uSequencer.cpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Sequencer/TestSlots.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Sequencer/TestExec1uSequencer.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Databases/SqliteTestDb.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Databases/SqliteFixtureDb.hpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Databases/SqliteTestDb.cpp
	$ENV{UCF_CODE_REPOS}/1uTestExec/Databases/SqliteFixtureDb.cpp
)

set (
    SrcFiles
	$ENV{UCF_CODE_REPOS}/UniConsole_imx/iTestExec-1u.hpp
	$ENV{UCF_CODE_REPOS}/imx/IOString.cpp
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/1-hdr/ConnectionID.hpp
	$ENV{UCF_CODE_REPOS}/iStreamIO_imx/2-src/ConnectionID.cpp
	$ENV{UCF_CODE_REPOS}/process-threads/StripChartTimer.hpp
	$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters/SqliteIO.hpp
	$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/Sqlite3Src/sqlite3.c 
	$ENV{UCF_CODE_REPOS}/process-threads/StripChartTimer.cpp
	$ENV{UCF_CODE_REPOS}/4-Database/embedded/sqlite/adapters/SqliteIO.cpp
)

add_library(TeSequencer SHARED
    ${AppFiles}
    ${SrcFiles}
)

if (MSVC)

    target_link_libraries(
	    TeSequencer Qt5::Widgets
	    wsock32
	    ws2_32
    )

else()

    target_link_libraries(
	    TeSequencer Qt5::Widgets
    )

endif()

target_compile_definitions(TeSequencer PRIVATE CMAKESEQ_LIBRARY)

make_directory($ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/)

foreach(f ${AppFiles})
    add_custom_command(
        TARGET TeSequencer
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy 
        ${f} 
        $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/src/
        )

endforeach()

add_custom_command(
    TARGET TeSequencer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy 
    ${CMAKE_CURRENT_LIST_FILE}
    $ENV{UCF_RELEASE_ROOT}/usr/src/${PACKAGE_NAME}/
    )

add_custom_command(
    TARGET TeSequencer
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy_directory
    $ENV{UCF_CODE_REPOS}/1uTestExec_imx
    $ENV{UCF_RELEASE_ROOT}/usr/src/$ENV{UCF_RELEASE_NAME}/Api/1uTestExec_imx
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

