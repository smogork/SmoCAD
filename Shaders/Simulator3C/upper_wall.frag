#version 330

#define NLIGHTS 1

in vec2 TexCoord;
in vec3 WorldPos;

out vec4 FragColor;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};


uniform sampler2D heightTexture;
uniform float u_MaxHeight;
uniform int u_TextureSize;
uniform MVP u_MVP;


const ivec3 off = ivec3(-1,0,1);
const vec3 lightPos[NLIGHTS] = vec3[](vec3(3, 10, 3));
const vec3 lightColor[NLIGHTS] = vec3[](vec3(1.0f, 1.0f, 1.0f));
const float ka = 0.1f;
const float kd = 0.6f;
const float ks = 0.2f;
const float m = 100.0f;
const vec3 surfaceColor = vec3(0.6f, 0.5f, 0.6f);

vec4 saturate(vec4 v)
{
    return clamp(v, 0.0f, 1.0f);
}

float saturate(float v)
{
    return clamp(v, 0.0f, 1.0f);
}

float textureToHeight(float textureVal)
{
    return (1.0f - textureVal) * u_MaxHeight;
}

vec3 normalMapping(vec3 N, vec3 T, vec3 tn)
{
    vec3 B = normalize(cross(N, T));
    T = cross(B, N);
    //mat3 conversion = transpose(mat3(T, B, N));
    mat3 conversion = transpose(mat3(vec3(1,0,0), vec3(0, 0, 1), vec3(0,1,0)));
    return conversion * tn;
}

vec4 phong(vec3 worldPos, vec3 norm, vec3 view)
{
	view = normalize(view);
	norm = normalize(norm);
	vec3 color = surfaceColor * ka; //ambient
	for (int k = 0; k < NLIGHTS; ++k)
	{
		vec3 lightVec = normalize(lightPos[k].xyz - worldPos);
		vec3 halfVec = normalize(view + lightVec);
		color += lightColor[k] * kd * surfaceColor * saturate(dot(norm, lightVec)); //diffuse
		color += lightColor[k] * ks * pow(saturate(dot(norm, halfVec)), m); //specular
	}
	return saturate(vec4(color, 1.0f));
}

void main() {
    vec2 size = vec2(1.0f/u_TextureSize, 0);
    float s11  = textureToHeight(texture(heightTexture, TexCoord).r);

    float s01 = textureToHeight(textureOffset(heightTexture, TexCoord, off.xy).x);//-1, 0
    float s21 = textureToHeight(textureOffset(heightTexture, TexCoord, off.zy).x);//1, 0
    float s10 = textureToHeight(textureOffset(heightTexture, TexCoord, off.yx).x);//0, -1
    float s12 = textureToHeight(textureOffset(heightTexture, TexCoord, off.yz).x);//0, 1

    vec3 height_du = normalize(vec3(size.xy, s01 - s21));//wzdluz X
    vec3 height_dv = normalize(vec3(size.yx, s10 - s12));//Wzdluz Y
    vec4 bump = vec4( cross(height_du,height_dv), s11);//Wynik ma wyjsc Z do gory

    vec3 pos_du = normalize(dFdx(WorldPos));
    vec3 pos_dv = normalize(dFdy(WorldPos));
    vec3 norm = cross(pos_du, pos_dv);

    //norm = normalMapping(norm, pos_du, bump.xyz);
    norm = (u_MVP.Model * vec4(norm, 0.0f)).xyz;


    //FragColor = vec4(bump.xyz, 1.0f);
    //FragColor = vec4(norm, 1.0f);
    //FragColor = vec4(TexCoord.r, TexCoord.g, 0.0f, 1.0f);

    vec4 viewVec = inverse(u_MVP.View) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    FragColor = phong(WorldPos, norm, viewVec.xyz);
}
