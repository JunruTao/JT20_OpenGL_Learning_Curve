#version 440

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 5000) out;

uniform mat4 perspective;
uniform mat4 model;

in vec2 texOut[];
out vec2 texOut2;

void main() {

	vec4 vertex[6];

	vertex[0] = gl_in[0].gl_Position;
	vertex[1] = gl_in[1].gl_Position;
	vertex[2] = gl_in[2].gl_Position;
	vertex[3] = gl_in[3].gl_Position;
	vertex[4] = gl_in[4].gl_Position;
	vertex[5] = gl_in[5].gl_Position;
	

	vec4 Pm1 = (vertex[0] + vertex[2])/2;
	vec4 Pm2 = (vertex[0] + vertex[4])/2;
	vec4 Pm3 = (vertex[2] + vertex[4])/2;

	Pm1 = Pm1 - normalize(Pm1 - vertex[1]) * 1.4;
	Pm2 = Pm2 - normalize(Pm2 - vertex[5]) * 1.4;
	Pm3 = Pm3 - normalize(Pm3 - vertex[3]) * 1.4;

	vec2 Tm1 = (texOut[0] + texOut[2])/2;
	vec2 Tm2 = (texOut[0] + texOut[4])/2;
	vec2 Tm3 = (texOut[2] + texOut[4])/2;

	texOut2 = texOut[2];
	gl_Position = vertex[2];
	EmitVertex();

	texOut2 = Tm1;
	gl_Position = Pm1;
	EmitVertex();

	texOut2 = Tm3;
	gl_Position = Pm3;
	EmitVertex();

	EndPrimitive();


	texOut2 = texOut[0];
	gl_Position = vertex[0];
	EmitVertex();

	texOut2 = Tm1;
	gl_Position = Pm1;
	EmitVertex();

	texOut2 = Tm2;
	gl_Position = Pm2;
	EmitVertex();

	EndPrimitive();

	texOut2 = texOut[4];
	gl_Position = vertex[4];
	EmitVertex();

	texOut2 = Tm2;
	gl_Position = Pm2;
	EmitVertex();

	texOut2 = Tm3;
	gl_Position = Pm3;
	EmitVertex();

	EndPrimitive();

//	texOut2 = texOut[3];
//	gl_Position = vertex[3];
//	EmitVertex();
//
//	texOut2 = texOut[4];
//	gl_Position = vertex[4];
//	EmitVertex();
//
//	texOut2 = texOut[5];
//	gl_Position = vertex[5];
//	EmitVertex();
//
//	EndPrimitive();

//	vec4 offset = perspective * vec4(10.3f, 0, 0, 0);
//
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