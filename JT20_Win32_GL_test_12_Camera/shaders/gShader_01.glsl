#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 300) out;

in vec2 texOut[];
out vec2 texOut2;

void main() {

	vec4 vertex[3];
	vertex[0] = gl_in[0].gl_Position;
	vertex[1] = gl_in[1].gl_Position;
	vertex[2] = gl_in[2].gl_Position;


	texOut2 = texOut[0];
	gl_Position = vertex[0];
	EmitVertex();

	texOut2 = texOut[1];
	gl_Position = vertex[1];
	EmitVertex();

	texOut2 = texOut[2];
	gl_Position = vertex[2];
	EmitVertex();

	EndPrimitive();


//	vec4 offset = {10.3f, 0, 0, 0};
//	texOut2 = texOut[0];
//	gl_Position = vertex[0] + offset;
//	EmitVertex();
//
//	texOut2 = texOut[1];
//	gl_Position = vertex[1] + offset;
//	EmitVertex();
//
//	texOut2 = texOut[2];
//	gl_Position = vertex[2] + offset;
//	EmitVertex();
}