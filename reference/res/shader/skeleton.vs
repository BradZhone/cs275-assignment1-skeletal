// This shader works for rendering the skeleton with simple lines
// For an animated mesh renderer (wireframe is acceptable), 
// you may require some other shader

#version 430 core

//You may need some other layouts for rendering an animated mesh
layout (location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(pos,1.0);
}