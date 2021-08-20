#ifndef timer_h
#define timer_h

#include "types.h"
#include "random.h"

class Timer
{
public:
    Timer(s16 length, s16 variance = 0) {
        m_length = length;
        m_ticks = 0;
    }
    
    void Start() {
        s16 adjust = m_variance ? Random(-m_variance, m_variance) : 0;
        m_ticks = m_length + adjust;
    }
    
    void Stop() { m_ticks = 0; }
    void Run() { if ( m_ticks > 0 ) --m_ticks; }
    bool Done() { return m_ticks == 0; };
    s16  GetTicks() { return m_ticks; }
    
private:
    s16 m_variance; // + or - variance when starting timer
    s16 m_length;
    s16 m_ticks;
};

#endif /* timer_h */
