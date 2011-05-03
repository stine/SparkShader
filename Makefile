CC=g++
CFLAGS=-c -g -O0 -Wall -I./glm/ -DGL_GLEXT_PROTOTYPES
LDFLAGS=-lX11 -lGL -lGLU
SOURCES=main.C Cube.C Shader.C Stopwatch.C
OBJECTS=$(SOURCES:.C=.o)
EXECUTABLE=GL4Demo

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.C.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *~ shaders/*~ glm/*~