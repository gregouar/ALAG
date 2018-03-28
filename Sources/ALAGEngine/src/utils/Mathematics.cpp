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

int RandomNumber(int maxNbr, int seed)
{
    return (int)(static_cast <float> (rand()+seed) /( static_cast <float> (RAND_MAX/maxNbr)));
}

float LinInterp(sf::Vector2f left, float p, sf::Vector2f right)
{
    float dX = right.x - left.x;
    float dY = right.y - left.y;
    float x = p-left.x;

    return left.y + dY*x/dX;
}

void GenerateGerstnerWave(sf::Glsl::Vec4 *tab, float *xDistribution, size_t tab_size, float steepness, float wave_length)
{
    float step = (1.0f/(tab_size-1));
    float angle = 0;

    float curMax = 0;

    std::map<float, sf::Glsl::Vec4> newTab;
    std::map<float, sf::Glsl::Vec4>::iterator it;

    if(wave_length * steepness > 2.9)
        steepness = 2.9/wave_length;

    for(size_t i = 0 ; i < tab_size ; ++i)
    {
        sf::Vector2f position( i*step + sin(angle) * wave_length * steepness * step,
                              -cos(angle));
        sf::Vector2f velocity(-sin(angle*0.5),
                               cos(angle*0.5) * sin(angle*0.5));

        float foam = 0;

        if(velocity.y > 0) velocity.y  = -velocity.y ;

        if(position.x < curMax)
        {
            float xLeft = 0;
            sf::Glsl::Vec4 left(0,0,0,0);

            std::map<float, sf::Glsl::Vec4>::iterator it = newTab.begin();

           while(it != newTab.end())
            {
                if(it->first > position.x)
                {
                    float xRight = it->first;
                    sf::Glsl::Vec4 right = it->second;

                    float factor=(right.x - left.x)/(xRight - xLeft);

                    float newPosY = left.x + (position.x-xLeft)*factor;

                    //foam = position.y - newPosY;

                    position.y = newPosY;
                    velocity.x = (velocity.x-left.y)*factor;
                    velocity.y = (velocity.y-left.z)*factor;

                   // newTab[xLeft].w = foam;
                   // newTab[xRight].w = foam;

                    it = newTab.end();
                } else {
                    xLeft =it->first;
                    left = it->second;
                    ++it;
                }
            }

        }

        newTab[position.x] = sf::Glsl::Vec4(position.y, velocity.x, velocity.y, foam*255);

        angle += step * 2 * PI;
        if(position.x > curMax)
            curMax = position.x;
    }


    /*{
    sf::RenderTexture r;
    r.create(512,512);
    r.clear(sf::Color::White);

    size_t i = 0;
    std::map<float, sf::Glsl::Vec4>::iterator it;
    for(it = newTab.begin() ; it != newTab.end() ; ++i)
    {
        float xL = it->first;
        sf::Glsl::Vec4 pL = it->second;
        ++it;

        if(it != newTab.end())
        {
            float xR = it->first;
            sf::Glsl::Vec4 pR = it->second;

            tab[i] = it->second;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f( xL*512.0, 256+pL.x*256.0),sf::Color::Black),
                sf::Vertex(sf::Vector2f( xR*512.0f, 256+pR.x*256.0),sf::Color::Black)
            };

            r.draw(line, 2, sf::Lines);
        }
    }
    //r.display();
    r.getTexture().copyToImage().saveToFile("wave0.png");
    }*/


    /*std::map<float, sf::Glsl::Vec4> newTab2;
    newTab2[0] = newTab.begin()->second;
    size_t i = 1;*/
    /**for(it = newTab.begin() ; it != newTab.end() ; ++i)
    {
        float xL = it->first;
        sf::Glsl::Vec4 pL = it->second;
        ++it;

        if(it != newTab.end())
        {
            float xR = it->first;
            sf::Glsl::Vec4 pR = it->second;

            float steepFactor = (pR.x - pL.x)/(xR - xL) - 7;
            sf::Vector2f newPos(xR, pR.x);
            if(steepFactor > 0.0)
            {
                float dX=atan(steepFactor)/(2*PI)*.2;
               newPos.x -= dX;
               pR.w = dX;
            }

            newTab2[newPos.x] = sf::Glsl::Vec4(newPos.y, pR.y, pR.z, pR.w);
        }
    }**/
    //newTab2[newTab.end()->first] = newTab.end()->second;

    {
    size_t i = 0;
    for(it = newTab.begin() ; it != newTab.end() ; ++it, ++i)
    {
        xDistribution[i] = it->first;
        tab[i] = it->second;
    }
    }

    /*for(size_t i = 1 ; i < tab_size ; ++i)
    {
        float steepFactor = (tab[i].x - tab[i-1].x)/(xDistribution[i] - xDistribution[i-1]);
        if(tab[i-1].x - tab[i].x < 0 && steepFactor > 7.0)
        {
            tab[i].w = steepFactor/14;
            if(tab[i].w > 1.0)
                tab[i].w = 1.0;
        }
    }*/

   // float collapsingFactor = (wave_length * steepness -1)*1.25;
    float collapsingFactor = (wave_length * steepness -1.5)*2;

   // collapsingFactor=0;

    if(collapsingFactor > 0)
    {
        float t = collapsingFactor/2.8;
        collapsingFactor = 2.8*(t*t);

        /** Parabolic test
        std::cout<<"Before:"<<std::endl;
        for(size_t i = 0 ; i < tab_size ; ++i)
            std::cout<<xDistribution[i]<<" "<<tab[i].x<<std::endl;

        for(size_t i = 0 ; i < tab_size ; ++i)
        {
            float dX = 0.5 - xDistribution[i];

            float y = (tab[i].x+1)/2;
            float dPy = 2*collapsingFactor*y;

            sf::Vector2f n = dX*Normalize(sf::Vector2f(-dPy,1.0));

           // float t = 1/(2*collapsingFactor) * (pow((1+2*collapsingFactor*y),2./3.) - 1);
           float t = 1/(2*collapsingFactor)() ///Cant find t for fixed length

            xDistribution[i] = (0.5 -collapsingFactor * t * t ) - n.y;
            tab[i].x = (t*2-1) + n.x*2 ;

        }

        std::cout<<"After:"<<std::endl;
        for(size_t i = 0 ; i < tab_size ; ++i)
            std::cout<<xDistribution[i]<<" "<<tab[i].x<<std::endl;**/

        /** Circle test **/
        for(size_t i = 0 ; i < tab_size ; ++i)
        {
            double dX = 0.5 - xDistribution[i];

            double y = (tab[i].x+1)/2;

            double t = y*collapsingFactor;

            sf::Vector2f n = sf::Vector2f(-dX*cos(t),-dX*sin(t));

            xDistribution[i] = 0.5+1.0/collapsingFactor*(0.5*cos(t)-.5) + n.x;
            tab[i].x = (1.0/collapsingFactor*sin(t) + n.y)*2-1 ;

        }
    }
    /*{
    sf::RenderTexture r;
    r.create(512,512);
    r.clear(sf::Color::White);

    for(size_t d = 0 ; d < 1 ; ++d)
    for(size_t i = 0 ; i < tab_size -1 ; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f( xDistribution[i] *512.0, 256+tab[i].x*256.0-d),sf::Color(tab[i].w*255,0,0)),
            sf::Vertex(sf::Vector2f( xDistribution[i+1] *512.0f, 256+tab[i+1].x*256.0-d),sf::Color(tab[i+1].w*255,0,0))
        };

        r.draw(line, 2, sf::Lines);
    }
    //r.display();
    r.getTexture().copyToImage().saveToFile("wave1.png");
    }*/



    float curMaxRight = 0;
    float overlappingRight = 0;
    float overlappingLeft = 1.0;

    for(size_t i = 0 ; i < tab_size ; ++i)
    {
        float t = xDistribution[i];
        if(t >= curMaxRight)
            curMaxRight = t;
        else
        {
            if(curMaxRight > overlappingRight)
                overlappingRight = curMaxRight;
            if(t < overlappingLeft)
                overlappingLeft = t;
        }
    }


    /*{
    sf::RenderTexture r;
    r.create(512,512);
    r.clear(sf::Color::White);

    for(size_t d = 0 ; d < 1 ; ++d)
    for(size_t i = 0 ; i < tab_size -1 ; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f( xDistribution[i] *512.0, 256+tab[i].x*256.0-d),sf::Color(tab[i].w*255,0,0)),
            sf::Vertex(sf::Vector2f( xDistribution[i+1] *512.0f, 256+tab[i+1].x*256.0-d),sf::Color(tab[i+1].w*255,0,0))
        };

        r.draw(line, 2, sf::Lines);
    }
    //r.display();
    r.getTexture().copyToImage().saveToFile("wave2.png");
    }*/


    std::map<float, sf::Glsl::Vec4> newTab2;
    size_t nbr_collapsing = 0;
    size_t ileft = 0;
    for(size_t i = 0 ; i < tab_size ; ++i)
    {
        float t = xDistribution[i];
        if(t >= overlappingLeft && t <= overlappingRight)
        {
            nbr_collapsing++;
        } else
        {
            if(t < overlappingLeft  && i > ileft)
                ileft = i;
            newTab2[xDistribution[i]] = tab[i];
        }
    }


    sf::Glsl::Vec4 pL = tab[ileft], pR = tab[ileft + nbr_collapsing];

    if(overlappingRight != 0)
    {
        double f = (double)(overlappingLeft - xDistribution[ileft])
                    /(double)(xDistribution[ileft+1] - xDistribution[ileft]);

        pL.x += f*(double)(tab[ileft+1].x - tab[ileft].x);
        pL.y += f*(double)(tab[ileft+1].y - tab[ileft].y);
        pL.z += f*(double)(tab[ileft+1].z - tab[ileft].z);
        pL.w = 0.0;
        newTab2[overlappingLeft] = pL;
    }
    if(overlappingRight != 0)
    {
        double f = (double)(overlappingRight - xDistribution[ileft+ nbr_collapsing] )
                    /(double)(xDistribution[ileft+nbr_collapsing+1] - xDistribution[ileft+nbr_collapsing]);

        pR.x += f*(double)(tab[ileft+ nbr_collapsing+1].x - tab[ileft+ nbr_collapsing].x);
        pR.y += f*(double)(tab[ileft+ nbr_collapsing+1].y - tab[ileft+ nbr_collapsing].y);
        pR.z += f*(double)(tab[ileft+ nbr_collapsing+1].z - tab[ileft+ nbr_collapsing].z);
        pR.w = 0.0;
        newTab2[overlappingRight] = pR;
    }

    for(size_t i = 1 ; (int)i < (int)nbr_collapsing-1 ; ++i)
    {
        float dX = (float)i/(float)nbr_collapsing*(overlappingRight-overlappingLeft);
        float a = (pR.x - pL.x)/(overlappingRight-overlappingLeft);

       // float foam = ((float)nbr_collapsing - Abs((float)(0.5*nbr_collapsing-i)))/(float)nbr_collapsing;
       // float foam = ((float)nbr_collapsing - (float)i)/(float)nbr_collapsing;
       float foam = 1.0;
       if(i > (float)nbr_collapsing/2) foam -= (float)(i - (float)nbr_collapsing/2)/(float)nbr_collapsing*2;

        newTab2[overlappingLeft+dX]
            = sf::Glsl::Vec4(pL.x + a*dX,-1,-1,foam);
    }

    {
    size_t i = 0;
    for(it = newTab2.begin() ; it != newTab2.end() &&  i < tab_size ; ++it, ++i)
    {
        xDistribution[i] = it->first;
        tab[i] = it->second;
    }
    }


    /*{
    sf::RenderTexture r;
    r.create(512,512);
    r.clear(sf::Color::White);

    for(size_t d = 0 ; d < 1 ; ++d)
    for(size_t i = 0 ; i < tab_size -1 ; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f( xDistribution[i] *512.0, 256+tab[i].x*256.0-d),sf::Color(tab[i].w*255,0,0)),
            sf::Vertex(sf::Vector2f( xDistribution[i+1] *512.0f, 256+tab[i+1].x*256.0-d),sf::Color(tab[i+1].w*255,0,0))
        };

        r.draw(line, 2, sf::Lines);
    }
    //r.display();
    r.getTexture().copyToImage().saveToFile("wave3.png");
    }*/
}

void GenerateGerstnerWave(sf::Glsl::Vec4 *tab, float *xDistribution, size_t tab_size,
                          float left_steepness, float right_steepness, float nbr_waves, float t)
{
    float size_factor = 1.25;
    float step_size = size_factor/tab_size;
    nbr_waves = size_factor*nbr_waves;

    float tp = floor((t+PI)/(2*PI))*2*PI;
    float height_factor= 1.0;

    t = t - floor((t)/(2*PI))*2*PI;


    for(size_t i = 0 ; i < tab_size ; ++i)
    {
       // float r =  (float)i/(float)tab_size;
        float r =  (float)i/(float)tab_size;
        r=1;
        float local_steepness = (left_steepness * (float)i*size_factor + right_steepness * (tab_size-((float)i*size_factor)))/(float)tab_size/nbr_waves;
        if((float)i*size_factor > (float)tab_size )
            local_steepness = left_steepness/nbr_waves;

        if(local_steepness > 8./nbr_waves)
        {
            height_factor *= 1./(local_steepness*nbr_waves - 7);
            local_steepness = 8./nbr_waves;
        }

        float rolling = -nbr_waves*2*PI*(float)i/(float)tab_size;

       // if(i == tab_size-1)
         //   rolling =  -nbr_waves*2*PI + PI - t;

//        sf::Vector2f ellipse(r*sin(t+rolling)*local_steepness,
  //                         r*cos(t+rolling) /* *local_steepness/5.0 */);
       // float collapse_angle = local_steepness*2;

       float collapsing_factor = (local_steepness-1.0)*2;
       if(collapsing_factor< 0) collapsing_factor = 0;

        sf::Vector2f shift(r*sin(t+rolling)*(local_steepness),
                           r*cos(t+rolling));

        //shift.x -= 10*collapsing_factor*(0.5-shift.y*0.5);
        //shift.y -= collapsing_factor*(0.5+shift.y*0.5);

       // shift.x = ellipse.x * sin(collapse_angle) - ellipse.y * cos(collapse_angle);
       // shift.y = ellipse.x * cos(collapse_angle) + ellipse.y * sin(collapse_angle);

       sf::Vector2f pos(-rolling/(2*PI*nbr_waves)  + shift.x * step_size,
                        shift.y);

        /*sf::Vector2f velocity(-sin(t+rolling*0.5),
                               cos(t+rolling*0.5) * sin(t+rolling*0.5));

        if(velocity.y > 0) velocity.y  = -velocity.y ;
        if(velocity.x > 0) velocity.x  = -velocity.x ;*/


        sf::Vector2f velocity(-1,-1);

       // sf::Vector2f velocity(-cos(t+rolling),-1);

       if(collapsing_factor > 0)
       {
            /*sf::Vector2f c_center(1,-1);
            float dist = Abs(c_center.x - pos.x);
            float t = (0.5+pos.y*0.5)/dist*0.5;
            pos = c_center + dist*sf::Vector2f(-cos(t), sin(t));
            pos.y = pos.y*2-1;*/

            /*double dX = 0.5 - xDistribution[i];
            double y = (tab[i].x+1)/2;
            double t = y*collapsingFactor;
            sf::Vector2f n = sf::Vector2f(-dX*cos(t),-dX*sin(t));
            xDistribution[i] = 0.5+1.0/collapsingFactor*(0.5*cos(t)-.5) + n.x;
            tab[i].x = (1.0/collapsingFactor*sin(t) + n.y)*2-1;*/

            collapsing_factor = - collapsing_factor;

           // float startpoint = (t)/(2*PI*nbr_waves);
            float startpoint = (t)/(2*PI*nbr_waves);
            //float r = (-rolling)/(2*PI*nbr_waves) - startpoint + shift.x/nbr_waves;
            float r = pos.x - startpoint;
            float relpos = r*nbr_waves - 0.5 - floor(r * nbr_waves -0.5);
            float dX = relpos-0.5;

            double y = (pos.y+1)/2.0;
            double T = y * collapsing_factor;
            sf::Vector2f n(dX*cos(T), dX*sin(T));
            pos.x = startpoint + (floor(r * nbr_waves - 0.5) + 1.0 + 1.0/collapsing_factor*(0.5*cos(T)-.5) + n.x)/nbr_waves;
            pos.y = (1.0/collapsing_factor*sin(T) + n.y)*2-1;

           // pos.x = startpoint + (relpos+.5+floor(r * nbr_waves -0.5))/nbr_waves;

           //std::cout<<dX<<std::endl;
            //tab[i].w = Abs(dX)*2;
       }


        //xDistribution[i] = step_size*i + shift.x * step_size  /*- 0.125*/;
       // xDistribution[i] = -rolling/(2*PI*nbr_waves)  + shift.x * step_size  /*- 0.125*/;
        //tab[i].x = shift.y;

        xDistribution[i] = pos.x;
        pos.y = 0.5+pos.y*0.5;
        pos.y *= height_factor;
        pos.y *= height_factor;
        pos.y = pos.y*2-1;
        tab[i].x = pos.y;

        tab[i].y = velocity.x;
        tab[i].z = velocity.y;
        tab[i].w = 0;
    }




    bool under = false;
    float maxX = -1, minX= 0;

    for(size_t i = 0 ; i < tab_size ; ++i)
    {
        if(xDistribution[i] >= maxX)
        {
            if(under)
            {
                size_t nbr_under = 1;
                while((int)i-(int)nbr_under-1 > 0 && xDistribution[i-nbr_under-1] >= minX )
                {
                    nbr_under++;
                }

                sf::Vector2f leftP(minX, LinInterp(sf::Vector2f(xDistribution[i-nbr_under-1], tab[i-nbr_under-1].x),
                                                   minX,
                                                   sf::Vector2f(xDistribution[i-nbr_under], tab[i-nbr_under].x)));
                sf::Vector2f rightP(maxX, LinInterp(sf::Vector2f(xDistribution[i-1],tab[i-1].x),
                                                     maxX,
                                                     sf::Vector2f(xDistribution[i],tab[i].x)));

                for(size_t j = 0 ; j < nbr_under ;++j)
                {
                    int t = i - nbr_under + j;
                    xDistribution[t] = LinInterp(sf::Vector2f(0,minX),(float)j,sf::Vector2f(nbr_under-1,maxX));
                    tab[t].x = LinInterp(leftP,xDistribution[t],rightP);

                    float d = (float)j/(float)(nbr_under-1);
                   /* if(d > .75)
                        tab[t].w = 1.0 - (d-.75)*4;
                    else  if(d > 0.5 )
                        tab[t].w = 1.0;
                    else
                        tab[t].w = 2*d;*/

                    if(d < .25)
                        tab[t].w = 4*d;
                    else if(d < .75)
                        tab[t].w = 1.0;
                    else
                        tab[t].w = 1.0 - (d-.75)*4;


                   // tab[t].w  = 1.0- Abs(0.5-(float)j/(float)nbr_under)*2.0;
                    //tab[t].w  =  1.0;
                }
                tab[i].w = 0.0;
            }

            maxX = xDistribution[i];
            minX = maxX;
            under = false;
        }
        else
        {
            if(xDistribution[i] < minX)
                minX = xDistribution[i];

            under = true;
        }
    }

    if(under)
    {
        size_t i = tab_size-1;

        size_t nbr_under = 1;
        while((int)i-(int)nbr_under-1 > 0 && xDistribution[i-nbr_under-1] >= minX )
        {
            nbr_under++;
        }

        sf::Vector2f leftP(minX, LinInterp(sf::Vector2f(xDistribution[i-nbr_under-1], tab[i-nbr_under-1].x),
                                           minX,
                                           sf::Vector2f(xDistribution[i-nbr_under], tab[i-nbr_under].x)));
        sf::Vector2f rightP(maxX,-1.0);

        for(size_t j = 0 ; j <= nbr_under ;++j)
        {
            int t = i - nbr_under + j;
            xDistribution[t] = LinInterp(sf::Vector2f(0,minX),(float)j,sf::Vector2f(nbr_under,maxX));
            tab[t].x = LinInterp(leftP,xDistribution[t],rightP);
           /* if((float)j > (float)nbr_under/2.0)
                tab[t].w = 1.0;
            else
                tab[t].w = 2*(float)j/(float)nbr_under;*/
           // tab[t].w  = 1.0- Abs(0.5-(float)j/(float)nbr_under)*2.0;
            //tab[t].w  =  1.0;


                float d = (float)j/(float)(nbr_under-1);

                if(d < .25)
                    tab[t].w = 4*d;
                else if(d < .75)
                    tab[t].w = 1.0;
                else
                    tab[t].w = 1.0 - (d-.75)*4;

        }
        tab[i].w = 0.0;
    }

}


/**void GenerateGerstnerWave(sf::Glsl::Vec4 *tab, size_t tab_size, float steepness, float wave_length)
{
    float step = (1.0f/(tab_size-1));
    float angle = 0;

    std::map<float, sf::Glsl::Vec4> newTab;

    for(size_t i = 0 ; i < tab_size ; ++i)
    {
        sf::Vector2f position( i*step + sin(angle) * wave_length * steepness * step,
                              -cos(angle));
        sf::Vector2f velocity(-sin(angle*0.5),
                               cos(angle*0.5) * sin(angle*0.5));

        if(velocity.y > 0) velocity.y  = -velocity.y ;

        newTab[position.x] =
            sf::Glsl::Vec4(position.x, position.y,
                            velocity.x, velocity.y);

        angle += step * 2 * PI;
    }

    size_t i = 0;
    std::map<float, sf::Glsl::Vec4>::iterator it;
    for(it = newTab.begin() ; it != newTab.end() ; ++it, ++i)
    {
        tab[i] = it->second;
    }


    sf::RenderTexture r;
    r.create(512,512);
    r.clear();

    for(size_t i = 0 ; i < tab_size -1 ; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f( tab[i].x*512.0, 256+tab[i].y*256.0)),
            sf::Vertex(sf::Vector2f( tab[i+1].x*512.0f, 256+tab[i+1].y*256.0))
        };

        r.draw(line, 2, sf::Lines);
    }
    //r.display();
    r.getTexture().copyToImage().saveToFile("wave1.png");
}**/


float Abs(float a)
{
    return (a > 0) ? a : -a;
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

Mat2x2::Mat2x2() : Mat2x2(0,0,0,0)
{

}

Mat2x2::Mat2x2(sf::Vector2f u, sf::Vector2f v) : Mat2x2(u.x, u.y,
                                                        v.x, v.y)
{

}

Mat2x2::Mat2x2( float a, float b,
                float c, float d )
{
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}

Mat2x2::~Mat2x2()
{

}

Mat2x2 Mat2x2::operator+(Mat2x2 const& m)
{
    return Mat2x2(this->values[0]+m.values[0], this->values[1]+m.values[1],
                  this->values[2]+m.values[2], this->values[3]+m.values[3]);
}

Mat2x2 Mat2x2::operator*(Mat2x2 const& m)
{
    sf::Vector2f u(m.values[0], m.values[2]);
    sf::Vector2f v(m.values[1], m.values[3]);

    u = (*this)*u;
    v = (*this)*v;

    return Mat2x2(u.x,v.x,
                  u.y,v.y);

}


Mat2x2 Mat2x2::operator*(float const& k)
{
    return Mat2x2(this->values[0]*k,this->values[1]*k,
                  this->values[2]*k,this->values[3]*k);

}

sf::Vector2f Mat2x2::operator*(sf::Vector2f const& v)
{
    sf::Vector2f r;
    r.x = this->values[0]*v.x + this->values[1]*v.y;
    r.y = this->values[2]*v.x + this->values[3]*v.y;
    return r;
}







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


Mat3x3::Mat3x3(const float v[9])
{
    for(int i = 0 ; i < 9 ; ++i)
        values[i] = v[i];
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

sf::Vector3f Mat3x3::operator*(sf::Vector3f const& v) const
{
    sf::Vector3f r;
    r.x = this->values[0]*v.x + this->values[1]*v.y + this->values[2]*v.z;
    r.y = this->values[3]*v.x + this->values[4]*v.y + this->values[5]*v.z;
    r.z = this->values[6]*v.x + this->values[7]*v.y + this->values[8]*v.z;
    return r;
}

sf::Vector2f Mat3x3::operator*(sf::Vector2f const& v) const
{
    sf::Vector2f r;
    r.x = this->values[0]*v.x + this->values[1]*v.y;
    r.y = this->values[3]*v.x + this->values[4]*v.y;
    return r;
}


}
