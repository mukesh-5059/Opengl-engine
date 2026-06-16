#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material Uniforms (Conventions)
uniform vec3 u_BaseColor;
uniform int u_UseDiffuseMap;
uniform sampler2D u_DiffuseMap;

uniform int u_UseSpecularMap;
uniform sampler2D u_SpecularMap;

uniform int u_UseEmissiveMap;
uniform sampler2D u_EmissiveMap;

uniform vec3 u_ViewPos;

void main() {
    // 1. Ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    // 2. Diffuse (Hardcoded light direction from top-right-front)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 diffuseColor;
    if (u_UseDiffuseMap == 1) {
        diffuseColor = texture(u_DiffuseMap, TexCoords).rgb * u_BaseColor;
    } else {
        diffuseColor = u_BaseColor;
    }
    vec3 diffuse = diff * diffuseColor;
    
    // 3. Specular (Blinn-Phong)
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0); // Shininess = 32
    
    vec3 specularColor = vec3(0.5);
    if (u_UseSpecularMap == 1) {
        specularColor = texture(u_SpecularMap, TexCoords).rgb;
    }
    vec3 specular = spec * specularColor;
    
    // 4. Emissive
    vec3 emissive = vec3(0.0);
    if (u_UseEmissiveMap == 1) {
        emissive = texture(u_EmissiveMap, TexCoords).rgb;
    }
    
    // Combine
    vec3 result = (ambient + diffuse + specular) + emissive;
    FragColor = vec4(result, 1.0);
}
