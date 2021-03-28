#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec2 texCoord0;
in vec4 c1;
in vec4 c2;
in vec4 c3;

//Zmienne interpolowane
out vec4 l;
out vec4 v;
out vec2 iTexCoord0;

mat4 Mtbn;;


void main(void) {
    Mtbn = mat4(c1,c2,c3,vec4(0,0,0,1));
    vec4 lp = vec4(0, -2, -6, 1); //przestrzeñ œwiata
    l = normalize(Mtbn * inverse(M) * lp - Mtbn*vertex); //wektor do œwiat³a w przestrzeni oka
    v = normalize(Mtbn * inverse(V*M) * vec4(0, 0, 0, 1) - Mtbn * vertex); //wektor do obserwatora w przestrzeni oka
    iTexCoord0 = texCoord0;
    
    gl_Position=P*V*M*vertex;
}
