#include "WaveTableOsc.h"

const CreatorImpl<WaveTableOsc> WaveTableOsc::creator("WaveTableOsc");

WaveTableOsc::WaveTableOsc() {
    phasor = 0.0;
    phasorInc = 0.0;
    //todo make these constructor-dependent
    willInterpolate = false;
    tableLength = 4096;
    sampleTable = new float[tableLength];
    float inc = 2.0 / tableLength;
    for (int i = 0; i <= tableLength; i++) {
        sampleTable[i] = -1.0 + inc*i;
    }
    this->setFrequency(220.0);
}

void WaveTableOsc::Tick() {
    if (willInterpolate) {
        float properPhase = phasor*tableLength;
        int truncPhase = properPhase;
        float fracPhase = properPhase - truncPhase;
        float s0 = sampleTable[truncPhase++];
        if (truncPhase >= tableLength) truncPhase = 0;
        float s1 = sampleTable[truncPhase];
        outSample = (s0 + (s1 - s0)*fracPhase) * UPSCALE;
    } else {
        outSample = sampleTable[int(phasor*tableLength)] * UPSCALE;
    }
    //Update phase
    phasor += phasorInc;
    if (phasor >= 1.0)
        phasor -= 1.0;
}