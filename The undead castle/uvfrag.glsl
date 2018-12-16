#version 430 core

out vec4 color;
in vec2 uv;

uniform sampler2D t;

void main(){
 vec4 rgba = texture(t, uv);
 if(rgba.a < 0.5){
	discard;
 }
 vec3 rgb = rgba.rgb;
 color = vec4(rgb, 1);
}