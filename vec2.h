#ifndef vec2_h
#define vec2_h

#include <math.h>

#define VEC_EPSILON   0.001f

#define DEG_TO_RAD(x)   ((x) * M_PI / 180.0f)
#define VEC_Clear(v)    { v.x = v.y = 0; }
#define SIGN(x)         x > 0 ? 1 : x < 0 ? -1 : 0

struct Vec2
{
    float x;
    float y;
    
    Vec2(float a = 0.0f, float b = 0.0f);
    
    void  Zero();
    float Length() const;
    float LengthSqr() const;
    float Normalize();
    bool  Compare(const Vec2 & a, float epsilon = VEC_EPSILON) const;
    void  RotateByDegrees(float degrees);
    void  RotateByRadians(float radians);
    void  print() const;
    
    Vec2 operator+(const Vec2 & a) const;
    Vec2 operator-(const Vec2 & a) const;
    Vec2 operator*(float a) const;
    Vec2 operator/(float a) const;
    void operator+=(const Vec2 & a);
    void operator-=(const Vec2 & a);
    void operator*=(float a);
    void operator/=(float a);
    bool operator==(Vec2 & a) const;
    bool operator!=(Vec2 & a) const;
};

#endif /* vec2_h */
