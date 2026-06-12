#version 460 core
out vec4 FragColor;

uniform vec3 u_BaseColor;

void main() {
    FragColor = vec4(u_BaseColor, 1.0);
}
