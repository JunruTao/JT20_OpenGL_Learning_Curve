#version 440


uniform float Scale;
uniform sampler2D colorMap0;
//uniform sampler2D normalMap0;

in vec2 tex;
layout(location = 0) out vec4 FragColor0;

void main() 
{
	//vec3 normal = texture(normalMap0, texOut2).rgb;
	FragColor0 = texture(colorMap0, tex);
}
