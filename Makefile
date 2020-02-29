CXX = g++
WARNINGS = -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow
CXX_FLAGS = -g -std=c++17

# Final binary
BIN = gameboy

# Put all auto generated stuff to this build dir.
BUILD_DIR = build

# Change variables and commands based on OS
ifdef OS
    RM = del /Q
    RM_DIR = RMDIR $(BUILD_DIR) /S /Q
    FixPath = $(subst /,\,$1)
    OS = windows
	# Windows can't overwrite directories with mkdir
    MKDIR = if not exist $(BUILD_DIR) mkdir
    LIBS = -lmingw32
else
    ifeq ($(shell uname), Linux)
        RM = rm -f
        RM_DIR = rm -r -f $(BUILD_DIR)
        FixPath = $1
        OS = linux
        MKDIR = mkdir -p
        LIBS = 
	endif
endif

# List of all .cpp source files.
CPP = $(wildcard src/*.cpp)
# All .o files go to build dir.
OBJ = $(CPP:src/%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)
# Shows where to find the header files
INCLUDE = -Iinclude
# Libraries to link
LIBRARY = 
LINKER = 

# Target of the binary - depends on all .o files.
$(BIN) : $(OBJ)
    # Create build directories - same structure as sources.
	$(MKDIR) $(BUILD_DIR)
    # Just link all the object files.
	$(CXX) $(CXX_FLAGS) $^ $(LIBRARY) $(LINKER) -o $@

# Include all .d files
-include $(DEP)

# Build target for every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
$(BUILD_DIR)/%.o : src/%.cpp
	$(MKDIR) $(BUILD_DIR)
    # The -MMD flags additionaly creates a .d file with
    # the same name as the .o file.
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -MMD -c $< -o $@

.PHONY : clean
clean :
    # This should remove all generated files.
	$(RM) $(call FixPath,$(OBJ)) $(call FixPath,$(DEP))
	$(RM_DIR)