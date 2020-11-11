#version 430

layout (location = 0) in vec3 vertPos;
uniform mat4 shadowMVP1;

void main(void){
	gl_Position = shadowMVP1 * vec4(vertPos, 1.0);
}