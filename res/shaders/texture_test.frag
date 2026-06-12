#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 u_BaseColor;
uniform sampler2D u_Texture;
uniform sampler2D u_Texture1;
uniform int u_UseTexture;

void main() {
    if (u_UseTexture == 1) {
        FragColor = texture(u_Texture, TexCoords) * vec4(u_BaseColor, 1.0) + texture(u_Texture1, TexCoords);
    } else {
        FragColor = vec4(u_BaseColor, 1.0);
    }
}
