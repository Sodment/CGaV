#version 330
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float Time;
uniform float Speed;

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
    vec2 flowVector = texture(texture_specular1, TexCoords).xy * 2 - 1;
    float noise = texture(texture_specular1, TexCoords).a;
    float time = Time * Speed + noise;

    vec3 texCoordA = FlowUV(TexCoords,flowVector,vec2(0.25,0.25), time, false);
    vec3 texCoordB = FlowUV(TexCoords,flowVector,vec2(0.25,0.25), time, true);

    vec4 texA = texture(texture_diffuse1, texCoordA.xy) * texCoordA.z;
    vec4 texB = texture(texture_diffuse1, texCoordB.xy) * texCoordB.z;
    FragColor = vec4(texA.rgb + texB.rgb, 1.0);
} 