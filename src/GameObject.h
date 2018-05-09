#pragma once
#include "Square.h"
#include "Texture.h"

//GameObject class - inherits from square
class GameObject : public Square
{
public:
	//Constructor
	GameObject();

	//Brick member variables for:
	//position, scale, colour and rotation
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 colour;

	//Texture 
	Texture texture;

	//
	bool active;
};