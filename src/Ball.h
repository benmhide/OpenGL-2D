#pragma once
#include "Square.h"
#include "Texture.h"

//
class Ball : public Square
{
public:
	//Constructor
	Ball();

	//
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 velocity;
	glm::vec3 colour;
	GLfloat radius;
	Texture texture;
};

