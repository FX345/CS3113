#version 330 core

attribute vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	// Handle both 2D and 3D positions correctly
	vec4 p = viewMatrix * modelMatrix * position;
	gl_Position = projectionMatrix * p;
}
