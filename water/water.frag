#version 410

in vec2 pass_textureCoords;
out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main () {
    vec4 reflectColour = texture(reflectionTexture,pass_textureCoords);
    vec4 refractColour = texture(refractionTexture,pass_textureCoords);

    out_Color = mix(reflectColour, refractColour,0.5);
}
