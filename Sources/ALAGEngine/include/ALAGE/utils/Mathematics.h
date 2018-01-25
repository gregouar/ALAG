#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

const GLfloat IdMat3X3[9] = {1,0,0,0,1,0,0,0,1};

float ComputeDotProduct(sf::Vector3f v, sf::Vector3f w);
float ComputeDotProduct(sf::Vector2f v, sf::Vector2f w);
float ComputeSquareDistance(sf::Vector3f v, sf::Vector3f w);
float ComputeSquareDistance(sf::Vector2f v, sf::Vector2f w);

void SfColorToGlColor(const sf::Color&, float glColor[4]);

#endif // MATHEMATICS_H
