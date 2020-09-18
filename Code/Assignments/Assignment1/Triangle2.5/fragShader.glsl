#version 430
out vec4 color;
uniform float change;
void main(void){
		color = vec4(0.5 + (change*2), 0.5 - (change*2), 1.0, 1.0);
}