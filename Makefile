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
SOURCES = $(wildcard $(SRC_DIR)*.cpp)
DATA_HANDLER_SOURCES = $(wildcard $(DATA_HANDLER_DIR)*.cpp)
IMGUI_SOURCES = $(wildcard $(IMGUI_DIR)*.cpp)
OBJS = $(SOURCES:.cpp=.o)
DATA_HANDLER_OBJS = $(DATA_HANDLER_SOURCES:.cpp=.o)
IMGUI_OBJS = $(IMGUI_SOURCES:.cpp=.o)
DATA_HANDLER_SHARED_OBJS = $(DATA_HANDLER_OBJS:.o=.so)
IMGUI_SHARED_OBJS = $(IMGUI_OBJS:.o=.so)
X64 = x64/

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
	$(CXX) $(CXX_FLAGS) $(DATA_HANDLER_FLAGS) -c -fPIC -o $@ $<

$(DATA_HANDLER_DIR)%.o: $(DATA_HANDLER_DIR)%.cpp
	$(CXX) $(CXX_FLAGS) $(DATA_HANDLER_FLAGS) -c -fPIC -o $@ $<

$(IMGUI_DIR)%.o: $(IMGUI_DIR)%.cpp
	$(CXX) $(CXX_FLAGS) $(IMGUI_FLAGS) -c -fPIC -o $@ $<

$(X64)libdata_handler.so: $(X64) $(DATA_HANDLER_OBJS)
	$(CXX) $(CXX_FLAGS) -shared -o $@ $(DATA_HANDLER_OBJS)

$(X64)libimgui.so: $(X64) $(IMGUI_OBJS)
	$(CXX) $(CXX_FLAGS) -shared -o $@ $(IMGUI_OBJS)

$(EXE): $(OBJS) $(X64)libdata_handler.so $(X64)libimgui.so
	$(CXX) $(CXX_FLAGS) -o $@ -L$(X64) $^ $(DATA_HANDLER_FLAGS) $(IMGUI_FLAGS) $(LIBS)

$(X64):
	mkdir -p $(X64)

clean:
	rm -rf $(EXE) $(OBJS) $(DATA_HANDLER_OBJS) $(X64) *.d *.db *.log key_iv.bin

##---------------------------------------------------------------------
## Automatic dependencies
##---------------------------------------------------------------------

# Generate dependency files for .cpp
# -include $(SOURCES:.cpp=.d) $(DATA_HANDLER_SOURCES:.cpp=.d)

# %.d: %.cpp
# 	@$(CXX) -MM -MT $(@:.d=.o) $< > $@