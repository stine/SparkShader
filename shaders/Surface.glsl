-- Vertex

#version 400

in vec4 worldPos;
in vec3 velocity;
in float age;

out vec4 vWorldPos;
out vec3 vVelocity;
out float vAge;

void main() {
  vWorldPos = worldPos;
  vVelocity = velocity;
  vAge = age;
}


-- Geometry

#version 400

layout(points, invocations = 1) in;
layout(points, max_vertices = 1) out;

uniform float ElapsedSec;
uniform vec3  WorldAccel;
uniform float MaxAgeSec = 1.0;

in vec4 vWorldPos[];
in vec3 vVelocity[];
in float vAge[];

out vec4 gWorldPos;
out vec3 gVelocity;
out float gAge;

void main() {

  // Calculate new position based on ElapsedSec and velocity.
  gWorldPos = vec4(vVelocity[0], 0.0) * ElapsedSec + vWorldPos[0];
  gl_Position = gWorldPos;

  // Calculate new velocity based on ElapsedSec and WorldAccel.
  gVelocity = WorldAccel * ElapsedSec + vVelocity[0];

  // Calculate new age.
  gAge = vAge[0] + ElapsedSec;

  // Determine if point has died.
  if (gAge < MaxAgeSec) {
    EmitVertex();
  }
}


-- Fragment

#version 400

uniform float MaxAgeSec = 1.0;

in vec4 gWorldPos;
in vec3 gVelocity;
in float gAge;

out vec4 fragcolor;

void main() {
  // TODO set color based on age.
  float life = gAge / MaxAgeSec;
  fragcolor = vec4(1.0, 1.0 - life, 0.0, 1.0 - life);
}
