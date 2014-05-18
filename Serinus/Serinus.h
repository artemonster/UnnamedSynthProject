#ifndef SERINUS_H_
#define SERINUS_H_

#include <iostream>
#include <math.h>
#include <cstdio>
#include <cstdint>
#include <map>
#include <vector>
#include "signal.h"
#include "HAL\RtAudio.h"
#include "HAL\RtMidi.h"
/**
================= WORK LOG =================
...prehistoric times:
Legend says, that a very blurry foundation to the project was laid somewhat in between 01.03.14 and 04.05.14

05.05.2014: Creation of the work log.
Today I've finally tweaked the module configuration system, so it will be as generic as possible.
If everything goes wrong, then I always can revert to stupid switchcase implementation for every module.
Also, the one and only SawDCO was renamed to DirectDCO (DDS), which now contains all 4 needed waveforms.

13.05.2014: Little things.
After some pause in the works I finally got back to the project. To get back to it I've started with liitle things,
like hiding PathModule member vars under protected and giving some accesor methods to them.
Also I've added a simple VCA module, which has two modes: linear and logarithmic. Second one is a little funky though,
I need an oscilloscope to see what actually happens there.
ADSR is also there, but I haven't decided yet on CV signals (a part of datapath or events)
*/

typedef int Sample; 					            //this type is used for samples
typedef std::map<int, int> ModuleTypes;
typedef std::map<int, std::string> ModuleValues;
namespace Types { enum TypeMapping { INT, FLOAT, BOOL }; }
struct InputConfig {
    int inputIndex;
    int sourceModule;
    int outputIndex;
};
const InputConfig NO_INPUT = { 0, 0, 0 };
typedef std::vector<InputConfig> ModuleInputs;

const unsigned int SAMPLE_RATE = 44100;				//won't change (I guess)
const unsigned int BUFFER_SIZE = 1024; 				//specify min max
const unsigned int MAX_VOICES = 16; 				//just for fun
const unsigned int SINT32_UPSCALE = 2147483647LL;	//scaling factor to transform normalised float -1...+1 to SInt32
const unsigned int SINT16_UPSCALE = 32767;

const unsigned int UPSCALE = SINT32_UPSCALE;
const float NORM = 1.0 / UPSCALE;
#endif /* SERINUS_H_ */
