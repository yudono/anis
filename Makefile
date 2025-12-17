CXX = clang++
CXXFLAGS = -std=c++11 -Wall -O2 -I/opt/homebrew/include -I/opt/homebrew/include/freetype2 -Wno-deprecated-declarations -Wno-c++11-narrowing
LDFLAGS = -L/opt/homebrew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lglfw -lfreetype

BUILD_DIR = build
# Source directories
GUI_DIR = lib/gui

# Source files
# LANG_SRC = $(wildcard core/lang/*.cpp) # Use manual list if wildcard issues
LANG_SRC = core/lang/lexer.cpp core/lang/parser.cpp core/lang/interpreter.cpp
LIB_SRC = lib/register.cpp
GUI_SRC = lib/gui/renderer.cpp lib/gui/parser.cpp lib/gui/widgets.cpp lib/gui/layout.cpp lib/gui/minigui.cpp
MAIN_SRC = sunda.cpp

# Object files
LANG_OBJ = $(patsubst core/lang/%.cpp, build/lang_%.o, $(LANG_SRC))
LIB_OBJ = build/register.o
GUI_OBJ = $(patsubst lib/gui/%.cpp, build/gui_%.o, $(GUI_SRC)) # Adjusted to match build rule
MAIN_OBJ = build/sunda.o

# Define OBJS for the default target
OBJS = $(MAIN_OBJ) $(LIB_OBJ) $(LANG_OBJ) $(GUI_OBJ)

TARGET = build/sunda

# Include directories
INCLUDES = -I/opt/homebrew/include -I/opt/homebrew/include/freetype2 -Ilib/gui -Icore/lang -I.

all: clean $(TARGET)

# Build rules
build/gui_%.o: $(GUI_DIR)/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/main.o: main.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Manual mapping for clarity due to mixed sources
build/renderer.o: $(GUI_DIR)/renderer.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/parser.o: $(GUI_DIR)/parser.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/widgets.o: $(GUI_DIR)/widgets.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/layout.o: $(GUI_DIR)/layout.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/minigui.o: $(GUI_DIR)/minigui.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Standard link
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

counter: build/counter.o build/renderer.o build/parser.o build/widgets.o build/layout.o build/minigui.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $(BUILD_DIR)/counter $(LDFLAGS)

# Sunda Language
build/sunda.o: sunda.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Pattern rule for language sources
build/lang_%.o: core/lang/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/register.o: lib/register.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

SUNDA_OBJS = build/sunda.o build/register.o build/lang_lexer.o build/lang_parser.o build/lang_interpreter.o \
             build/renderer.o build/parser.o build/widgets.o build/layout.o build/minigui.o

sunda: $(SUNDA_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SUNDA_OBJS) -o $(BUILD_DIR)/sunda $(LDFLAGS)


clean:
	rm -rf $(BUILD_DIR)
