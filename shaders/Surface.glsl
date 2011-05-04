-- Vertex

#version 400

// Incoming Attributes
in vec3  position;
in float maxRadius;
in float ageSeconds;

// Outgoing Varyings
out vec3  positionV;
out float maxRadiusV;
out float ageSecondsV;

void main() {

  // Pass along necessary values to the GS.
  positionV   = position;
  maxRadiusV  = maxRadius;
  ageSecondsV = ageSeconds;
}


-- Geometry

#version 400

layout(points, invocations = 1) in;
layout(points, max_vertices = 1) out;

// Uniforms
uniform mat4 modelViewProjectionMatrix;
uniform float radiusChangeSpeed;
uniform float secondsElapsed;

// Incoming Varyings
in vec3  positionV[];
in float maxRadiusV[];
in float ageSecondsV[];

// Outgoing Varyings
out vec3  ptPosition;
out float ptMaxRadius;
out float ptAgeSeconds;

void main() {

  // Set appropriate 'out' values for this vertex.
  ptPosition   = positionV[0];
  ptMaxRadius  = maxRadiusV[0];
  ptAgeSeconds = ageSecondsV[0] + secondsElapsed;

  // Calculate the radius of the point.  Linear progression
  // throughout 0 -> maxRadius -> 0.
  float radius = ageSecondsV[0] * radiusChangeSpeed;
  if (radius > maxRadiusV[0]) {
    radius = 2 * maxRadiusV[0] - radius;
  }
  gl_PointSize = radius;

  // Calculate the position of this point.
  gl_Position = modelViewProjectionMatrix * vec4(positionV[0], 1.0);

  // If radius > 0.0f, the point will be drawn and continues to live.
  if (radius >= 0.0f) {
    EmitVertex();
  }
}


-- Fragment

#version 400

// Outgoing (final) Varyings
out vec4 fragcolor;

void main() {
  fragcolor = vec4(1.0, 1.0, 0.0, 1.0);
}
