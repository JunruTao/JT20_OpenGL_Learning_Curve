#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 5000) out;

uniform mat4 perspective;
uniform mat4 model;

    out float X;
    out float Y;
    out float Z;
    out float TX;
    out float TY;

in vec2 texOut[];
out vec2 texOut2;

void main() {

    
    vec4 pos_outs[3];
    pos_outs[0] = gl_in[0].gl_Position;
    pos_outs[1] = gl_in[1].gl_Position;
    pos_outs[2] = gl_in[2].gl_Position;

    X = pos_outs[0].x;
    Y = pos_outs[0].y;
    Z = pos_outs[0].z;
    TX = texOut[0].x;
    TY = texOut[0].y;
	texOut2 = texOut[0];
	gl_Position = pos_outs[0];
	EmitVertex();

    	
    X = pos_outs[1].x;
    Y = pos_outs[1].y;
    Z = pos_outs[1].z;
    TX = texOut[1].x;
    TY = texOut[1].y;
    texOut2 = texOut[1];
	gl_Position = pos_outs[1];
    EmitVertex();
    	
    X = pos_outs[2].x;
    Y = pos_outs[2].y;
    Z = pos_outs[2].z;
    TX = texOut[2].x;
    TY = texOut[2].y;
    texOut2 = texOut[2];
	gl_Position = pos_outs[2];
    EmitVertex();
    EndPrimitive();

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