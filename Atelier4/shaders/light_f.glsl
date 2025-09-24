#version 330 core

in vec3 v_normal;
in vec3 v_worldPos;
out vec4 fragColor;

#define MAX_LIGHTS 2

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light
{
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;

    vec3 dir;
    vec4 pos;

    float linAttenuation;
    float quadAttenuation;
    int type;
};

layout(std140) uniform Lights
{
    vec3 viewDir;
    Light lights[MAX_LIGHTS];
};

vec4 computeLighting(int lightIndex, vec3 normal)
{
    //A remplir
    return vec4(1);
}

void main()
{
    vec3 nNormal = normalize(v_normal);
    vec4 finalColor = vec4(0);
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        finalColor += computeLighting(i, nNormal);
    }
    fragColor = vec4(finalColor.xyz, 1);
}
