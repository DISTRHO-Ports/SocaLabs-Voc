/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

using namespace gin;

//==============================================================================
VocAudioProcessorEditor::VocAudioProcessorEditor (VocAudioProcessor& p)
    : GinAudioProcessorEditor (p), proc (p)
{
    additionalProgramming = "Neil Thapen";
    
    logo = ImageFileFormat::loadFrom (BinaryData::logo_png, BinaryData::logo_pngSize);
    
    for (auto pp : p.getPluginParameters())
    {
        auto c = pp->isOnOff() ? (ParamComponent*)new Switch (pp) : (ParamComponent*)new Knob (pp);
        
        addAndMakeVisible (c);
        controls.add (c);
    }
    
    addAndMakeVisible (&scope);
    
    setGridSize (7, 2);
    
    scope.setNumSamplesPerPixel (2);
    scope.setVerticalZoomFactor (3.0f);
}

VocAudioProcessorEditor::~VocAudioProcessorEditor()
{
}

//==============================================================================
void VocAudioProcessorEditor::paint (Graphics& g)
{
    GinAudioProcessorEditor::paint (g);
        
    g.drawImageAt (logo, getWidth() / 2 - logo.getWidth() / 2, 0);
}

void VocAudioProcessorEditor::resized()
{
    using AP = VocAudioProcessor;
    
    GinAudioProcessorEditor::resized();
    
    componentForId (AP::paramTenseness)->setBounds (getGridArea (0, 0));
    componentForId (AP::paramSmoothing)->setBounds (getGridArea (1, 0));
    componentForId (AP::paramConstrictionPosition)->setBounds (getGridArea (2, 0));
    componentForId (AP::paramConstrictionAmount)->setBounds (getGridArea (3, 0));

    componentForId (AP::paramAttack)->setBounds (getGridArea (0, 1));
    componentForId (AP::paramDecay)->setBounds (getGridArea (1, 1));
    componentForId (AP::paramSustain)->setBounds (getGridArea (2, 1));
    componentForId (AP::paramRelease)->setBounds (getGridArea (3, 1));
    
    componentForId (AP::paramGlide)->setBounds (getGridArea (6, 0));
    componentForId (AP::paramOutput)->setBounds (getGridArea (6, 1));

    scope.setBounds (getGridArea (4, 0, 2, 2).reduced (5));
}
