#include "DirectDCO.h"
#include <cmath>
const CreatorImpl<DirectDCO> DirectDCO::creator("DirectDCO");
const ParameterTypes DirectDCO::parameterInfo_ = {
        { "tune", { 0,Types::FLOAT } },
        { "detune", { 1,Types::FLOAT } },
        { "trig", { 2,Types::BOOL } },
        { "waveform", { 3,Types::INT } },
        { "mode", { 4,Types::INT } } };
const PortNames DirectDCO::outputInfo_ = {"sample"};
const PortNames DirectDCO::inputInfo_ = {"pitch","pwm","phase","amp"};

DirectDCO::DirectDCO(int maxPoly) : PatchModule (maxPoly) {
    ItilializeVoices(O::OMAX, I::IMAX);
    phasor_ = new float[kMaxPoly];
    for (int i = 0; i < kMaxPoly; ++i) {
        phasor_[i] = 0.0;
    }
    parameters_ = new void*[parameterInfo_.size()];
    parameters_[0] = &tuneFreq_;
    parameters_[1] = &detune_;
    parameters_[2] = &reTrigger_;
    parameters_[3] = &waveform_;
    parameters_[4] = &mode_;
    mode_ = 0;
}

void DirectDCO::FillBuffers() {
    for (int voice = 0; voice < voiceCount_; ++voice) {
        for (int i = 0; i < kBufferSize; ++i) {
            Sample  amplitude = *input_[voice][I::AMP][i];
            Sample  pwm       = *input_[voice][I::PWM][i];
            Sample  pitch     = *input_[voice][I::PITCH][i];
            Sample* output    = &output_[voice][O::SAMPLE][i];
            switch (waveform_) {
                case SAW:
                    *output = phasor_[voice] * amplitude;
                    break;
                case TRI:
                    if (phasor_[voice] <= 0.0f) {
                        *output = ( 2 * phasor_[voice] + 1.0f ) * amplitude;
                    } else {
                        *output = ( -2 * phasor_[voice] + 1.0f ) * amplitude;
                    }
                    break;
                case SIN:
                    *output = sin((phasor_[voice])*3.14159265f)*amplitude;
                    break;
                case SQR:
                    if ((phasor_[voice] + 1.0f) >= pwm*2) {
                        *output = amplitude;
                    } else {
                        *output = 0;
                    }
                    break;
                default:
                    break;
            }
            if (mode_ == 0) {   //Hz
                phasor_[voice] += pitch * 2.0f / kSampleRate;
            } else {            //CV
                phasor_[voice] += tuneFreq_ * static_cast<float>(pow(2, pitch)) * 2.0f / kSampleRate;
            }  
            if (phasor_[voice] >= 1.0f) phasor_[voice] -= 2.0f;       
        }
    }
}