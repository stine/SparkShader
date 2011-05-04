CC=g++
CFLAGS=-c -g -O0 -Wall -Iglm/
LDFLAGS=-L/usr/lib/fglrx -L/usr/local/lib/ -lGL -lX11 -lGLEW -lpthread -lXrandr -lm -lrt
SOURCES=bstrlib.C glsw.C main.C Cube.C Shader.C Stopwatch.C
OBJECTS=$(SOURCES:.C=.o)
EXECUTABLE=SparkShader

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) /usr/local/lib/libglfw.a -o $@

.C.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *~ shaders/*~ glm/*~