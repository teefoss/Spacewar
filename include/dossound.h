#ifndef dossound_h
#define dossound_h

#ifdef __cplusplus
extern "C" {
#endif

// initialize sound: SDL must be inited with SDL_INIT_AUDIO before calling
void DOS_InitSound(void);

// sound a pitch at 'frequency' hertz for 'milliseconds' length
// program execution is suspended while sound plays
void DOS_Sound(unsigned frequency, unsigned milliseconds);

// sound a pitch at 'frequency' hertz for 'milliseconds' length
// sound is played on its own thread
void DOS_SoundAsync(unsigned frequency, unsigned milliseconds);

// play an 800 hertz tone for 200 milliseconds
void DOS_Beep(void);

// queue a pitch at 'frequency' hertz for 'milliseconds' length that can
// be played asynchronously later with DOS_PlayQueuedSound()
void DOS_QueueSound(unsigned frequency, unsigned milliseconds);
void DOS_PlayQueuedSound(void);

// plays musical notes (asynchronous)
// string format:
//                                  EXAMPLE
//   notes:  a b c d e f g          "ccggaag"
//    flat:  [note]-                "a- b-"
//   sharp:  [note]+                "c+ d+"
//  length:  l[int(1, 2, 4...128)]  "l8 cdef l2 g"
//   tempo:  t[bmp]                 "t80 l2 g+ef+g+"
//  octave:  o[int(1-6)]            "o6 ba+ba+b"
//           > increase octave      "a b > c"
//           < decrease octave      "l2 e l4 dc < b"
//
void DOS_Play(const char * string);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* dossounc_h */
