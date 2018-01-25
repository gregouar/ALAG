#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <SFML/System.hpp>

float ComputeDotProduct(sf::Vector3f v, sf::Vector3f w);
float ComputeDotProduct(sf::Vector2f v, sf::Vector2f w);
float ComputeSquareDistance(sf::Vector3f v, sf::Vector3f w);
float ComputeSquareDistance(sf::Vector2f v, sf::Vector2f w);


#endif // MATHEMATICS_H
