#version 330
in  vec2  TexCoords;
out vec4  FragColor;
  
uniform sampler2D screentexture;
uniform vec2  offsets[9];
uniform float blur_kernel[9];
uniform float Time;

void main()
{
    vec3 col = vec3(0.0f);
    vec3 sample[9];
    vec3 sample_r[9];
    for(int i = 0; i < 9; i++)
    {
      sample[i] = vec3(texture(screentexture, TexCoords + offsets[i]));
      col += vec3(sample[i] * blur_kernel[i]);
    }
    FragColor = vec4(col, 1.0f);
} 