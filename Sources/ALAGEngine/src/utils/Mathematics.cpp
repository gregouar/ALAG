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


sf::Vector2f Normalize(sf::Vector2f v)
{
    float norm = sqrt(ComputeDotProduct(v,v));
    if(norm != 0)
        return v/norm;
    return v;
}


sf::Vector3f Normalize(sf::Vector3f v)
{
    float norm = sqrt(ComputeDotProduct(v,v));
    if(norm != 0)
        return v/norm;
    return v;
}


void SfColorToGlColor(const sf::Color& sfColor, float glColor[4])
{
    glColor[0] = (float)sfColor.r * 0.00392156862;
    glColor[1] = (float)sfColor.g * 0.00392156862;
    glColor[2] = (float)sfColor.b * 0.00392156862;
    glColor[3] = (float)sfColor.a * 0.00392156862;
}

namespace alag {

Mat3x3::Mat3x3() : Mat3x3(0,0,0,0,0,0,0,0,0)
{

}

Mat3x3::Mat3x3(sf::Vector3f u, sf::Vector3f v, sf::Vector3f w) : Mat3x3(u.x, u.y, u.z,
                                                                        v.x, v.y, v.z,
                                                                        w.x, w.y, w.z)
{

}

Mat3x3::Mat3x3( float a, float b, float c,
        float d, float e, float f,
        float g, float h, float i)
{
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
    values[4] = e;
    values[5] = f;
    values[6] = g;
    values[7] = h;
    values[8] = i;
}

Mat3x3::~Mat3x3()
{

}

Mat3x3 Mat3x3::operator+(Mat3x3 const& m)
{
    return Mat3x3(this->values[0]+m.values[0], this->values[1]+m.values[1], this->values[2]+m.values[2],
                  this->values[3]+m.values[3], this->values[4]+m.values[4], this->values[5]+m.values[5],
                  this->values[6]+m.values[6], this->values[7]+m.values[7], this->values[8]+m.values[8]);
}

Mat3x3 Mat3x3::operator*(Mat3x3 const& m)
{
    sf::Vector3f u(m.values[0], m.values[3], m.values[6]);
    sf::Vector3f v(m.values[1], m.values[4], m.values[7]);
    sf::Vector3f w(m.values[2], m.values[5], m.values[8]);

    u = (*this)*u;
    v = (*this)*v;
    w = (*this)*w;

    return Mat3x3(u.x,v.x,w.x,
                  u.y,v.y,w.y,
                  u.z,v.z,w.z);

}


Mat3x3 Mat3x3::operator*(float const& k)
{
    return Mat3x3(this->values[0]*k,this->values[1]*k,this->values[2]*k,
                  this->values[3]*k,this->values[4]*k,this->values[5]*k,
                  this->values[6]*k,this->values[7]*k,this->values[8]*k);

}

sf::Vector3f Mat3x3::operator*(sf::Vector3f const& v)
{
    sf::Vector3f r;
    r.x = this->values[0]*v.x + this->values[1]*v.y + this->values[2]*v.z;
    r.y = this->values[3]*v.x + this->values[4]*v.y + this->values[5]*v.z;
    r.z = this->values[6]*v.x + this->values[7]*v.y + this->values[8]*v.z;
    return r;
}

sf::Vector2f Mat3x3::operator*(sf::Vector2f const& v)
{
    sf::Vector2f r;
    r.x = this->values[0]*v.x + this->values[1]*v.y;
    r.y = this->values[3]*v.x + this->values[4]*v.y;
    return r;
}


}
