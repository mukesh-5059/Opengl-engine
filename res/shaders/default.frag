#version 460 core
out vec4 FragColor;

in vec3 Normal;

const vec3 lightDir = normalize(vec3(-0.2f, -1.0f, -0.3f));
const vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
const vec3 diffuseColor = vec3(0.8f, 0.8f, 0.8f);
const vec3 materialColor = vec3(0.5f, 0.5f, 0.5f); // Matte grey

void main() {
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diffuseColor * diff * materialColor;
    vec3 ambient = ambientColor * materialColor;

    FragColor = vec4(ambient + diffuse, 1.0);
}