#version 430 core

in vec2 uv;

out vec4 rgba;

uniform sampler2D t;

void main(){
	rgba = texture(t, uv);
}