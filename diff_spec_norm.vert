#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    PointLight TangentPointLight[2];
    DirLight TangentDirLight;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 viewPos;
uniform PointLight pointLights[2];
uniform DirLight dirLight;

void main()
{
    vs_out.FragPos = vec3(M * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentPointLight = pointLights;
    for(int i = 0; i < 2; i++)
    {
        vs_out.TangentPointLight[i].position = TBN * pointLights[i].position;
    }
    vs_out.TangentDirLight = dirLight;
    vs_out.TangentDirLight.direction = TBN * dirLight.direction;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
        
    gl_Position = P * V * M * vec4(aPos, 1.0);
}