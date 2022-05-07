TEMPLATE = app
CONFIG += console debug warn_on
CONFIG -= app_bundle qt

# Setup global paths. The WEBOTS_HOME environment variable should be setup (system wide in etc/profile).
WEBOTS_HOME_PATH = $$(WEBOTS_HOME)
CONTROLLER_PATH = $$PWD
CONTROLLER_NAME = $$basename(CONTROLLER_PATH)

# Get C or C++ sources in the current directory (only).
SOURCES = $$files(*.c, true)
SOURCES += $$files(*.cpp, true)
HEADERS = $$files(*.h, true)

# Setup the project.
# Its name is defined to be the controller directory name.
TARGET = $$CONTROLLER_NAME
DESTDIR = $$CONTROLLER_PATH
QMAKE_TARGET = $$CONTROLLER_NAME

# Do not link with the Qt libraries :-)
CONFIG -= qt

# Link with the Webots controller library.
INCLUDEPATH += $$WEBOTS_HOME_PATH/include/controller/c $$WEBOTS_HOME_PATH/include/controller/cpp
LIBS += -L$$WEBOTS_HOME_PATH/lib/controller -lController -lCppController -lcar -lCppCar -ldriver -lCppDriver

win32 {
  CONFIG += console
}
macx {
  CONFIG -= app_bundle
  CONFIG += sdk_no_version_check
}

DISTFILES += \
    LICENSE \
    README.md \
    best_gen.txt
