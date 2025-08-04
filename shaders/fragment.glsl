#version 330 core
out vec4 FragColor;

in vec2 tex_coords;
uniform sampler2D uTexture;

void main ()
{
    FragColor = texture (uTexture, tex_coords);
}
