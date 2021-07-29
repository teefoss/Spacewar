#include "vec2.h"

#include <stdio.h>

Vec2::Vec2(float a, float b)
{
    x = a;
    y = b;
}

void Vec2::zero()
{
    x = 0.0f;
    y = 0.0f;
}

float Vec2::length() const
{
    return sqrtf(x * x + y * y);
}

float Vec2::lengthSquared() const
{
    return x * x + y * y;
}

float Vec2::normalize()
{
    float sqr_length = x * x + y * y;
    float inv_length = 1.0f / sqrtf(sqr_length);
    x *= inv_length;
    y *= inv_length;
    
    return inv_length * sqr_length;
}

bool Vec2::compare(const Vec2 & a, float epsilon) const
{
    if ( fabsf( x - a.x ) > epsilon ) {
        return false;
    }
            
    if ( fabsf( y - a.y ) > epsilon ) {
        return false;
    }

    return true;
}

void Vec2::print() const
{
    printf("(%.2f, %.2f)\n", x, y);
}

void Vec2::rotateByDegrees(float degrees)
{
    float radians = DEG_TO_RAD(degrees);
    rotateByRadians(radians);
}

void Vec2::rotateByRadians(float radians)
{
    float new_x = cos(radians) * x - sin(radians) * y;
    float new_y = sin(radians) * x + cos(radians) * y;
    x = new_x;
    y = new_y;}

Vec2 Vec2::operator+(const Vec2 & a) const
{
    return Vec2(x + a.x, y + a.y);
}

Vec2 Vec2::operator-(const Vec2 & a) const
{
    return Vec2(x - a.x, y - a.y);
}

Vec2 Vec2::operator*(float a) const
{
    return Vec2(x * a, y * a);
}

Vec2 Vec2::operator/(float a) const
{
    float invalid = 1.0f / a;
    return Vec2(x * invalid, y * invalid);
}

void Vec2::operator+=(const Vec2 & a)
{
    x += a.x;
    y += a.y;
}

void Vec2::operator-=(const Vec2 & a)
{
    x -= a.x;
    y -= a.y;
}

void Vec2::operator*=(float a)
{
    x *= a;
    y *= a;
}

void Vec2::operator/=(float a)
{
    float invalid = 1.0f / a;
    x *= invalid;
    y *= invalid;
}

bool Vec2::operator==(Vec2 & a) const
{
    return compare(a);
}

bool Vec2::operator!=(Vec2 & a) const
{
    return !compare(a);
}
