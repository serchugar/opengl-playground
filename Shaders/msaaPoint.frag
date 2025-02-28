#version 460 core

in float vFeather;

uniform vec3 uColor = vec3(0.0);
uniform bool uMSAA = true;

out vec4 FragColor;

void main() {
    /* All operations are done directly with the squared distance.

     dot(gl_PointCoord - center.x, gl_PointCoord - center.y); returns the 
     squared distance from the current pixel coord to the center.

     length(gl_PointCoord - center); returns the euclidean distance
     from the current pixel coord to the center. This functions performs 
     a square root. Which is expensive.

     Is much more efficient to use squared distances and do a few extra
     multiplications than to do a square root */

	vec2 center = vec2(0.5, 0.5);
    float distSq = dot(gl_PointCoord - center.x, gl_PointCoord - center.y);

    const float radius = 0.5;

    // Squared distances 
    const float radiusSq = radius * radius;
    float featherSq = (radius - vFeather) * (radius - vFeather);

	float alpha;

    if (uMSAA) {
        // Apply smooth alpha transition for MSAA effect
        alpha = smoothstep(radiusSq, featherSq, distSq);

    }
    else {
        // Hard-edged circle (binary alpha)
        alpha = (distSq <= radiusSq) ? 1.0 : 0.0;
    }

	// Do not compute the pixels with less alpha than threshold
	// This saves performance
	if (alpha < 0.01)
		discard;

    FragColor = vec4(uColor, alpha);
}