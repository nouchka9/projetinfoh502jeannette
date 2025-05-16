#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightDir;
uniform vec3 lightDirColor;
uniform vec3 lightPointPos;
uniform vec3 lightPointColor;
uniform float lightPointConstant;
uniform float lightPointLinear;
uniform float lightPointQuadratic;
uniform vec3 viewPos;
uniform float alpha;
uniform vec4 tintColor;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = 0.0;
    float bias = 0.005;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main() {
    vec4 color = texture(texture_diffuse1, TexCoords);
    if (color.a < 0.1) {
        color = vec4(1.0, 0.5, 0.5, 1.0);
    }

    vec3 normal = normalize(Normal);

    vec3 ambient = 0.15 * color.rgb;
    vec3 diffuseDir = vec3(0.0);
    vec3 specularDir = vec3(0.0);
    float shadow = 0.0; // Shadows disabled
    {
        vec3 lightDirNorm = normalize(-lightDir);
        float diff = max(dot(lightDirNorm, normal), 0.0);
        diffuseDir = diff * lightDirColor;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDirNorm, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        specularDir = 0.5 * spec * lightDirColor;
    }

    vec3 diffusePoint = vec3(0.0);
    vec3 specularPoint = vec3(0.0);
    {
        vec3 lightDirPoint = normalize(lightPointPos - FragPos);
        float diff = max(dot(lightDirPoint, normal), 0.0);
        diffusePoint = diff * lightPointColor;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDirPoint, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        specularPoint = 0.5 * spec * lightPointColor;

        float distance = length(lightPointPos - FragPos);
        float attenuation = 1.0 / (lightPointConstant + lightPointLinear * distance + lightPointQuadratic * (distance * distance));
        diffusePoint *= attenuation;
        specularPoint *= attenuation;
    }

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuseDir + specularDir) + (diffusePoint + specularPoint)) * color.rgb;
    FragColor = vec4(lighting * tintColor.rgb, alpha * color.a * tintColor.a);
}