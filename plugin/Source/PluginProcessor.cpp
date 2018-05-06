/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <time.h>

using namespace gin;

const char* VocAudioProcessor::paramTenseness               = "tenseness";
const char* VocAudioProcessor::paramConstrictionPosition    = "constrictionP";
const char* VocAudioProcessor::paramConstrictionAmount      = "constrictionA";
const char* VocAudioProcessor::paramSmoothing               = "smoothing";
const char* VocAudioProcessor::paramOutput                  = "output";

//==============================================================================
String percentTextFunction (const Parameter& p, float v)
{
    return String::formatted("%.0f%%", v / p.getUserRangeEnd() * 100);
}

String onOffTextFunction (const Parameter&, float v)
{
    return v > 0.0f ? "On" : "Off";
}

//==============================================================================
VocAudioProcessor::VocAudioProcessor()
{
    addPluginParameter (new Parameter (paramTenseness,            "Tenseness",             "Tenseness",    "", 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, percentTextFunction));
    addPluginParameter (new Parameter (paramConstrictionPosition, "Constriction Position", "Const Pos" ,   "", 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, percentTextFunction));
    addPluginParameter (new Parameter (paramConstrictionAmount,   "Constriction Amount",   "Const Amt",    "", 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, percentTextFunction));
    addPluginParameter (new Parameter (paramSmoothing,            "Smoothing",             "Smoothing",    "", 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, percentTextFunction));
    addPluginParameter (new Parameter (paramOutput,               "Output",                "Output",       "", 0.0f, 1.0f,  0.0f, 1.0f, 1.0f, percentTextFunction));
}

VocAudioProcessor::~VocAudioProcessor()
{
    if (voc != nullptr)
        voc_shutdown (voc);
}

//==============================================================================
void VocAudioProcessor::prepareToPlay (double sampleRate, int)
{
    if (voc != nullptr)
        voc_shutdown (voc);
    
    voc = voc_init ((unsigned long) sampleRate, (unsigned int) time (NULL));
    
    outputSmoothed.reset (sampleRate, 0.05);
    
    last = 0;
}

void VocAudioProcessor::releaseResources()
{
}

void VocAudioProcessor::runUntil (int& done, AudioSampleBuffer& buffer, int pos)
{
    int todo = jmin (pos, buffer.getNumSamples()) - done;
    
    if (todo > 0)
    {
        float* data = buffer.getWritePointer (0, done);
        for (int i = 0; i < todo; i++)
            data[i] = last = voc_f (voc, 0);
        
        done += todo;
    }

}

void VocAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midi)
{
    voc_constriction_amount_set (voc, getParameter (paramConstrictionAmount)->getUserValue());
    voc_constriction_position_set (voc, getParameter (paramConstrictionPosition)->getUserValue());
    voc_tenseness_set (voc, getParameter (paramTenseness)->getUserValue());
    voc_smoothing_set (voc, getParameter (paramSmoothing)->getUserValue());
    
    outputSmoothed.setValue (getParameter (paramOutput)->getUserValue());

    int done = 0;
    runUntil (done, buffer, 0);
    
    int pos = 0;
    MidiMessage msg;
    MidiBuffer::Iterator itr (midi);
    while (itr.getNextEvent (msg, pos))
    {
        runUntil (done, buffer, pos);
        
        if (msg.isNoteOn())
        {
            noteQueue.add (msg.getNoteNumber());
            velocity = msg.getVelocity();
        }
        else if (msg.isNoteOff())
        {
            noteQueue.removeFirstMatchingValue (msg.getNoteNumber());
            velocity = msg.getVelocity();
        }
        else if (msg.isAllNotesOff())
        {
            noteQueue.clear();
        }
        
        const int curNote = noteQueue.size() > 0 ? noteQueue.getLast() : -1;
        
        if (curNote != lastNote)
        {
            if (curNote == -1)
                voc_note_off (voc, velocity);
            else
                voc_note_on(voc, curNote, velocity);
            
            lastNote = curNote;
        }
    }
    
    runUntil (done, buffer, buffer.getNumSamples());
    
    float* data = buffer.getWritePointer (0);
    for (int i = 0; i < buffer.getNumSamples(); i++)
        data[i] *= outputSmoothed.getNextValue();
    
    ScopedLock sl (editorLock);
     if (editor)
         editor->scope.addSamples (data, buffer.getNumSamples());
}

//==============================================================================
bool VocAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* VocAudioProcessor::createEditor()
{
    editor = new VocAudioProcessorEditor (*this);
    return editor;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocAudioProcessor();
}
