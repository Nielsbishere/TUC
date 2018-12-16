#version 430 core

layout(location=0) in vec2 vpos;
layout(location=1) in vec2 vuv;
out vec2 uv;

uniform vec2 uvScale;
uniform vec2 uvOffset;

uniform mat4 model;

void main(){
	uv = vuv * uvScale + uvOffset;
	gl_Position = model * vec4(vpos, 0, 1);
}