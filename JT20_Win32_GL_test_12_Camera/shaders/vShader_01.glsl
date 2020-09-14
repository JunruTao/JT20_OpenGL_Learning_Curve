#version 440

in vec3 pos;
in vec2 tex;

out vec2 texOut;
uniform float Scale;
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(pos.xyz, 1.0);
	texOut = tex;
}