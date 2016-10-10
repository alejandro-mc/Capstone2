#version 330

in        vec2   a_Pos;       //vertex position
in        vec2   a_Textcoord; //vertex texture coordinate
out       vec2   texcoord;
uniform   mat4   u_ModelView;
uniform   mat4   u_proj;
uniform   mat4   u_scaleRec;

void main(void)
{
    gl_Position = u_proj * u_ModelView * u_scaleRec * vec4(a_Pos,0.0,1.0);
    texcoord = a_Textcoord;
}
