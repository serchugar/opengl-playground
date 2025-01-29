#version 460 core

layout (location = 0) in vec3 aPxCoords;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;

out vec3 fragColor;

uniform vec2 uWindowSize;

void main()
{
	// Convert pixel coords to NDC
	vec2 ndc = vec2(2.0 * aPxCoords.x / uWindowSize.x - 1.0,
					1.0 - 2.0 * aPxCoords.y / uWindowSize.y);

	fragColor = aColor;

	gl_Position = vec4(ndc, aPxCoords.z, 1.0);
	gl_PointSize = aSize;
}
