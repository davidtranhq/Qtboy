SDL2 = ../../sdl2
OBJS = tile_tests.o tile.o
CFLAGS = -g -std=c++17
INCLUDE = -I../.. \
		  -I$(SDL2)/include
LIBRARY = -lmingw32 -lSDL2main -lSDL2
LINKER = -L$(SDL2)/lib
VPATH = ../..
	
all: $(OBJS) 
	g++ $(OBJS) $(INCLUDE) $(LIBRARY) $(LINKER) $(CFLAGS) -o tile_tests
	
%.o : %.cpp
	g++ -c $^ $(INCLUDE) $(LIBRARY) $(LINKER) $(CFLAGS) -o $@
	
.PHONY: clean

clean:
	del *.o
