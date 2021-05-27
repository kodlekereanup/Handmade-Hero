#include <math.h>
#include <stdint.h>

//TODO(anup): Full review the Windows API Platform Layer Code 
//            DAY14
#define internal       static
#define local_persist  static
#define global_persist static
#define pi32 3.14159265359f

/*
Instead of using different files abstracted by the .h files, we just stick the
handmade.c source file into the win32_handmade.c file as means of composition.
 A good advantage of this is that it so happens to act as a single translation
 unit, meaning the devs don't have to exercise their minds when it comes to 
compilation and build issues.

The file acts as "one" big unit of compilation and is never necessary to be 
included in the compilation command.
*/
typedef int32_t bool32_t;
typedef float   float32_t;
typedef double  float64_t;

#include "handmade.c"
#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>
#include <stdbool.h>

#include "win32_handmade.h"

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)

typedef X_INPUT_GET_STATE(fpXInputGetState);
typedef X_INPUT_SET_STATE(fpXInputSetState);

// NOTE(anup): Learn what are stubs
X_INPUT_GET_STATE(XInputGetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
X_INPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }

global_persist bool32_t globalRunning;
global_persist fpXInputGetState* XInputGetState_ = XInputGetStateStub;
global_persist fpXInputSetState* XInputSetState_ = XInputSetStateStub;
global_persist Win32_Offscreen_Buffer GlobalBackBuffer;
global_persist LPDIRECTSOUNDBUFFER globalSecondaryBuffer;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(fpDirectSoundCreate);

// ---------------------------------------- HELPER MACROS -------------------------------

// #define MIN(X, Y)  (((X) < (Y)) ? (X) : (Y))
// #define MAX(X, Y)  (((X) > (Y)) ? (X) : (Y))
// #define SWAP(X, Y) ()
// #define COM_OBJ->FUN(...) OBJ##->lpVtbl->FUN(OBJ, __VA_ARGS__)

#define COM(OBJ, FN, ...) OBJ##->lpVtbl->FN(OBJ, __VA_ARGS__)


// --------------------------------------------------------------------------------------

//TODO: Copy this function from day13 file of Casey
internal void
Win32LoadXInput(void) {
	HMODULE  XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(XInputLibrary) {
		XInputGetState = (fpXInputGetState*)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState_= (fpXInputSetState*)GetProcAddress(XInputLibrary, "XInputSetState");
	} else {
        
	}
    
    return;
}

internal void
Win32InitDSound(HWND window, int32_t samplesPerSecond, int32_t bufferSize) {
	HMODULE dSoundLibrary = LoadLibraryA("dsound.dll");
	if(dSoundLibrary) {
		fpDirectSoundCreate* directSoundCreate = (fpDirectSoundCreate*) GetProcAddress(dSoundLibrary, "DirectSoundCreate"); 
		
		LPDIRECTSOUND directSound;
		if(directSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0))) {
			
			WAVEFORMATEX waveFormat    = {0};
			waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
			waveFormat.nChannels       = 2;
			waveFormat.nSamplesPerSec  = samplesPerSecond;
			waveFormat.wBitsPerSample  = 16;
			waveFormat.nBlockAlign     = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
			waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			waveFormat.cbSize          = 0;
			
			if(SUCCEEDED(COM(directSound, SetCooperativeLevel, window, DSSCL_PRIORITY))) {
				
				DSBUFFERDESC bufferDescription = { 0 }; 
				bufferDescription.dwSize  = sizeof(bufferDescription);
				bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER; 
				
				LPDIRECTSOUNDBUFFER primaryBuffer;
                
                if(SUCCEEDED(COM(directSound, CreateSoundBuffer, 
                                 &bufferDescription, &primaryBuffer, 0))) {
                    
					if(SUCCEEDED(COM(primaryBuffer, SetFormat, &waveFormat))) {
                        
					} else {
                        
					}
				} else {
                    
				}
			} else {
                
			}
			
			DSBUFFERDESC bufferDescription  = {0};
			bufferDescription.dwSize        = sizeof(bufferDescription);
			bufferDescription.dwFlags       = 0;
			bufferDescription.dwBufferBytes = bufferSize;
			bufferDescription.lpwfxFormat   = &waveFormat;
			
			if(SUCCEEDED(COM(directSound, CreateSoundBuffer, 
                             &bufferDescription, &globalSecondaryBuffer, 0))) {
				
			} else {
                
			}
		} else {
            
		}
	} else {
        
	}
    
    return;
}

internal Win32_Window_Dimensions 
Win32GetWindowDimensions(HWND Window) {
	Win32_Window_Dimensions result;
	
	RECT clientRect;
	GetClientRect(Window, &clientRect);
	result.width  = clientRect.right - clientRect.left;
	result.height = clientRect.bottom - clientRect.top;
	
	return result;
}

internal void 
Win32ResizeDIBSection(Win32_Offscreen_Buffer* Buffer, 
					  int width, int height) {
	//Win32ResizeDeviceIndependentBitmap 
	
	if(Buffer->memory) VirtualFree(Buffer->memory, 0, MEM_RELEASE);
	
	Buffer->width = width;
	Buffer->height = height;
	int bytesPerPixel = 4;
	
	Buffer->info.bmiHeader.biSize        = sizeof(Buffer->info.bmiHeader);
	Buffer->info.bmiHeader.biWidth       = Buffer->width;
	Buffer->info.bmiHeader.biHeight      = -Buffer->height;
	Buffer->info.bmiHeader.biPlanes      = 1;
	Buffer->info.bmiHeader.biBitCount    = 32;
	Buffer->info.bmiHeader.biCompression = BI_RGB;
	
	int bitmapMemorySize = (Buffer->width * Buffer->height) * bytesPerPixel;
	Buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	Buffer->pitch = Buffer->width * bytesPerPixel;
    
    return;
}

internal void 
Win32DisplayBufferInWindow(HDC dc, Win32_Offscreen_Buffer* Buffer,
						   int windowWidth, int windowHeight) {
	
	StretchDIBits(dc, 
				  0, 0, windowWidth, windowHeight,
				  0, 0, Buffer->width, Buffer->height,
				  Buffer->memory, &Buffer->info,
				  DIB_RGB_COLORS, SRCCOPY);
	return;
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window, 
						UINT Message, 
						WPARAM wParam, 
						LPARAM lParam) {
	
	LRESULT result = 0;
	
	switch(Message) {
		case WM_DESTROY: {
			//TODO(anup): Handle this as ab error - recreate window?
			globalRunning = false;
		} break;
		
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP: 
		case WM_KEYDOWN: 
		case WM_KEYUP: {
			uint32_t virtualKeyCode = wParam;
			bool32_t wasDown = ((lParam & (1 << 30)) != 0);
			bool32_t isDown  = ((lParam & (1 << 31)) == 0);  
			if(wasDown != isDown) { 
				if(virtualKeyCode == 'W') {
				} else if(virtualKeyCode == 'A') {
				} else if(virtualKeyCode == 'S') {
				} else if(virtualKeyCode == 'D') {
				} else if(virtualKeyCode == 'Q') {
				} else if(virtualKeyCode == 'E') {
				} else if(virtualKeyCode == VK_UP) { 
				} else if(virtualKeyCode == VK_LEFT) {
				} else if(virtualKeyCode == VK_DOWN) {
				} else if(virtualKeyCode == VK_RIGHT) {
				} else if(virtualKeyCode == VK_ESCAPE) {
				} else if(virtualKeyCode == VK_SPACE) {
				}
			}
			
			bool32_t altKeyWasDown = (lParam & (1 << 29)); 
			if((virtualKeyCode == VK_F4)  && altKeyWasDown) globalRunning = false;
			
		} break;
		
		case WM_CLOSE: {
			//TODO(anup): Handle this with a message to the user?
			globalRunning = false;
		} break;
		
		case WM_ACTIVATEAPP: {
		} break;
		
		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC dc = BeginPaint(Window, &paint);
			Win32_Window_Dimensions dimension = Win32GetWindowDimensions(Window);
			Win32DisplayBufferInWindow(dc, &GlobalBackBuffer, dimension.width, dimension.height);
			EndPaint(Window, &paint);
		} break;
		default: {
			result = DefWindowProcA(Window, Message, wParam, lParam);
		} break;
	}
	
	return result;
}

internal void
Win32ClearSoundBuffer(Win32_Sound_Output* soundOutput) {
    
    VOID* region1;
	VOID* region2;
	DWORD region1Size;
	DWORD region2Size;
	
	if(SUCCEEDED(COM(globalSecondaryBuffer, Lock,
                     0, soundOutput->secondaryBufferSize,
                     &region1, &region1Size,
                     &region2, &region2Size,
                     0))) {
		
		uint8_t* destSample = (uint8_t*) region1;
        for(DWORD byteIndex = 0; byteIndex < region1Size; ++byteIndex) 
			*destSample++ = 0;
		
        destSample = (uint8_t*) region2;
        for(DWORD byteIndex = 0; byteIndex < region2Size; ++byteIndex) 
			*destSample++ = 0;
        
        COM(globalSecondaryBuffer, Unlock, region1, region1Size, region2, region2Size);
        
        //globalSecondaryBuffer->lpVtbl->Unlock(globalSecondaryBuffer,
        //region1, region1Size, 
        //region2, region2Size);
    } 
    
    return;
}


internal void 
Win32FillSoundBuffer(Win32_Sound_Output* soundOutput, 
                     GameSoundOutputBuffer* sourceBuffer,
                     DWORD byteToLock, DWORD bytesToWrite) {
    VOID* region1;
    VOID* region2;
    DWORD region1Size;
    DWORD region2Size;
    
    if(SUCCEEDED(COM(globalSecondaryBuffer, Lock, byteToLock, bytesToWrite, 
                     &region1, &region1Size, &region2, &region2Size, 0))) {
        
        // TODO: Move this piece of code into a function
        // It is redundant
        DWORD region1SampleCount = region1Size / soundOutput->bytesPerSample;
        int16_t* destSample      = (int16_t*) region1;
        int16_t* sourceSample    = sourceBuffer->samples;
        for(DWORD sampleIndex = 0;
            sampleIndex < region1SampleCount;
            ++sampleIndex) {
            
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
            ++soundOutput->runningSampleIndex;
        }
        
        DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;
        destSample               = (int16_t*) region2;
        for(DWORD sampleIndex = 0;
            sampleIndex < region2SampleCount;
            ++sampleIndex) {
            
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
            ++soundOutput->runningSampleIndex;
        }
        
        COM(globalSecondaryBuffer, Unlock, region1, region1Size, region2, region2Size);
    }
    
    return;
}

internal void
Win32ProcessXInputDigitalButton(GameButtonState* oldState, GameButtonState* newState, 
                                DWORD XInputButtonState, DWORD buttonBit) {
    
    newState->endedDown = ((XInputButtonState & buttonBit) == buttonBit);
    newState->halfTransCount = (oldState->endedDown != newState->endedDown) ? 1 : 0;
    return;
}

int CALLBACK
WinMain(HINSTANCE hInstance, 
        HINSTANCE hPrevInstance, 
        LPSTR lpCmdLine, 
        int nCmdShow) {
    
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);
    int64_t perfCountFrequency = perfCountFrequencyResult.QuadPart;
    
    Win32LoadXInput();
    WNDCLASSA WindowClass = { 0 };
    
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
    
    //FILE* debugInfo = fopen("win32_handmade_debug_info.csv", "w+");
    
    //NOTE(anup): What does this all mean?
    WindowClass.style       = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance   = hInstance;
    //WindowClass.hIcon = ;
    WindowClass.lpszClassName = "Handmade Hero";
    
    if(RegisterClass(&WindowClass)) {
        HWND window = 
            CreateWindowEx(0, WindowClass.lpszClassName,
                           "Handmade Hero",
                           WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           0, 0, hInstance, 0);
        
        if(window) {
            HDC dc = GetDC(window);
            
            Win32_Sound_Output soundOutput  = { 0 };
            soundOutput.samplesPerSecond    = 48000;
            soundOutput.runningSampleIndex  = 0;
            soundOutput.bytesPerSample      = sizeof(int16_t) * 2;
            soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            soundOutput.latencySampleCount  = soundOutput.samplesPerSecond / 15;
            
            Win32InitDSound(window, soundOutput.samplesPerSecond, soundOutput.secondaryBufferSize);
            Win32ClearSoundBuffer(&soundOutput);
            globalSecondaryBuffer->lpVtbl->Play(globalSecondaryBuffer, 0, 0, DSBPLAY_LOOPING);
            
            globalRunning = true;
            
            int16_t* samples = (int16_t*) VirtualAlloc(0, soundOutput.secondaryBufferSize,
                                                       MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            
            GameInput gInput[2] = { 0 };
            GameInput* newInput = &gInput[0];
            GameInput* oldInput = &gInput[1];
            
            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);
            uint64_t lastCycleCount = __rdtsc();
            
            while(globalRunning) {
                MSG message;
                
                while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                    if(message.message == WM_QUIT) globalRunning = false;
                    
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                } 
                
                // NOTE: min, max, _countof are MSVC specific macros. 
                // TODO: Define your own macros to make it platform agnostic. 
                int maxControllerCount = min(max(0, XUSER_MAX_COUNT), _countof(newInput->controllers));
                
                /*
                int maxControllerCount = XUSER_MAX_COUNT;
                if(maxControllerCount > ARRAYCOUNT(input.controllers)) {
                    maxControllerCount = ARRAYCOUNT(input.controllers);
                }
*/
                
                //NOTE(anup): How does this for-loop work?
                for(DWORD controllerIndex = 0; 
                    controllerIndex < maxControllerCount; 
                    ++controllerIndex) {
                    
                    GameControllerInput* oldController = &oldInput->controllers[controllerIndex];
                    GameControllerInput* newController = &newInput->controllers[controllerIndex];
                    
                    XINPUT_STATE controllerState;
                    
                    if(XInputGetState(controllerIndex, &controllerState) == ERROR_SUCCESS) {
                        //NOTE(anup): Why did we use a pointer here?
                        XINPUT_GAMEPAD* gamepad = &controllerState.Gamepad;
                        
                        bool32_t gamepadUp    = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool32_t gamepadDown  = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool32_t gamepadLeft  = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool32_t gamepadRight = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        
                        newController->isAnalog = true;
                        newController->startX   = oldController->endX;
                        newController->startY   = oldController->endY;
                        
                        newController->minX = newController->maxX = newController->endX = 
                            (gamepad->sThumbLX < 0) ? 
                            (float32_t) gamepad->sThumbLX / 32768.0f : (float32_t) gamepad->sThumbLX / 32767.0f;
                        
                        newController->minY = newController->maxY = newController->endY = 
                            (gamepad->sThumbLY < 0) ? 
                            (float32_t) gamepad->sThumbLY / 32768.0f : (float32_t) gamepad->sThumbLY / 32767.0f;
                        
                        Win32ProcessXInputDigitalButton(&oldController->leftShoulder,
                                                        &newController->leftShoulder,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_LEFT_SHOULDER);
                        
                        Win32ProcessXInputDigitalButton(&oldController->rightShoulder,
                                                        &newController->rightShoulder,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        
                        Win32ProcessXInputDigitalButton(&oldController->down,
                                                        &newController->down,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_A);
                        
                        Win32ProcessXInputDigitalButton(&oldController->right,
                                                        &newController->right,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_B);
                        
                        Win32ProcessXInputDigitalButton(&oldController->left,
                                                        &newController->left,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_X);
                        
                        Win32ProcessXInputDigitalButton(&oldController->up,
                                                        &newController->up,
                                                        gamepad->wButtons,
                                                        XINPUT_GAMEPAD_Y);
                        
                        //bool32_t gamepadStart         = (gamepad->wButtons & XINPUT_GAMEPAD_START);
                        //bool32_t gamepadBack          = (gamepad->wButtons & XINPUT_GAMEPAD_BACK);
                        
                        /* 
                             XINPUT_VIBRATION vibration;
                        if(gamepadA) {
                            vibration.wLeftMotorSpeed = 60000;
                            vibration.wRightMotorSpeed = 60000;
                            XInputSetState(0, &vibration);
                        } else {
                            vibration.wLeftMotorSpeed = 0;
                            vibration.wRightMotorSpeed = 0;
                            XInputSetState(0, &vibration);
                        */
                    } else {
                        // TODO
                    }
                }
                
                DWORD playCursor      = 0;
                DWORD writeCursor     = 0;
                DWORD byteToLock      = 0;
                DWORD targetCursor    = 0;
                DWORD bytesToWrite    = 0;
                bool32_t soundIsValid = false;
                
                if(SUCCEEDED(COM(globalSecondaryBuffer, GetCurrentPosition, 
                                 &playCursor, &writeCursor))) {
                    
                    byteToLock = ((soundOutput.runningSampleIndex * soundOutput.bytesPerSample) 
                                  % soundOutput.secondaryBufferSize);
                    
                    targetCursor = ((playCursor + 
                                     (soundOutput.latencySampleCount * soundOutput.bytesPerSample)) 
                                    % soundOutput.secondaryBufferSize);
                    
                    if(byteToLock > targetCursor) {
                        bytesToWrite = soundOutput.secondaryBufferSize - byteToLock;
                        bytesToWrite += targetCursor;
                    } else {
                        bytesToWrite = targetCursor - byteToLock;
                    }
                    
                    soundIsValid = true;
                }
                
                GameSoundOutputBuffer soundBuffer = { 0 };
                soundBuffer.samplesPerSecond      = soundOutput.samplesPerSecond;
                soundBuffer.sampleCount           = bytesToWrite / soundOutput.bytesPerSample;
                soundBuffer.samples               = samples;
                
                // TODO: Maybe move this into some kind of a constructor?
                GameOffscreenBuffer gbBuffer = { 0 };
                gbBuffer.memory = GlobalBackBuffer.memory;
                gbBuffer.width  = GlobalBackBuffer.width;
                gbBuffer.height = GlobalBackBuffer.height;
                gbBuffer.pitch  = GlobalBackBuffer.pitch;
                
                GameUpdateAndRender(newInput, &gbBuffer, &soundBuffer);
                
                if(soundIsValid) {
                    Win32FillSoundBuffer(&soundOutput, &soundBuffer, byteToLock, bytesToWrite);
                }
                
                Win32_Window_Dimensions dimension = Win32GetWindowDimensions(window);
                Win32DisplayBufferInWindow(dc, &GlobalBackBuffer, 
                                           dimension.width, dimension.height);
                //ReleaseDC(window, dc);
                uint64_t endCycleCount = __rdtsc();
                
                LARGE_INTEGER endCounter;
                QueryPerformanceCounter(&endCounter);
                
                uint64_t cyclesElapsed = endCycleCount - lastCycleCount;
                int64_t counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
                float32_t msPerFrame = (((1000.0f * (float32_t)counterElapsed) / (float32_t)perfCountFrequency));
                float32_t FPS = (float32_t)perfCountFrequency / (float32_t)counterElapsed;
                float32_t MCPF = ((float32_t)cyclesElapsed / (1000.0f * 1000.0f));
                
                //char buffer[256];
                //sprintf(buffer, "%fms/frame %fFPS %fmc/frame\n", msPerFrame, FPS, MCPF);
                //OutputDebugStringA(buffer);
                
                //TODO(anup): Temporary Code -- Cleanup
                //const char* OUTPUT_FORMAT = "\n%d, %d, %d";
                //fprintf(debugInfo, OUTPUT_FORMAT, msPerFrame, FPS, MCPF);
                
                lastCounter = endCounter;
                lastCycleCount = endCycleCount;
                
                //SWAP();
                GameInput* temp = newInput;
                newInput = oldInput;
                oldInput = temp;
            }
            
        } else {
            //TODO(anup): Logging
        }
        
    } else {
        //TODO(anup): Create Log System
    }
    
    //fclose(debugInfo);
    
    return 0;
}
