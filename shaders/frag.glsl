#version 440

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_sampler;

void main(){

	// Output color = color of the texture at the specified UV
	color = texture( texture_sampler, UV ).rgba ;
	//color = vec3(0.5, 1, 0);
}