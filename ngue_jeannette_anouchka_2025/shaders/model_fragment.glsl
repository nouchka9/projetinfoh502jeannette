#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec4 tintColor;
uniform float transparency;
uniform vec3 lightDir;
uniform vec3 lightDirColor;
uniform vec3 lightPointPos;
uniform vec3 lightPointColor;
uniform float lightPointConstant;
uniform float lightPointLinear;
uniform float lightPointQuadratic;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(-lightDir))), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if (projCoords.z > 1.0) shadow = 0.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirNorm = normalize(-lightDir);
    float diff = max(dot(norm, lightDirNorm), 0.0);
    vec3 diffuse = diff * lightDirColor;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightDirColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirNorm, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightDirColor;

    vec3 lightDirPoint = normalize(lightPointPos - FragPos);
    float diffPoint = max(dot(norm, lightDirPoint), 0.0);
    float distance = length(lightPointPos - FragPos);
    float attenuation = 1.0 / (lightPointConstant + lightPointLinear * distance + lightPointQuadratic * (distance * distance));
    vec3 diffusePoint = diffPoint * lightPointColor * attenuation;
    vec3 specularPoint = specularStrength * spec * lightPointColor * attenuation;

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular + diffusePoint + specularPoint));
    vec3 result = lighting * vec3(texture(texture1, TexCoord));
    FragColor = vec4(result, transparency) * tintColor;

    // Force white tint if red persists
    if (tintColor.r > 0.9 && tintColor.g < 0.1 && tintColor.b < 0.1) {
        FragColor *= vec4(1.0, 1.0, 1.0, 1.0);
    }
}