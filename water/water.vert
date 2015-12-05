#version 410

layout(location = 0)in vec3 positions;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec4 clipSpace;

void main(void){

    clipSpace = projectionMatrix * modelViewMatrix * vec4(positions,1.0);
    gl_Position = clipSpace;

}