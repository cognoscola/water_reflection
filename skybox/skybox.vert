#version 410

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

layout(location = 0)in vec3 positions;
out vec3 textureCoords;

//uniform vec4 u_plane0 = vec4(0,-1,0,0);

void main () {

   vec4 vsPos =  vec4 (positions, 1.0);
   gl_Position = projectionMatrix * viewMatrix * vsPos;
   textureCoords= positions;

//   gl_ClipDistance[0] = dot(u_plane0,vsPos);

}