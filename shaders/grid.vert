#version 410

uniform mat4 view, proj;
layout(location = 1) in vec3 vertex_colour;

in vec3 vertex_points;
out vec3 colour;

void main () {

    colour = vertex_colour;
    gl_Position = proj * view * vec4 (vertex_points, 1.0);
}