/* date = May 23rd 2021 9:35 pm */

#ifndef HANDMADE_H
#define HANDMADE_H

// ArrayCount is a user-defined macro. Can be substituted for _countof
#define ARRAYCOUNT(ARRAY) (sizeof(ARRAY) / sizeof((ARRAY)[0])) 

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

/*
internal void 
GameUpdateAndRender(Game_Offscreen_Buffer* screenBuffer,
                    Game_Sound_Output_Buffer* soundBuffer);

internal void 
GameOutputSound(Game_Sound_Output_Buffer* soundBuffer, 
                int toneHz);
*/

#endif //HANDMADE_H