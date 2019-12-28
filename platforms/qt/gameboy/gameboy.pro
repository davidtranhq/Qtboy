TEMPLATE = app
TARGET = QtBoy

CONFIG += c++17

QT = gui core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../include \
               ../include

SOURCES += \
    ../../../src/cartridge.cpp \
    ../../../src/debugger.cpp \
    ../../../src/disassembler.cpp \
    ../../../src/exception.cpp \
    ../../../src/instructions.cpp \
    ../../../src/mbc1.cpp \
    ../../../src/memory.cpp \
    ../../../src/ppu.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/rom.cpp \
    ../../../src/system.cpp \
    ../src/debuggerwindow.cpp \
    ../src/disassemblerwindow.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/memoryviewer.cpp \
    ../src/qt_renderer.cpp \
    ../src/tile.cpp

HEADERS += \
    ../../../include/cartridge.hpp \
    ../../../include/debug_types.hpp \
    ../../../include/debugger.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/instruction_info.hpp \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/ppu.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/renderer.hpp \
    ../../../include/rom.hpp \
    ../../../include/system.hpp \
    ../include/debuggerwindow.h \
    ../include/disassemblerwindow.h \
    ../include/mainwindow.h \
    ../include/memoryviewer.h \
    ../include/qt_renderer.h \
    ../include/tile.h
