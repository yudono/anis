CXX = clang++
CXXFLAGS = -std=c++11 -Wall -O2 -I/opt/homebrew/include -I/opt/homebrew/include/freetype2 -Wno-deprecated-declarations -Wno-c++11-narrowing
LDFLAGS = -L/opt/homebrew/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lglfw -lfreetype

BUILD_DIR = build
TARGET = $(BUILD_DIR)/dashboard
SRC = main.cpp renderer.cpp parser.cpp widgets.cpp layout.cpp
OBJ = $(SRC:%.cpp=$(BUILD_DIR)/%.o)

all: clean $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
