#ifndef PATCHMODULE_H_
#define PATCHMODULE_H_

#include "Serinus.h"
#include "Util\Factory.h"
/**
This is a base class for every patch module, used in the engine.
Each module impementation should implement all virtual methods according to their specifications.
Each module has their special enum-namespaces, which should be maintained in PatchModuleConfigs.h
These namespaces serve many purposes, but main one is to name IO and parameters of the module.
Each module has it's own set of inputs (pointers to other module's outputs) and outputs (Samples)
Inputs can be treated as sample inputs, as well as modulating inputs for the parameters.
All parameters internal parameters pointers should be mapped to their respective members,
and do not forget to map all types in getParameterTypes() method.
Always call parent constructor and ItilializeVoices() directly in your derived constructor.

Authored: AK
Last revision: 26.05.2014
*/
class PatchModule {
public:
    /**This method is called first in constructor with parameters to initialize inputs and outputs.*/
    void ItilializeVoices(int maxOutputs, int maxInputs) {
        if (maxOutputs == 0) {
            output_ = NULL;
        } else {
            output_ = new Sample**[maxPoly_];
            for (int i = 0; i < maxPoly_; ++i) {
                output_[i] = new Sample*[maxOutputs];
                for (int j = 0; j < maxOutputs; ++j) {
                    output_[i][j] = new Sample[bufferSize_];
                }
            }
        }

        if (maxInputs == 0) {
            input_ = NULL;
        } else {
            input_ = new Sample***[maxPoly_];
            for (int i = 0; i < maxPoly_; ++i) {
                input_[i] = new Sample**[maxInputs];
                for (int j = 0; j < maxInputs; ++j) {
                    input_[i][j] = new Sample*[bufferSize_];
                }
            }
        }
    }
    /**This method fills output buffer for the specified voice and buffer size.*/
    virtual void FillBuffers(int voice, int bufferSize) = 0;
    /**This method updates all outputs and internal states of the module by an external command.*/
    virtual void ProcessCommand(const int &cmdType, int polyVoiceNr, const MidiCmd &inValue, int &retVal) = 0;
    /**This method loads up the configuration map (internal state) for the parameters.*/
    inline void LoadConfiguration(const ModuleValues &values) {
        if (parameterInfo_.empty() || values.empty()) return;
        ModuleTypes::const_iterator typesIt;
        ModuleValues::const_iterator valuesIt;
        for (valuesIt = values.begin(); valuesIt != values.end(); valuesIt++) {
            int parameter = valuesIt->first;
            std::string value = valuesIt->second;
            typesIt = parameterInfo_.find(parameter);
            int type = typesIt->second;
            switch (type) {
                case Types::INT: {
                    int *ptr = static_cast<int*>( parameters_[parameter] );
                    *ptr = atoi(value.c_str());
                    break;
                }
                case Types::FLOAT: {
                    float *ptr = static_cast<float*>( parameters_[parameter] );
                    *ptr = static_cast<float>( atof(value.c_str()) );
                    break;
                }
                case Types::SAMPLE: {
                    Sample *ptr = static_cast<Sample*>( parameters_[parameter] );
                    *ptr = static_cast<Sample>( atof(value.c_str()) );
                    break;
                }
                case Types::BOOL: {
                    bool *ptr = static_cast<bool*>( parameters_[parameter] );
                    if (value[0] == 'T' || value[0] == 't' || value[0] == '1') {
                        *ptr = true;
                    } else {
                        *ptr = false;
                    }
                    break;
                }
                default: break;
            }
        }
    };

    /**This method returns the address of the output buffer start of a given index and voice number.*/
    inline Sample* getOutput(const int voiceNum, const int index) {
        return &(output_[voiceNum][index][0]);
    }
    /**This method is used to link input with the output of other module.*/
    inline void setLink(const int voiceNum, const int index, Sample* link) {
        input_[voiceNum][index][0] = link;
    }
    /**This method checks, whether or not the module has some inputs.*/
    inline bool hasNoInputs() {
        if (input_ == NULL) {
            return true;
        } else {
            return false;
        }
    }
    /**Default constructor to be called from all derived classes.*/
    PatchModule(int maxPoly, int bufferSize) {
        maxPoly_ = maxPoly;
        bufferSize_ = bufferSize;
    }
    /**Virtual desctructor, so we can delete derived by deleting base class.*/
    virtual ~PatchModule() {
        delete[] output_;
        delete[] input_;
        delete[] parameters_;
    };
    void setId(int id) {
        id_ = id;
    }
protected:
    int id_;
    int maxPoly_;
    int bufferSize_;
    /**Pointer to an array of output buffer samples (multiple voices).*/
    Sample ***output_;
    /**Pointer to an array of pointers to output buffer of input module at specified index (multiple voices).*/
    Sample ****input_;
    /**Pointer to an array of pointers to internal parameters of the module.*/
    void **parameters_;
    /**Map with string name of the parameter, index to the look-up table and type.*/
    ModuleTypes parameterInfo_;
};

#endif /* PATCHMODULE_H_ */
