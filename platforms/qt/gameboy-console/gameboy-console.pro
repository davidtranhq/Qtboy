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
    ../../../include/cartridge.hpp \
    ../../../include/debug_types.hpp \
    ../../../include/debugger.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/instruction_info.hpp \
    ../../../include/json_opcodes.hpp \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/ppu.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/renderer.hpp \
    ../../../include/rom.hpp \
    ../../../include/system.hpp \
    ../../../include/timer.hpp
