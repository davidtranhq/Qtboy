TEMPLATE = app
TARGET = QtBoy-Console

CONFIG += c++17 console

INCLUDEPATH += ../../../include

SOURCES += \
    ../../../src/cartridge.cpp \
    ../../../src/debugger.cpp \
    ../../../src/disassembler.cpp \
    ../../../src/exception.cpp \
    ../../../src/instructions.cpp \
    ../../../src/main.cpp \
    ../../../src/mbc1.cpp \
    ../../../src/memory.cpp \
    ../../../src/ppu.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/rom.cpp \
    ../../../src/system.cpp \
    ../../../src/timer.cpp

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
    ../../../include/joypad.hpp \
    ../../../include/json_opcodes.hpp \
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
    ../../../include/thread_safe_system.hpp \
    ../../../include/timer.hpp \
    ../../../include/wave_channel.hpp
