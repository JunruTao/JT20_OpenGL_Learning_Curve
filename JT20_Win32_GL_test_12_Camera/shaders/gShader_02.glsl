#version 440

layout(points) in;
layout(triangle_strip, max_vertices = 300) out;

uniform mat4 MVP;
in vec2 texOut[];
out vec2 texOut2;


void main() {

	vec4 O_pt = gl_in[0].gl_Position;

	texOut2 = vec2(0.0f,0.0f); vec4 v1 = (O_pt + vec4(0, 0, 0.2f, 0)) * MVP;
	gl_Position = v1; EmitVertex();
	texOut2 = vec2(1.0f, 1.0f); vec4 v2 = (O_pt + vec4(-0.2f, 0, -0.1f, 0))* MVP;
	gl_Position = v2; EmitVertex();
	texOut2 = vec2(1.0f, 0.0f); vec4 v3 = (O_pt + vec4(0.2f, 0, -0.1f, 0))* MVP;
	gl_Position = v3; EmitVertex();
	EndPrimitive();

	vec3 up = cross(v1.xyz - v2.xyz, v3.xyz - v2.xyz);
	vec3 v4temp = O_pt.xyz + normalize(up)*0.3f;
	vec4 v4 = vec4(v4temp, 1);

	texOut2 = vec2(0.0f, 0.0f);
	gl_Position = v1; EmitVertex();
	texOut2 = vec2(1.0f, 1.0f);
	gl_Position = v2; EmitVertex();
	texOut2 = vec2(1.0f, 0.0f);
	gl_Position = v4; EmitVertex();
	EndPrimitive();

	texOut2 = vec2(0.0f, 0.0f);
	gl_Position = v2; EmitVertex();
	texOut2 = vec2(1.0f, 1.0f);
	gl_Position = v3; EmitVertex();
	texOut2 = vec2(1.0f, 0.0f);
	gl_Position = v4; EmitVertex();
	EndPrimitive();

	texOut2 = vec2(0.0f, 0.0f);
	gl_Position = v3; EmitVertex();
	texOut2 = vec2(1.0f, 1.0f);
	gl_Position = v1; EmitVertex();
	texOut2 = vec2(1.0f, 0.0f);
	gl_Position = v4; EmitVertex();
	EndPrimitive();

}