#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 tex_coords;

uniform mat4 model;
uniform mat4 cameraView;
uniform mat4 projection;

void main ()
{
    gl_Position = projection * cameraView * model * vec4 (aPos, 1.0);
    tex_coords  = aTexCoords;
}
