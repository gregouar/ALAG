#include "ALAGE/utils/Mathematics.h"

float ComputeDotProduct(sf::Vector3f v, sf::Vector3f w)
{
    return v.x*w.x + v.y*w.y + v.z*w.z;
}

float ComputeDotProduct(sf::Vector2f v, sf::Vector2f w)
{
    return v.x*w.x + v.y*w.y;
}

float ComputeSquareDistance(sf::Vector3f v, sf::Vector3f w)
{
    return ComputeDotProduct(v-w,v-w);
}

float ComputeSquareDistance(sf::Vector2f v, sf::Vector2f w)
{
    return ComputeDotProduct(v-w,v-w);
}



void SfColorToGlColor(const sf::Color& sfColor, float glColor[4])
{
    glColor[0] = (float)sfColor.r * 0.00392156862;
    glColor[1] = (float)sfColor.g * 0.00392156862;
    glColor[2] = (float)sfColor.b * 0.00392156862;
    glColor[3] = (float)sfColor.a * 0.00392156862;
}

