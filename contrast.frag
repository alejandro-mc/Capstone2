#version 330
in  vec2 texcoord;
out vec4 FragColor;
uniform  sampler2D texhandle;
uniform  float     contrast;
uniform  float     brightness;

void main(void)
{
    FragColor = (texture(texhandle,texcoord.st) - vec4(0.5,0.5,0.5,0)) * contrast
                + vec4(0.5,0.5,0.5,0.0)
                + vec4(brightness,brightness,brightness,0.0);
}
