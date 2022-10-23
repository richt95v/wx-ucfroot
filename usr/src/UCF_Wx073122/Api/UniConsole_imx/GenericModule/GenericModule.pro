
VERSION=1
CONFIG-=debug_and_release
CONFIG-=debug_and_release_target

############################################################################################################################

include (../../../9-map/Links.pri)

TARGET = GenericUniConsoleModule
TEMPLATE = lib

DESTDIR=$${DISTRIBUTION}/libs
# override the DESTDIR set in Links.pri

############################################################################################################################

unix {

    QMAKE_CXXFLAGS +=-fpic
    QMAKE_CXXFLAGS +=-shared

    LIBS += -lboost_filesystem
    LIBS += -lboost_system
    LIBS += -lboost_program_options
    LIBS+= -ldl

    DEPENDPATH += $$PWD/../../../../../usr/lib
}

win32 {

    message("Defining winx build")

    QMAKE_CXXFLAGS +=  /Zc:strictStrings-

    DEFINES-=UNICODE
}

debug {

    message("dbg set")

    OBJECTS_DIR = $${REL_OBJ}

    unix {
        QMAKE_CXXFLAGS += -g
        QMAKE_CXXFLAGS += -g3
        QMAKE_CXXFLAGS += -ggdb
        QMAKE_CXXFLAGS += -O0
    }
}
else {

    message("release set")

    OBJECTS_DIR = $${REL_OBJ}
}

INCLUDEPATH+= \
	./SrcCode \
    $${API_DIR} \
    $${COMMON_CODE} \

SOURCES += \
    $${API_DIR}/IOString.cpp \
	./SrcCode/GenericModule.cpp 

HEADERS += \
	./SrcCode/GenericModule.hpp


