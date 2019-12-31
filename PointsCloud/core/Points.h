#pragma once

//???
class Points
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Points() {};

    Points(float x_,float y_, float z_): x(x_), y(y_), z(z_){}
    Points(float* points) {
        x = points[0];
        y = points[1];
        z = points[2];
    
    }

    ~Points() {};
};

