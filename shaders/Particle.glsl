-- Vertex

#version 400

uniform mat4 MVPMatrix;
in vec3 position;
out vec4 vWorldPos;

void main() {
  vWorldPos = MVPMatrix * vec4(position, 1.0);
}


-- Geometry

#version 400

layout(triangles, invocations = 1) in;
layout(points, max_vertices = 200) out;

uniform mat4 MVPMatrix;
uniform float Time;
uniform float ElapsedSec;
uniform float BirthFrequency;
uniform float MaxAgeSec = 1.0;

in vec4 vWorldPos[];

out vec4 gWorldPos;
out vec3 gVelocity;
out float gAge;

const float TwoPi = 2.0 * 3.14159265;
const float InverseMaxInt = 1.0 / 4294967295.0;

float hashRand(uint seed, float b)
{
  uint i = (seed^12345391u) * 2654435769u;
  i^=(i<<6u)^(i>>26u);
  i*=2654435769u;
  i+=(i<<5u)^(i>>12u);
  return float(b * i) * InverseMaxInt;
}

void main()
{
  // Create random seed.
  uint seed = uint(Time * 1000.0) + uint(gl_PrimitiveIDIn);
  
  // Calculate vectors between points 0,1,2; A,B,C.
  vec4 vecAB = vWorldPos[1] - vWorldPos[0];
  vec4 vecAC = vWorldPos[2] - vWorldPos[0];
  
  // Calculate the triangle normal, in world coordinates.
  vec3 triNormal = cross(vec3(vecAB), vec3(vecAC));
  float triArea = length(triNormal);
  triNormal /= triArea;

  // Number of particles to emit, based on ElapsedTime, BirthFreq, and triArea.
  uint count = clamp(uint(triArea * ElapsedSec * BirthFrequency), 0, 200);
  
  gAge = 0.0;
  gVelocity = vec3(0.0);
  for (uint i = 0; i < count; ++i) {
    
    // Calculate a random position on the surface of the triangle.
    float r = hashRand(seed++, 1.0);
    float s = hashRand(seed++, 1.0);
    if (r + s >= 1.0) {
      r = 1 - r;
      s = 1 - s;
    }
    gWorldPos = vWorldPos[0] + r * vecAB + s * vecAC;
    gl_Position = gWorldPos;
    
    // Emit point.
    EmitVertex();
  }
}


-- Fragment

#version 400

out vec4 fragcolor;

void main() {
  fragcolor = vec4(1.0, 1.0, 1.0, 1.0);
}

