TEMPLATE = app
TARGET = QtBoy

CONFIG += c++17 O3
QT = gui core multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#LIBS += -L ../lib/sdl2-64/lib -lSDL2

INCLUDEPATH += ../../../include \
               ../include \
               #../lib/sdl2-64/include

SOURCES += \
    ../../../src/apu.cpp \
    ../../../src/audio_types.cpp \
    ../../../src/cartridge.cpp \
    ../../../src/debugger.cpp \
    ../../../src/disassembler.cpp \
    ../../../src/exception.cpp \
    ../../../src/graphic_types.cpp \
    ../../../src/instructions.cpp \
#    ../../../src/main.cpp \
    ../../../src/joypad.cpp \
    ../../../src/mbc1.cpp \
    ../../../src/memory.cpp \
    ../../../src/noise_channel.cpp \
    ../../../src/ppu.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/rom.cpp \
    ../../../src/square_channel.cpp \
    ../../../src/system.cpp \
    ../../../src/timer.cpp \
    ../../../src/wave_channel.cpp \
    ../src/background_tab.cpp \
    ../src/debuggerwindow.cpp \
    ../src/disassemblerwindow.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/memoryviewer.cpp \
    ../src/qt_renderer.cpp \
    ../src/qt_speaker.cpp \
#    ../src/sdl_speaker.cpp \
    ../src/sprite_tab.cpp \
    ../src/tile.cpp \
    ../src/tile_tab.cpp \
    ../src/vram_window.cpp

HEADERS += \
    ../../../include/apu.hpp \
    ../../../include/audio_types.hpp \
    ../../../include/cartridge.hpp \
    ../../../include/debug_types.hpp \
    ../../../include/debugger.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/graphic_types.hpp \
    ../../../include/instruction_info.hpp \
    ../../../include/joypad.h \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/noise_channel.hpp \
    ../../../include/ppu.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/renderer.hpp \
    ../../../include/rom.hpp \
    ../../../include/speaker.hpp \
    ../../../include/square_channel.hpp \
    ../../../include/system.hpp \
    ../../../include/timer.hpp \
    ../../../include/wave_channel.hpp \
    ../include/background_tab.h \
    ../include/debuggerwindow.h \
    ../include/disassemblerwindow.h \
    ../include/mainwindow.h \
    ../include/memoryviewer.h \
    ../include/qt_renderer.h \
    ../include/qt_speaker.h \
#    ../include/sdl_speaker.h \
    ../include/sprite_tab.h \
    ../include/tile.h \
    ../include/tile_tab.h \
    ../include/vram_window.h \ \
    #../lib/sdl2-64/include/SDL.h
