#version 330
in  vec2  TexCoords;
out vec4  FragColor;
  
uniform sampler2D screentexture;
uniform vec2  offsets[9];
uniform float blur_kernel[9];
uniform bool drunk;

void main()
{
    vec3 col = vec3(0.0f);
    vec3 sample[9];
    for(int i = 0; i < 9; i++)
    {
      sample[i] = vec3(texture(screentexture, TexCoords + offsets[i]));
      col += vec3(sample[i] * blur_kernel[i]);
    }
    if(drunk)
    {
        vec3 pink = vec3(0.9, 0.2, 1.0);
        FragColor = vec4(col * pink, 1.0f);
    }
    else
    {
        FragColor = vec4(col, 1.0f);
    }
} 