#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D earthTexture;

void main(){
    FragColor = texture(earthTexture, TexCoord);
}