#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_ErrorTexture;

void main() {
    FragColor = texture(u_ErrorTexture, TexCoords);
}
