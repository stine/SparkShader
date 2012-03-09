-- Vertex

#version 400

uniform mat4 MVPMatrix;
in vec3 position;

void main() {
  gl_Position = MVPMatrix * vec4(position, 1.0);
}


-- Fragment

#version 400

uniform vec4 Color;
out vec4 fragcolor;

void main() {
  fragcolor = Color;
}

