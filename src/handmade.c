/* ALL PLATFORM INDEPENDENT CODE LIVES HERE */

#include "handmade.h"

internal void
GameOutputSound(GameSoundOutputBuffer* soundBuffer, 
                int toneHz) {
    
    local_persist float32_t tSine;
    int16_t toneVolume = 3000;
    int wavePeriod = soundBuffer->samplesPerSecond / toneHz;
    
    int16_t* sampleOut = soundBuffer->samples;
    
    for(int sampleIndex = 0; 
        sampleIndex < soundBuffer->sampleCount; 
        ++sampleIndex) {
        
        float32_t sineValue = sinf(tSine);
        int16_t sampleValue = (int16_t) (sineValue * toneVolume);
        *sampleOut++        = sampleValue;
        *sampleOut++        = sampleValue;
        
        tSine += 2.0f * pi32 * 1.0f / (float32_t) wavePeriod;
    }
    
    return;
}

// TODO: Change the name of the file to handmade.c
internal void 
Win32RenderGradient(GameOffscreenBuffer* Buffer, 
					int xOffset, int yOffset) {
	
	uint8_t* row = (uint8_t*)Buffer->memory;
	for(int y = 0; 
        y < Buffer->height; 
        ++y) {
        
		uint32_t* pixel = (uint32_t*)row;
		for(int x = 0; 
            x < Buffer->width; 
            ++x) {
			// pixel in memory: 00 00 00 00
			// You'd think this would be useful in the RGBxx representation, being like
			// RR GG BB xx. But that is not the case. 
			// If the pixel value is set to (255, 0, 0, 0), we should get a RED color
			// on the screen, according to our assumptiom. Instead, we get BLUE.
			// This is because of Little Endianness.
			// When loading this value into the 32-bit variable, 
			// we get the following value
			// 0x00000000 ---> 0xxxBBGGRR
			
			uint8_t blue  = (x + xOffset);
			uint8_t green = (y + yOffset);
			
			// set the blue channel to the lower 8 bits
			// shift the green channel to the next 8 bits and OR them together
			// to get one clean compsite value
			
			// Memory   : BB GG RR xx
			// Register : xx RR GG BB
			// Pixel(32-bits)
			*pixel++ = ((green << 8) | blue);
		}
		row += Buffer->pitch;
	}
    
    return;
}

internal void
GameUpdateAndRender(GameMemory* gameMemory, 
                    GameInput* input, GameOffscreenBuffer* screenBuffer,
                    GameSoundOutputBuffer* soundBuffer) {
    
    ASSERT(sizeof(GameState) <= gameMemory->permanentStorageSize);
    
    GameState* gameState = gameMemory->permanentStorage;
    if(!gameMemory->isInitialized) {
        gameState->toneHz         = 256;
        gameMemory->isInitialized = true;
    }
    
    GameControllerInput* input0 = &input->controllers[0];
    
    if(input0->isAnalog) {
        gameState->xOffset += (int)4.0f * (input0->endX);
        gameState->toneHz = 256 + (int)(128.0f * (input0->endY));
    } else {
        
    }
    
    if(input0->down.endedDown) {
        ++gameState->yOffset;
    }
    
    GameOutputSound(soundBuffer, gameState->toneHz);
    Win32RenderGradient(screenBuffer, 
                        gameState->xOffset, gameState->yOffset);
    
    return;
}
