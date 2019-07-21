win32: include(gtest_dependency.pri)
unix: LIBS+=-L/public/devel/lib -L/usr/local/lib -lgtest

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

INCLUDEPATH += ../ForestGenerator/include/
SOURCES += main.cpp \
            ../ForestGenerator/src/LSystem.cpp \
            ../ForestGenerator/src/LSystem_createGeometry.cpp \
            ../ForestGenerator/src/LSystem_ForestMode.cpp \
            ../ForestGenerator/src/Instance.cpp

NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
        message("including $HOME/NGL")
        include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
        message("Using custom NGL location")
        include($(NGLDIR)/UseNGL.pri)
}
