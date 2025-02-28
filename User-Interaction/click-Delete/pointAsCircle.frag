#version 460

in vec3 fragColor;

uniform vec3 outline = vec3(43/255.0, 43/255.0, 43/255.0 );
uniform int renderMode = 2;

out vec4 finalColor;

void main()
{
	float dist = dot(gl_PointCoord - 0.5, gl_PointCoord - 0.5);

	// Do not render anything further away than 0.5 radius. Do not modify this. To increase point size, do it in the vertex shader with gl_PointSize
	if(dist > 0.25) // Radius squared
		discard;

	// MODE 1: Outline
	if (renderMode == 1)
	{
		// Secondary color for fragments in the outline
		if(dist > 0.2)
			finalColor = vec4(outline, 1.0);

		// Primary color for fragments closer to the center
		else
			finalColor = vec4(fragColor, 1.0);
	}

	// MODE 2: Gradient
	else if (renderMode == 2)
	{
		if(dist > 0.25)
			finalColor = vec4(outline, 1.0);

		// Primary color for fragments closer to the center
		else
		{
			float blendFactor = smoothstep(0.0, 0.3, dist);

			vec3 gradientColor = mix(fragColor, outline, blendFactor);

			finalColor = vec4(gradientColor, 1.0);
		}
	}

	// MODE 0: Solid Color
	else
		finalColor = vec4(fragColor, 1.0);
}