#version 330
in  vec2 texcoord;
out vec4 FragColor;
uniform  sampler2D texhandle;
uniform  float     threshold;

void main(void)
{
    FragColor = ceil(texture(texhandle,texcoord.st) - vec4(threshold,threshold,threshold,0.0));
    //FragColor = texture(texhandle,texcoord.st) + vec4(threshold,threshold,threshold,0.0);
}
