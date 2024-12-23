#version 460 core

layout (location = 0) in vec3 aPos; // Attribute position 0

void main()
{
	gl_Position = vec4(aPos.x, aPos.y*(-1.0), aPos.z, 1.0);
}