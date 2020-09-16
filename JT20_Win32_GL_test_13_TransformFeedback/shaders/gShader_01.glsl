#version 440

layout(triangles) in;
layout(triangle_strip, max_vertices = 5000) out;

uniform mat4 perspective;
uniform mat4 model;
uniform sampler2D colorMap0;


    out float X;
    out float Y;
    out float Z;
    out float TX;
    out float TY;

in vec2 texOut[];
out vec2 tex;

void main() {


        vec4 vertex[3];
        vertex[0]=gl_in[0].gl_Position;
        vertex[1]=gl_in[1].gl_Position;
        vertex[2]=gl_in[2].gl_Position;

        gl_Position = vertex[2];

        
        X=gl_Position.x;
        Y=gl_Position.y;
        Z=gl_Position.z; 
        TX=texOut[2].x;
        TY=texOut[2].y;
        tex.x=TX;tex.y=TY;
        EmitVertex();

        gl_Position = (vertex[1]+vertex[2])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z; TX=(texOut[1].x+texOut[2].x)/2;TY=(texOut[1].y+texOut[2].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[0]+vertex[2])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z; TX=(texOut[0].x+texOut[2].x)/2;TY=(texOut[0].y+texOut[2].y)/2;tex.x=TX;tex.y=TY;EmitVertex();
        EndPrimitive();
        //////
        gl_Position = vertex[1] + texture(colorMap0,texOut[1]) * 0.3;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=texOut[1].x;TY=texOut[1].y;tex.x=TX;tex.y=TY;
        EmitVertex();

        gl_Position = (vertex[0]+vertex[1])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[0].x+texOut[1].x)/2;TY=(texOut[0].y+texOut[1].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[2]+vertex[1])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[2].x+texOut[1].x)/2;TY=(texOut[2].y+texOut[1].y)/2;tex.x=TX;tex.y=TY;EmitVertex();
        EndPrimitive();
        /////
        gl_Position = vertex[0];
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=texOut[0].x;TY=texOut[0].y;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[1]+vertex[0])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[1].x+texOut[0].x)/2;TY=(texOut[1].y+texOut[0].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[2]+vertex[0])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[2].x+texOut[0].x)/2;TY=(texOut[2].y+texOut[0].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        EndPrimitive();

        ///
        gl_Position = (vertex[1]+vertex[2])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[1].x+texOut[2].x)/2;TY=(texOut[1].y+texOut[2].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[1]+vertex[0])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[1].x+texOut[0].x)/2;TY=(texOut[1].y+texOut[0].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        gl_Position = (vertex[2]+vertex[0])/2;
        X=gl_Position.x;Y=gl_Position.y;Z=gl_Position.z;TX=(texOut[2].x+texOut[0].x)/2;TY=(texOut[2].y+texOut[0].y)/2;tex.x=TX;tex.y=TY;EmitVertex();

        EndPrimitive();

    

//----
    
//    vec4 pos_outs[3];
//    pos_outs[0] = gl_in[0].gl_Position;
//    pos_outs[1] = gl_in[1].gl_Position;
//    pos_outs[2] = gl_in[2].gl_Position;
//
//    X = pos_outs[0].x;
//    Y = pos_outs[0].y;
//    Z = pos_outs[0].z;
//    TX = texOut[0].x;
//    TY = texOut[0].y;
//	texOut2 = texOut[0];
//	gl_Position = pos_outs[0];
//	EmitVertex();
//
//    	
//    X = pos_outs[1].x;
//    Y = pos_outs[1].y;
//    Z = pos_outs[1].z;
//    TX = texOut[1].x;
//    TY = texOut[1].y;
//    texOut2 = texOut[1];
//	gl_Position = pos_outs[1];
//    EmitVertex();
//    	
//    X = pos_outs[2].x;
//    Y = pos_outs[2].y;
//    Z = pos_outs[2].z;
//    TX = texOut[2].x;
//    TY = texOut[2].y;
//    texOut2 = texOut[2];
//	gl_Position = pos_outs[2];
//    EmitVertex();
//    EndPrimitive();
//
    //--------//

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