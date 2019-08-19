OBJS = processor.o system.o instructions.o main.o cartridge.o memory.o mbc1.o rom.o ram.o exception.o disassembler.o
CFLAGS = -g -std=c++17
	
all: $(OBJS) 
	g++ -o gameboy $(OBJS) $(CFLAGS)  
	
%.o : %.cpp
	g++ -c -o $@ $^ $(CFLAGS) 
	
.PHONY: clean

clean:
	del /q *.o