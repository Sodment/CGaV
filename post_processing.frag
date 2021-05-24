#version 330
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screentexture;
uniform sampler2D flowmap;
uniform float kernel[9];
uniform float Time;
uniform float Speed;
const float offset = 1.0 / 300.0;  


vec3 FlowUV(vec2 TexCoords,vec2 flow,vec2 jump, float time, bool flowB)
{
    float phaseOffset = flowB ? 0.5 : 0;
    float progress = fract(time + phaseOffset);
    vec3 uvw;
    uvw.xy = TexCoords - flow + progress + phaseOffset;
    uvw.xy += (time - progress) * jump;
    uvw.z = 1 - abs(1 - 2 * progress);
    return uvw;
}


void main()
{
    vec2 flowVector = texture(flowmap, TexCoords).xy * 2 - 1;
    float noise = texture(flowmap, TexCoords).a;
    float time = Time * Speed + noise;
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    
    vec3 sampleTex[9];
    vec3 texCoordA = FlowUV(TexCoords,flowVector,vec2(0.25,0.25), time, false);
    vec3 texCoordB = FlowUV(TexCoords,flowVector,vec2(0.25,0.25), time, true);

    vec4 texA = texture(screentexture, texCoordA.xy) * texCoordA.z;
    vec4 texB = texture(screentexture, texCoordB.xy) * texCoordB.z;
    for(int i = 0; i < 9; i++)
    {
         vec4 texA = texture(screentexture, texCoordA.xy + offsets[i]) * texCoordA.z;
         vec4 texB = texture(screentexture, texCoordB.xy + offsets[i]) * texCoordB.z;
         sampleTex[i] = texA.rgb + texB.rgb;
        }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * kernel[i];
    }
    FragColor = vec4(col, 1.0);
} 