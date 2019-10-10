TEMPLATE = app
TARGET = QtBoy

CONFIG += c++17

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../include \
               ../include

SOURCES += \
    ../../../src/cartridge.cpp \
    ../../../src/disassembler.cpp \
    ../../../src/exception.cpp \
    ../../../src/instructions.cpp \
    ../../../src/mbc1.cpp \
    ../../../src/memory.cpp \
    ../../../src/processor.cpp \
    ../../../src/ram.cpp \
    ../../../src/rom.cpp \
    ../../../src/system.cpp \
    ../src/disassemblerwindow.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp

HEADERS += \
    ../../../include/cartridge.hpp \
    ../../../include/disassembler.hpp \
    ../../../include/exception.hpp \
    ../../../include/instruction_info.hpp \
    ../../../include/json.hpp \
    ../../../include/json_opcodes.hpp \
    ../../../include/memory.hpp \
    ../../../include/memory_bank_controller.hpp \
    ../../../include/processor.hpp \
    ../../../include/ram.hpp \
    ../../../include/register_pair.hpp \
    ../../../include/rom.hpp \
    ../../../include/system.hpp \
    ../include/disassemblerwindow.h \
    ../include/mainwindow.h
