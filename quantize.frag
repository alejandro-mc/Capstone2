#version 330
in  vec2 texcoord;
out vec4 FragColor;
uniform  sampler2D texhandle;
uniform  float     levels;
uniform  float     dither;
uniform  int       width;
uniform  int       height;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
    //in this implementation there is always dither, what changes is the magnitude.

    //in a checkerboard pattern the positive dither occurs when row and column are both
    //even or odd, which means that their sum is even
    //otherwise the dither is negative
    //but to this we must map the texture coordinate to the original image coordinates
    //so we multiply both s and t coords by width and height respectiviely
    //the whole operation of mapping and adding both cordinates can be done
    //in one dot product operation which take one GPU cycle
    //since the mod operation will return 0 or 1 map this to -1 to 1 so we can use it
    //in determining the final color, that map also takes 1 GPU cycle
    //finally if dither is 1 we keep the computed direction if it's zero then
    //there is no direction
    float direction = (2.0 * mod(dot(texcoord.st,vec2(width,height)),2.0) - 1.0) * dither;

    //here we increase the magnitude of the direction vector by a little delta value of 0.01
    //and add it to the color to obtain the color before quantization is applied
    //0.004 = 1/255
    float noiseval = rand(gl_FragCoord.xy) * 0.05 + 0.05;
    vec4 color = vec4(vec3(noiseval),0.0) * vec4(vec3(direction),0) + texture(texhandle,texcoord.st);

    //in this step we apply quantization to the dithered fragment value
    FragColor = floor(color * vec4(vec3(levels),0.0) + vec4(vec3(0.5),0))/ vec4(vec3(levels),0.0);
}


