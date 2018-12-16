#version 430 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 vuv;

out vec2 uv;

uniform vec2 pos;
uniform vec2 scale;

void main(){
	gl_Position = vec4(position, 0, 1) * vec4(scale, 1, 1) + vec4(pos, 0, 0);
	uv = vuv;
}