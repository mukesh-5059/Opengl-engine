#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;

uniform vec3 u_ViewPos;

// Directional Light properties
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const DirLight dirLight = DirLight(
    vec3(-0.2f, -1.0f, -0.3f), // pointing slightly down, back, and left
    vec3(0.15f, 0.15f, 0.15f),  // ambient
    vec3(0.8f, 0.8f, 0.8f),    // diffuse
    vec3(0.5f, 0.5f, 0.5f)     // specular
);

void main() {
    // Normal and view vectors
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    
    // Resolve base diffuse color
    vec3 baseColor = vec3(texture(u_DiffuseMap, TexCoords));
    
    // Ambient
    vec3 ambient = dirLight.ambient * baseColor;
    
    // Diffuse 
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * baseColor;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    
    vec3 specularColor = vec3(texture(u_SpecularMap, TexCoords));
    vec3 specular = dirLight.specular * spec * specularColor;
    
    // Final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
