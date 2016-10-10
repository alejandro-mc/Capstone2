#version 330
in  vec2 texcoord;
out vec4 FragColor;
uniform  sampler2D texhandle;

void main(void)
{
    //FragColor = vec4(1.0,1.0,1.0,1.0);//make it white
    FragColor = texture(texhandle,texcoord.st);
}
