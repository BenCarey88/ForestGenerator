# This specifies the exe name
TARGET=ForestGenerator
# where to put the .o files
OBJECTS_DIR=obj
# where to put moc auto generated files
MOC_DIR=moc
# where our exe is going to live (root of project)
DESTDIR=./

# core Qt Libs to use add more here if needed.
QT+=gui opengl core
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
        cache()
        DEFINES +=QT5BUILD
}
# on a mac we don't create a .app bundle file ( for ease of multiplatform use)
CONFIG-=app_bundle
# were are going to default to a console app
CONFIG += console

# Auto include all .cpp files in the project src directory
SOURCES+= $$PWD/src/*.cpp
# same for the .h files
HEADERS+= $$PWD/include/*.h
#and for .ui files
FORMS += $$PWD/ui/*.ui
OTHER_FILES+= README.md \
              shaders/*.glsl
# and add the include dir into the search path for Qt and make
INCLUDEPATH +=./include \
                $(HOME)/boost_1_70_0/boost

NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
        message("including $HOME/NGL")
        include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
        message("Using custom NGL location")
        include($(NGLDIR)/UseNGL.pri)
}
