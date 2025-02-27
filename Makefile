#
# Cross Platform Makefile
# Compatible with Debian 12.8.0
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev

EXE = veritas-lock
SRC_DIR = src/
DATA_HANDLER_DIR = $(SRC_DIR)data_handler/
IMGUI_DIR = $(SRC_DIR)imgui/
VIEW_DIR = $(SRC_DIR)view/
SOURCES = $(wildcard $(SRC_DIR)*.cpp)
DATA_HANDLER_SOURCES = $(wildcard $(DATA_HANDLER_DIR)*.cpp)
IMGUI_SOURCES = $(wildcard $(IMGUI_DIR)*.cpp)
VIEW_SOURCES = $(wildcard $(VIEW_DIR)*.cpp)
OBJS = $(SOURCES:.cpp=.o) $(DATA_HANDLER_SOURCES:.cpp=.o) $(IMGUI_SOURCES:.cpp=.o) $(VIEW_SOURCES:.cpp=.o) 

CXX = g++
CXX_FLAGS = -std=c++17
# Uncomment the following line to enable logging of database calls
#CXX_FLAGS += -DDEBUG
DATA_HANDLER_FLAGS = -lsqlite3 -ljson-c -lcrypto
IMGUI_FLAGS = -I$(IMGUI_DIR)
LIBS =

UNAME_S := $(shell uname -s)

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`

	IMGUI_FLAGS += `sdl2-config --cflags`
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(EXE)
	@echo Build complete for $(UNAME_S)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) $(DATA_HANDLER_FLAGS) $(IMGUI_FLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(DATA_HANDLER_FLAGS) $(IMGUI_FLAGS) $(LIBS)

clean:
	rm -rf $(EXE) $(OBJS) *.d *.db *.log key_iv.bin


##---------------------------------------------------------------------
## Automatic dependencies
##---------------------------------------------------------------------

# Generate dependency files for .cpp
# -include $(SOURCES:.cpp=.d) $(DATA_HANDLER_SOURCES:.cpp=.d)

# %.d: %.cpp
# 	@$(CXX) -MM -MT $(@:.d=.o) $< > $@