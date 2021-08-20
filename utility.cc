#include "utility.h"

#include <math.h>
#include <SDL2/SDL_image.h>
#include <dos.h>


void ClampF(float * in, float min, float max)
{
    if ( *in < min ) {
        *in = min;
    } else if ( *in >max ) {
        *in = max;
    }
}


bool FlashInterval(int ms)
{
    return (int)SDL_GetTicks() % (ms * 2) < ms;
}


void DrawLine_Bresenham(SDL_Renderer * renderer, Vec2 p1, Vec2 p2)
{
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;
    
   int m_new = 2 * (y2 - y1);
   int slope_error_new = m_new - (x2 - x1);
   for (int x = x1, y = y1; x <= x2; x++)
   {
       SDL_RenderDrawPoint(renderer, x, y);
 
       // Add slope to increment angle formed
       slope_error_new += m_new;
 
       // Slope error reached limit, time to
       // increment y and update slope error.
       if (slope_error_new >= 0)
       {
           y++;
           slope_error_new  -= 2 * (x2 - x1);
       }
   }
}


// digital differential analyzer algorithm
void DrawLine_DDA(SDL_Renderer * renderer, Vec2 p1, Vec2 p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float ax = fabsf(dx);
    float ay = fabsf(dy);
    float step = (ax >= ay) ? ax : ay;
    dx /= step;
    dy /= step;
    float x = p1.x;
    float y = p1.y;
    
    for( int i = 0; i < step; i++ ) {
        SDL_RenderDrawPoint(renderer, x, y);
        x += dx;
        y += dy;
    }
}


bool VecInRect(Vec2 v, const SDL_Rect * r)
{
    int x = (float)v.x;
    int y = (float)v.y;
    int left = r->x;
    int right = r->x + r->w;
    int top = r->y;
    int bottom = r->y + r->h;
    
    return x >= left && x <= right && y >= top && y <= bottom;
}


void PrintRect(SDL_Rect r)
{
    printf("(%d, %d, %d, %d)\n", r.x, r.y, r.w, r.h);
}


int CalculateLargestWindowedScale(int height)
{
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    printf("desktop height: %d\n", mode.h);
    
    int scale = mode.h / height - 1;
    printf("scale: %d\n", scale);
    //scale = 5;
    
    return scale;
}


// TODO: support other ascpect ratios!
void SetFullscreen(SDL_Window * window, SDL_Renderer * renderer, bool fs, int height)
{
    if ( fs ) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        
        // scale things up
        int h;
        SDL_GetWindowSize(window, NULL, &h);
        float scale = (float)h / (float)height;
        SDL_RenderSetScale(renderer, scale, scale);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        
        // scale things down
        int scale = CalculateLargestWindowedScale(height);
        SDL_RenderSetScale(renderer, scale, scale);
    }
}


void RandomizedSound(int len, int min_freq, int max_freq)
{
    if ( min_freq == 0 || max_freq == 0 ) {
        return;
    }
    
    for ( int i = 0; i < len; i++ ) {
        DOS_QueueSound(arc4random_uniform(max_freq - min_freq) + min_freq, 25);
    }
    
    DOS_PlayQueuedSound();
}


void WrapPosition(Vec2 * position, int w, int h)
{
    position->x = fmod(position->x + w, w);
    position->y = fmod(position->y + h, h);
}


static double sq(double x)
{
    return x * x;
}


static const double eps = 1e-14;

#if 0
static bool within(double x1, double y1, double x2, double y2, double x, double y)
{
    double d1 = sqrt(sq(x2 - x1) + sq(y2 - y1)); // dist between end-points
    double d2 = sqrt(sq(x - x1) + sq(y - y1)); // dist from point to one end
    double d3 = sqrt(sq(x2 - x) + sq(y2 - y)); // dist from point to other end
    double delta = d1 - d2 - d3;
    
    return fabs(delta) < eps; // true if delta is less than a small tolerance
}

static int rxy(double x1, double y1, double x2, double y2, double x, double y, bool segment)
{
    if (!segment || within(x1, y1, x2, y2, x, y)) {
        return 1;
    } else {
        return 0;
    }
}
 
static double fx(double a, double b, double c, double x) {
    return -(a * x + c) / b;
}
 
static double fy(double a, double b, double c, double y) {
    return -(b * y + c) / a;
}
#endif

// Prints the intersection points (if any) of a circle, center 'cp' with
// radius 'r', and either an infinite line containing the points 'p1' and 'p2'
// or a segment drawn between those points.
bool LineCircleIntersection(Vec2 p1, Vec2 p2, Vec2 cp, double r/*, bool segment*/)
{
    double x0 = cp.x, y0 = cp.y;
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double A = y2 - y1;
    double B = x1 - x2;
    double C = x2 * y1 - x1 * y2;
    double a = sq(A) + sq(B);
    double b, c, d;
    bool bnz = true;
    //int cnt = 0;
 
    if (fabs(B) >= eps) {
        // if B isn't zero or close to it
        b = 2 * (A * C + A * B * y0 - sq(B) * x0);
        c = sq(C) + 2 * B * C * y0 - sq(B) * (sq(r) - sq(x0) - sq(y0));
    } else {
        b = 2 * (B * C + A * B * x0 - sq(A) * y0);
        c = sq(C) + 2 * A * C * x0 - sq(A) * (sq(r) - sq(x0) - sq(y0));
        bnz = false;
    }
    d = sq(b) - 4 * a * c; // discriminant
    if (d < 0) {
        // line & circle don't intersect
        //printf("[]\n");
        return false;
    }
 
#if 0
    if (d == 0) {
        // line is tangent to circle, so just one intersect at most
        if (bnz) {
            double x = -b / (2 * a);
            double y = fx(A, B, C, x);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
        } else {
            double y = -b / (2 * a);
            double x = fy(A, B, C, y);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
        }
    } else {
        // two intersects at most
        d = sqrt(d);
        if (bnz) {
            double x = (-b + d) / (2 * a);
            double y = fx(A, B, C, x);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
 
            x = (-b - d) / (2 * a);
            y = fx(A, B, C, x);
            cnt += rxy(x1, y1, x2, y2, x, y, segment);
        } else {
            double y = (-b + d) / (2 * a);
            double x = fy(A, B, C, y);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
 
            y = (-b - d) / (2 * a);
            x = fy(A, B, C, y);
            cnt += rxy(x1, y1, x2, y2, x, y, segment);
        }
    }
#endif
    return true;
    
//    if (cnt <= 0) {
//        printf("[]");
//    }
}


int Map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void SetColor(SDL_Renderer * renderer, const SDL_Color * color)
{
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}



// midpoint circle algorithm
void DrawCircle (SDL_Renderer * renderer, u16 x0, u16 y0, u16 radius)
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
 
    SDL_RenderDrawPoint(renderer, x0, y0 + radius);
    SDL_RenderDrawPoint(renderer, x0, y0 - radius);
    SDL_RenderDrawPoint(renderer, x0 + radius, y0);
    SDL_RenderDrawPoint(renderer, x0 - radius, y0);
 
    while( x < y ) {
        
        if( f >= 0 ) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        
        x++;
        ddF_x += 2;
        f += ddF_x + 1;
        
        SDL_RenderDrawPoint(renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(renderer, x0 + x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(renderer, x0 - y, y0 - x);
    }
}
