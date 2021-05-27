/* date = May 23rd 2021 9:35 pm */

#ifndef HANDMADE_H
#define HANDMADE_H

// ---------------------------------------- HELPER MACROS -------------------------------

// #define MIN(X, Y)  (((X) < (Y)) ? (X) : (Y))
// #define MAX(X, Y)  (((X) > (Y)) ? (X) : (Y))
// #define SWAP(X, Y) ()
// #define COM_OBJ->FUN(...) OBJ##->lpVtbl->FUN(OBJ, __VA_ARGS__)

#define COM(OBJ, FN, ...) OBJ##->lpVtbl->FN(OBJ, __VA_ARGS__)

/*
HANDMADE_INTERNAL
1: Build for developer only
0: Release Build!

HANDMADE_RELEASE:
0 : Build for developer only
1 : Build for public release

 HANDMADE_SLOW: 
0 : no slow code allowed
1 : slow code welcome

 HANDMADE_INTERNAL  is the same as !HANDMADE_RELEASE

*/

#if HANDMADE_SLOW
#define ASSERT(EXPRESSION) if(!(EXPRESSION)) { *(int*) 0 = 0; } 
#else
#define ASSERT(EXPRESSION)
#endif

#define KILOBYTES(VALUE) ((VALUE) * 1024) 
#define MEGABYTES(VALUE) (KILOBYTES(VALUE) * 1024)
#define GIGABYTES(VALUE) (MEGABYTES(VALUE) * 1024)
#define TERABYTES(VALUE) (GIGABYTES(VALUE) * 1024)

// ArrayCount is a user-defined macro. Can be substituted for _countof
#define ARRAYCOUNT(ARRAY) (sizeof(ARRAY) / sizeof((ARRAY)[0])) 


// --------------------------------------------------------------------------------------

typedef struct {
    void* memory;
    int width;
    int height;
    int pitch;
} GameOffscreenBuffer;

typedef struct {
    int16_t samplesPerSecond;
    int sampleCount;
    int16_t* samples;
} GameSoundOutputBuffer;

typedef struct {
    int halfTransCount;
    bool32_t endedDown;
} GameButtonState;

typedef struct {
    
    bool32_t isAnalog;
    
    float32_t startX;
    float32_t startY;
    float32_t minX;
    float32_t minY;
    float32_t maxX;
    float32_t maxY;
    float32_t endX;
    float32_t endY;
    
    union {
        GameButtonState buttons[6];
        struct {
            GameButtonState up;
            GameButtonState down;
            GameButtonState left;
            GameButtonState right;
            GameButtonState leftShoulder;
            GameButtonState rightShoulder;
        };
    };
} GameControllerInput;

typedef struct {
    GameControllerInput controllers[4];
} GameInput;

typedef struct {
    int toneHz;
    int xOffset;
    int yOffset;
} GameState;

typedef struct {
    bool32_t isInitialized;
    uint64_t permanentStorageSize;
    void* permanentStorage;
    uint64_t transientStorageSize;
    void* transientStorage;
} GameMemory;

/*
internal void 
GameUpdateAndRender(Game_Offscreen_Buffer* screenBuffer,
                    Game_Sound_Output_Buffer* soundBuffer);

internal void 
GameOutputSound(Game_Sound_Output_Buffer* soundBuffer, 
                int toneHz);
*/

#endif //HANDMADE_H