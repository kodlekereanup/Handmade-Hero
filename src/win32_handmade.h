/* date = May 24th 2021 3:47 pm */

#ifndef WIN32_HANDMADE_H
#define WIN32_HANDMADE_H

typedef struct { 
    BITMAPINFO info;
    void* memory;
	int width;    
    int height;
    int pitch;
} Win32_Offscreen_Buffer;

typedef struct {
    int width;
    int height;
} Win32_Window_Dimensions;

typedef struct {
	int samplesPerSecond;
	uint32_t runningSampleIndex;
	int bytesPerSample;
	int secondaryBufferSize;
	float32_t tSine;
	int latencySampleCount;
} Win32_Sound_Output;

#endif //WIN32_HANDMADE_H
