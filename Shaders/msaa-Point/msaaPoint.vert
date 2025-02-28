#version 460 core

layout (location = 0) in vec3 aPxCoords;

uniform vec2 uWindowSize;
uniform bool uNDCinShader = false;
uniform float uSize = 440.0;

out float vFeather;

void main() {
	if (uNDCinShader) {
		// Convert pixel coords to NDC
		vec2 ndc = vec2(2.0 * aPxCoords.x / uWindowSize.x - 1.0,
						1.0 - 2.0 * aPxCoords.y / uWindowSize.y);

		gl_Position = vec4(ndc, aPxCoords.z, 1.0);
	}
	else {
		gl_Position = vec4(aPxCoords, 1.0);
	}

	gl_PointSize = uSize;

	//   Compute feather value per vertex, done here so the frag shader does not do
	// this division per each pixel rendered
    //	 Inverse rule of 3. Wanted to maintain the smoothing achieved with a radius
	// of 100 and a feather of 0.02, so any size feels visually the same
	const float baseFeather = 2; // 100.0 * 0.02
	vFeather = baseFeather / uSize;
}