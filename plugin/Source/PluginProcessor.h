/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

extern "C" {
#include "voc.h"
}

//==============================================================================
/**
*/
class VocAudioProcessorEditor;
class VocAudioProcessor : public gin::GinProcessor
{
public:
    //==============================================================================
    VocAudioProcessor();
    ~VocAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    
    static const char* paramTenseness;
    static const char* paramConstrictionPosition;
    static const char* paramConstrictionAmount;
    static const char* paramSmoothing;
    static const char* paramGlide;
    static const char* paramOutput;

    static const char* paramAttack;
    static const char* paramDecay;
    static const char* paramSustain;
    static const char* paramRelease;
    
    gin::AudioFifo fifo {1,44100};

private:
    void runUntil (int& done, AudioSampleBuffer& buffer, int pos);
    
    int lastNote = -1;
    int velocity = 0;
    float bend = 0;
    float lastGlide = 0;
    double sampleRate = 44100.0;
    Array<int> noteQueue;
    
    LinearSmoothedValue<float> outputSmoothed;
    LinearSmoothedValue<float> noteSmoothed;
    
    CriticalSection editorLock;
    VocAudioProcessorEditor* editor = nullptr;
    
    voc_state* voc = nullptr;
    
    gin::ADSR adsr;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocAudioProcessor)
};
