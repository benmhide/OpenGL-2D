#version 330

in vec2 fragmentPosition;
in vec4 fragmentColour;
in vec2 fragmentUV;

out vec4 colour;

uniform sampler2D sampler;

void main()
{
	//colour = fragmentColour + vec4(1.0 * (cos(time) + 1.0) * 0.5,
	//							   1.0 * (cos(time + 2.0) + 1.0) * 0.5,
	//							   1.0 * (cos(time + 1.0) + 1.0) * 0.5,
	//							   0.0);
	
	vec4 textureColour = texture(sampler, fragmentUV);
	
	colour = fragmentColour * textureColour;
	
	//colour = vec4(fragmentColour.r * (cos(fragmentPosition.x * 4.0 + time) + 1.0) * 0.5,
	//			  fragmentColour.g * (cos(fragmentPosition.y * 8.0 + time) + 1.0) * 0.5,
	//			  fragmentColour.b * (cos(fragmentPosition.x * 2.0 + time) + 1.0) * 0.5,
	//			  fragmentColour.a) * textureColour;
}