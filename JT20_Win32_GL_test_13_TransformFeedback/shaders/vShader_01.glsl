#version 440

in vec3 pos;
in vec2 tex;

out vec2 texOut;
uniform float Scale;
uniform mat4 perspective;
uniform mat4 model;

void main()
{
	//gl_Position = perspective * model * vec4(pos.xyz, 1.0);
	gl_Position = vec4(pos.xyz, 1.0);
	texOut = tex;
}