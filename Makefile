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
SOURCES = $(wildcard $(SRC_DIR)*.cpp)
DATA_HANDLER_SOURCES = $(wildcard $(DATA_HANDLER_DIR)*.cpp)
OBJS = $(SOURCES:.cpp=.o)
DATA_HANDLER_OBJS = $(DATA_HANDLER_SOURCES:.cpp=.o)
DATA_HANDLER_SHARED_OBJS = $(DATA_HANDLER_OBJS:.o=.so)
X64 = x64/

CXX = g++
CXX_VERSION_FLAG = -std=c++17
DATA_HANDLER_FLAGS = -lsqlite3 -ljson-c -lcrypto

UNAME_S := $(shell uname -s)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------


all: $(EXE)
	@echo Build complete for $(UNAME_S)

%.o: %.cpp
	$(CXX) $(DATA_HANDLER_FLAGS) -c -fPIC -o $@ $<

$(DATA_HANDLER_DIR)%.o: $(DATA_HANDLER_DIR)%.cpp
	$(CXX) $(DATA_HANDLER_FLAGS) -c -fPIC -o $@ $<

$(X64)libdata_handler.so: $(X64) $(DATA_HANDLER_OBJS)
	$(CXX) -shared -o $@ $(DATA_HANDLER_OBJS)

$(EXE): $(OBJS) $(X64)libdata_handler.so
	$(CXX) -o $@ -L$(X64) $^ $(DATA_HANDLER_FLAGS)

$(X64):
	mkdir -p $(X64)

clean:
	rm -rf $(EXE) $(OBJS) $(DATA_HANDLER_OBJS) $(X64) *.db *.log key_iv.bin