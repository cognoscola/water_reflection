#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 textureCoords;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec2 pass_textureCoords;

void main(void){

    gl_Position = projectionMatrix * modelViewMatrix * vec4(positions,1.0);
    pass_textureCoords = textureCoords;
}