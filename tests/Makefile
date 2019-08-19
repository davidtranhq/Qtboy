OBJS = cartridge_tests.o cartridge.o mbc1.o rom.o ram.o
CFLAGS = -g -std=c++17
INCLUDE = -I..
VPATH = ..
	
all: $(OBJS) 
	g++ $(OBJS) $(INCLUDE) $(CFLAGS) -o cartridge_tests
	
%.o : %.cpp
	g++ -c $^ $(INCLUDE) $(CFLAGS) -o $@
	
