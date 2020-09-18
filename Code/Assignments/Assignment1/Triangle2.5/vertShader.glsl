#version 430 
uniform float offset;

// 4x4 matrix that returns a rotation around the z axis by amount
// float rad
mat4 buildRotateZ(float rad){
	mat4 zRotation = mat4(cos(rad), -sin(rad), 0.0, 0.0,
						  sin(rad), cos(rad), 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0		  );
	
	return zRotation;
}


void main(void){

	//hard coded rotation... by 1/64 of Pi degrees
	mat4 newRotate = buildRotateZ(3.1415926/(64*offset));

	//Watching the way this moves and the way I had to program offset makes me quickly
	// realize why a physics engine can be important for realistic looking movements
	
	if(gl_VertexID == 0)
		gl_Position = vec4(0.25 + offset, -0.25, 0.0, 1.0) * newRotate;
	else if(gl_VertexID == 1)
		gl_Position = vec4(-0.25 - offset, -0.25, 0.0, 1.0) * newRotate;
	else
		gl_Position = vec4(0, 0.25 + offset, 0.0, 1.0) * newRotate;
}