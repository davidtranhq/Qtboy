CXX = g++
WARNINGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
CXX_FLAGS = -g -std=c++17

# Final binary
BIN = gameboy

# Change variables and commands based on OS
ifdef OS
    RM = del /Q
    FixPath = $(subst /,\,$1)
    OS = windows
    MKDIR = if not exist $(call FixPath,$(@D)) mkdir $(call FixPath,$(@D))
    LIBS = -lmingw32
else
    ifeq ($(shell uname), Linux)
        RM = rm -f
        FixPath = $1
        OS = linux
        MKDIR = mkdir -p
        LIBS = 
	endif
endif

# Put all auto generated stuff to this build dir.
BUILD_DIR = build/$(OS)

# Specifies the platform-specfic directory
PLATFORM = platforms/$(OS)

# List of all .cpp source files.
CPP = $(wildcard src/*.cpp)

# All .o files go to build dir.
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)
# Shows where to find the header files
INCLUDE = -Iinclude \
	      -I$(PLATFORM)/sdl2/include
# Libraries to link
LIBRARY = -L$(PLATFORM)/sdl2/lib
LINKER = $(LIBS) -lSDL2main -lSDL2

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files.
$(BUILD_DIR)/$(BIN) : $(OBJ)
    # Create build directories - same structure as sources.
	$(MKDIR) $(call FixPath,$(@D))
    # Just link all the object files.
	$(CXX) $(CXX_FLAGS) $^ $(LIBRARY) $(LINKER) -o $@

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : %.cpp
	$(MKDIR) $(call FixPath,$(@D))
    # The -MMD flags additionaly creates a .d file with
    # the same name as the .o file.
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -MMD -c $< -o $@

.PHONY : clean
clean :
    # This should remove all generated files.
	$(RM) /q $(call FixPath,$(BUILD_DIR)/$(BIN)) $(call FixPath,$(OBJ)) $(call FixPath,$(DEP))