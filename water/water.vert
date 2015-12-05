#version 410

layout(location = 0)in vec3 positions;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec2 textureCoords;
out vec4 clipSpace;

const float tiling = 6.0;

void main(void){

    clipSpace = projectionMatrix * modelViewMatrix * vec4(positions.x, positions.y,0.0,1.0);
    gl_Position = clipSpace;
    textureCoords = vec2(positions.x/2.0 + 0.5, positions.y/2.0 + 0.5);

}