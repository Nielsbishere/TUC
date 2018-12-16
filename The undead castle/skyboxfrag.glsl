#version 430 core

in vec3 coord;
out vec4 color;

uniform samplerCube skybox;
uniform vec3 fogColor;

void main(){
 float LOWER = 0;
 float UPPER = 0.1;
 vec4 final = texture(skybox, coord);
 float fog = clamp((coord.y - LOWER) / (UPPER - LOWER), 0, 1);
 color = mix(vec4(fogColor,1), final, fog);
}