# Build Doraemon's Treat Hunt on macOS or Linux.
#   make        build the game
#   make run    build and run
#   make clean  remove the binary

CXX      ?= clang++
CXXFLAGS ?= -std=c++11 -Wall -Wno-deprecated-declarations -O2
TARGET   := doraemon-treat-hunt
SRC      := main.cpp

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDLIBS := -framework GLUT -framework OpenGL
else
    LDLIBS := -lglut -lGLU -lGL
endif

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
