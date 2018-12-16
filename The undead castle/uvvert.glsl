#version 430 core

in layout(location=0) vec3 pos;
in layout(location=1) vec2 vuv;
in layout(location=2) vec3 normal;
out vec2 uv;

uniform mat4 MVP;
uniform vec2 offset;
uniform vec2 scale=vec2(1,1);

void main(){
 gl_Position = (MVP * vec4(pos, 1)) * vec4(scale, 1, 1) + vec4(offset, 0, 0);
 uv = vuv;
}