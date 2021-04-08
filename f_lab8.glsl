#version 330

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 l;
in vec4 v;
in vec2 iTexCoord0;


vec2 parralaxTexCoords(vec4 v, vec2 t, float h, float s)
{
	vec2 ti = -h*v.xy / s;
	float hi = -1 / s;

	vec2 tc = t;
	float hc = 1;
	float ht = texture(texture_specular1, tc).r;

	if(v.z <= 0) discard;

	while(hc > ht) {
		tc = tc+ti;
		if(tc.x < 0 || tc.x > 1 || tc.y <0 || tc.y > 1) discard;
		hc = hc + hi;
		ht = texture(texture_specular1, tc).r;
	}

	vec2 tco = tc -ti;
	float hco = hc - hi;
	float hto = h * texture(texture_specular1, tco).r;

	float x = (hco-hto) / (hco - hto - (hc - ht));

    return (1-x)*tco + x * tc;
}

void main(void) {

	//Znormalizowane interpolowane wektory
	vec4 mv = normalize(v);
	vec2 nt = parralaxTexCoords(mv,iTexCoord0,0.1,100);

	vec4 ml = normalize(l);
	vec4 mn = normalize(vec4(texture(texture_normal1, nt).rgb * 2 - 1, 0));
	//Wektor odbity
	vec4 mr = reflect(-ml, mn);

	//Parametry powierzchni
	vec4 kd = texture(texture_diffuse1, nt); 
	vec4 ks = texture(texture_height1, nt);

	//Obliczenie modelu oœwietlenia
	float nl = clamp(dot(mn, ml), 0, 1);
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25);
	pixelColor= vec4(kd.rgb * nl, kd.a) + vec4(ks.rgb*rv, 0);
}
