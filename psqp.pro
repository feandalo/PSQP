TEMPLATE = app
TARGET = PSQP

copyapp.commands = $(COPY_DIR) ./PSQP* ..
first.depends = $(first) copyapp
export(first.depends)
export(copyapp.commands)
QMAKE_EXTRA_TARGETS += first copyapp

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

QT += core \
    gui \
    opengl
LIBS += -L/usr/local/lib \ 
    -lgmp \
    -lpthread
MOC_DIR += ./tmp
UI_DIR += ./ui
INCLUDEPATH += ../include/ \
               ./ui/include
OBJECTS_DIR += ./obj

SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.h, true)

FORMS += ../ui/src/mainWindow.ui
RESOURCES += psqp.qrc
