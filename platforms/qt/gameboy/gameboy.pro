TEMPLATE = app
TARGET = QtBoy

CONFIG += c++17 O3
QT = gui core multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../include \
               ../include \

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
    ../../../src/mbc2.cpp \
    ../../../src/mbc3.cpp \
    ../../../src/mbc5.cpp \
    ../../../src/memory.cpp \
    ../../../src/noise_channel.cpp \
    ../../../src/ppu.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/raw_audio.cpp \
    ../../../src/reusable_thread.cpp \
    ../../../src/rom.cpp \
    ../../../src/speaker.cpp \
    ../../../src/square_channel.cpp \
    ../../../src/system.cpp \
    ../../../src/thread_safe_system.cpp \
    ../../../src/timer.cpp \
    ../../../src/wave_channel.cpp \
    ../src/breakpoint_window.cpp \
    ../src/debugger_thread.cpp \
    ../src/debuggerwindow.cpp \
    ../src/disassemblerwindow.cpp \
    ../src/frame_buffer_tab.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/memory_viewer.cpp \
    ../src/memoryviewer.cpp \
    ../src/palette_tab.cpp \
    ../src/qt_renderer.cpp \
    ../src/qt_speaker.cpp \
    ../src/sprite_tab.cpp \
    ../src/tile.cpp \
    ../src/tile_tab.cpp \
    ../src/vram_window.cpp \
    ../src/window_tab.cpp

HEADERS += \
    ../../../include/apu.hpp \
    ../../../include/cartridge.hpp \
    ../../../include/debug_types.hpp \
    ../../../include/debugger.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/graphic_types.hpp \
    ../../../include/instruction_info.hpp \
    ../../../include/joypad.hpp \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/noise_channel.hpp \
    ../../../include/ppu.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/raw_audio.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/renderer.hpp \
    ../../../include/reusable_thread.hpp \
    ../../../include/rom.hpp \
    ../../../include/speaker.hpp \
    ../../../include/square_channel.hpp \
    ../../../include/system.hpp \
    ../../../include/thread_safe_system.hpp \
    ../../../include/timer.hpp \
    ../../../include/wave_channel.hpp \
    ../include/breakpoint_window.h \
    ../include/custom_palette_window.h \
    ../include/debugger_thread.h \
    ../include/debuggerwindow.h \
    ../include/disassemblerwindow.h \
    ../include/frame_buffer_tab.h \
    ../include/mainwindow.h \
    ../include/memory_viewer.h \
    ../include/memoryviewer.h \
    ../include/qt_renderer.h \
    ../include/qt_speaker.h \
#    ../include/sdl_speaker.h \
    ../include/sprite_tab.h \
    ../include/tile.h \
    ../include/tile_tab.h \
    ../include/vram_window.h \
    ../include/window_tab.h \
    ../include/palette_tab.h

