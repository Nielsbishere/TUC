#version 430 core

in vec2 uv;

uniform sampler2D img;
uniform vec3 col;

out vec4 color;

uniform vec3 border = vec3(1, 0, 0);
uniform vec3 vals = vec3(.5, .6, .65);

void main(){
	float dist = 1 - texture(img, uv).a;
	if(dist < vals.x){
		color = vec4(col, 1);
	}else if(dist < vals.y){
		float d = (dist - vals.x) / (vals.y - vals.x);
		color = vec4((1 - d) * col + d * border, 1);
	}else if(dist < vals.z){
		float d = (dist - vals.y) / (vals.z - vals.y);
		color = vec4(border, 1 - d);
	}
}