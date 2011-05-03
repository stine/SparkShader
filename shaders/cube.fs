#version 400 core

// Incoming Varyings
in vec4 vColor;

// Outgoing (final) Varyings
out vec4 fragcolor;

void main() {
  fragcolor = vColor;
}
