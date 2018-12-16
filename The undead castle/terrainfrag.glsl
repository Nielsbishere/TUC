#version 430 core
#define LIGHTS 16

out vec4 color;

in vec2 uv;
in vec3 n;
in vec3 wpos;
in vec3 cpos;

uniform sampler2D t;

uniform vec3 ambient;
uniform vec3 eye;
struct Light {
    vec3 position;
	vec3 color;
	float power;
};
uniform Light lights[LIGHTS];
uniform int actualLights;

uniform vec3 fogColor;
uniform float fogDensity;
uniform int isFogEnabled;

void main(){

 vec3 col;
 if(wpos.y > 30){
	col = vec3(1, 1, 1);
 }else if(wpos.y > 20){
	float snow = (wpos.y - 20) / 10;
	col = (1 - snow) * vec3(.4, .4, .4) + snow * vec3(1, 1, 1);
 }else if(wpos.y > 15){
	float stone = (wpos.y - 15) / 5;
	col = (1 - stone) * vec3(.38, .20, .01) + stone * vec3(.4, .4, .4);
 }else if(wpos.y > 10){
	col = vec3(.38, .20, .01);
 }else if(wpos.y > 0){
	float dirt = 1 - wpos.y / 10;
	col = dirt * vec3(0, .6, .18) + (1 - dirt) * vec3(.38, .20, .01);
 }else{
   col = vec3(0, .6, .18);
 }

 vec3 computedLight;
 int lightAm = min(actualLights, LIGHTS);
 for(int i = 0; i < lightAm; i++){
  vec3 ldir = normalize(lights[i].position - wpos);
  float cosTheta = clamp(dot(ldir, normalize(n)), 0, 1);
  float d = length(lights[i].position - wpos);
//  float reflected = ref * pow(clamp(dot(reflect(-ldir, n), normalize(eye - wpos)), 0, 1), 9);
  computedLight = computedLight + cosTheta * lights[i].color * lights[i].power / d / d;
 }
 if(actualLights > 0){
  computedLight = computedLight / lightAm;
 }
 vec3 light = computedLight * col + ambient * col;

 //Fog calculation
 if(isFogEnabled == 0){
  color = vec4(light, 1);
 }else{
   float d = length(cpos);
   float fogFactor = clamp(1.0 / exp((d * fogDensity) * (d * fogDensity)), 0, 1);
   color = vec4(mix(fogColor, light, fogFactor), 1);
 }
}