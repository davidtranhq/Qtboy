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
    ../../../src/graphics.cpp \
    ../../../src/instructions.cpp \
#    ../../../src/main.cpp \
    ../../../src/joypad.cpp \
    ../../../src/mbc1.cpp \
    ../../../src/memory.cpp \
    ../../../src/ppu.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/rom.cpp \
    ../../../src/system.cpp \
    ../../../src/timer.cpp \
    ../src/background_tab.cpp \
    ../src/debuggerwindow.cpp \
    ../src/disassemblerwindow.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/memoryviewer.cpp \
    ../src/qt_renderer.cpp \
    ../src/sprite_tab.cpp \
    ../src/tile.cpp \
    ../src/tile_tab.cpp \
    ../src/vram_window.cpp

HEADERS += \
    ../../../include/cartridge.hpp \
    ../../../include/debug_types.hpp \
    ../../../include/debugger.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/graphics.h \
    ../../../include/instruction_info.hpp \
    ../../../include/joypad.h \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/ppu.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/renderer.hpp \
    ../../../include/rom.hpp \
    ../../../include/system.hpp \
    ../../../include/timer.hpp \
    ../include/background_tab.h \
    ../include/debuggerwindow.h \
    ../include/disassemblerwindow.h \
    ../include/mainwindow.h \
    ../include/memoryviewer.h \
    ../include/qt_renderer.h \
    ../include/sprite_tab.h \
    ../include/tile.h \
    ../include/tile_tab.h \
    ../include/vram_window.h
