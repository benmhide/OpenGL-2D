#pragma once
#include "Square.h"
#include "Texture.h"

//Background class - inherits from square
class Background : public Square
{
public:
	//Constructor
	Background();

	//Brick member variables for:
	//position, scale and colour
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;

	//Brick texture
	Texture texture;
};

