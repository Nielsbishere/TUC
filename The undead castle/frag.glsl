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
 vec4 rgba = texture(t, uv);
 if(rgba.a < 0.5){
	discard;
 }
 vec3 rgb = rgba.rgb;

 //Ambient/diffuse/spectral lighting with multiple lights.
 vec3 computedLight;
 int lightAm = clamp(actualLights, 0, LIGHTS);
 for(int i = 0; i < lightAm; i++){
  vec3 ldir = normalize(lights[i].position - wpos);
  float cosTheta = clamp(dot(ldir, normalize(n)), 0, 1);
  float d = length(lights[i].position - wpos);
  float reflected = pow(clamp(dot(reflect(-ldir, n), normalize(eye - wpos)), 0, 1), 9);
  computedLight = computedLight + cosTheta * lights[i].color * lights[i].power / d / d + reflected * lights[i].color;
 }
 if(actualLights > 0){
  computedLight = computedLight / lightAm;
 }
 vec3 light = computedLight * rgb + ambient * rgb;

 //Fog calculation
 if(isFogEnabled == 0){
  color = vec4(light, 1);
 }else{
   float d = length(cpos);
   float fogFactor = clamp(1.0 / exp((d * fogDensity) * (d * fogDensity)), 0, 1);
   color = vec4(mix(fogColor, light, fogFactor), 1);
 }
}