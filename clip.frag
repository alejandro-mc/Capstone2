#version 330
in  vec2 texcoord;
out vec4 FragColor;
uniform  sampler2D texhandle;
uniform  float     threshold1;
uniform  float     threshold2;

void main(void)
{
    FragColor = clamp(texture(texhandle,texcoord.st),threshold1,threshold2);
}
