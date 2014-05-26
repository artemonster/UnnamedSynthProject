#include "ADSR.h"

const CreatorImpl<ADSR> ADSR::creator("ADSR");

ADSR::ADSR(int maxPoly, int bufferSize) : PatchModule (maxPoly, bufferSize) {
    ItilializeVoices(O_ADSR::MAX, I_ADSR::MAX);
    outputSample_ = new float[maxPoly];
    state_ = new State[maxPoly];
    keyPressed_ = new bool[maxPoly];
    for (int i = 0; i < maxPoly; ++i) {
        outputSample_[i] = 0;
        state_[i] = IDLE;
        keyPressed_[i] = false;
    }
    parameters_ = new void*[P_ADSR::MAX];
    parameters_[P_ADSR::MODE] = &isLinear_;
    parameters_[P_ADSR::ATTACK] = &attack_;
    parameters_[P_ADSR::DECAY] = &decay_;
    parameters_[P_ADSR::SUSTAIN] = &sustain_;
    parameters_[P_ADSR::RELEASE] = &release_;

    ModuleTypes map {
        std::make_pair(P_ADSR::MODE, Types::BOOL),
        std::make_pair(P_ADSR::ATTACK, Types::FLOAT),
        std::make_pair(P_ADSR::DECAY, Types::FLOAT),
        std::make_pair(P_ADSR::SUSTAIN, Types::FLOAT),
        std::make_pair(P_ADSR::RELEASE, Types::FLOAT)
    };   
    parameterInfo_ = map;
    isLinear_ = true;
}

void ADSR::FillBuffers(int voice, int bufferSize) {
    Sample* gatebuf = input_[voice][I_ADSR::GATE][0];
    for (int i = 0; i < bufferSize; ++i) {
        Sample gate = *(gatebuf + i);
        if (gate >= 0.5f && keyPressed_[voice]==false) {
            keyPressed_[voice] = true;
            state_[voice]      = ATTACK;
            sustainLevel_      = sustain_ / 100;
            attackRate_        = 1.0f / (SAMPLE_RATE * (attack_ / 1000));
            decayRate_         = (1.0f - sustainLevel_ ) / (SAMPLE_RATE * (decay_ / 1000));
            releaseRate_       = sustainLevel_ / (SAMPLE_RATE * (release_ / 1000));
        } 
        if (gate <= 0.5f && keyPressed_[voice]==true) {
            keyPressed_[voice] = false;
            state_[voice]      = RELEASE;
            releaseRate_       = outputSample_[voice] / (SAMPLE_RATE * (release_ / 1000));
        }

        switch (state_[voice]) {
            case ATTACK:
                outputSample_[voice] += attackRate_;
                if (outputSample_[voice] >= 1.0f) {
                    outputSample_[voice] = 1.0f; //prevent overshoot
                    state_[voice] = DECAY;
                }
                break;
            case DECAY:
                outputSample_[voice] -= decayRate_;
                if (outputSample_[voice] <= sustainLevel_) {
                    outputSample_[voice] = sustainLevel_;
                    state_[voice] = SUSTAIN;
                }
                break;
            case SUSTAIN:
                outputSample_[voice] = sustainLevel_;
                break;
            case RELEASE:
                outputSample_[voice] -= releaseRate_;
                if (outputSample_[voice] <= 0.0f) {
                    outputSample_[voice] = 0.0f; //prevent undershoot
                    state_[voice] = IDLE;
                }
                break;
            case IDLE:
                outputSample_[voice] = 0.0f;
                break;
            default: break;
        }
        output_[voice][O_ADSR::SAMPLE][i] = outputSample_[voice];  
    }
}