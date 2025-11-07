/*
  ==============================================================================

    ScaleModePage.h
    Created: 7 Nov 2025 1:17:18pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h" // so it knows your processor type



class ScaleModePage : public juce::Component
{
    public:
    ScaleModePage(GuitarControllerAudioProcessor& p);
    void resized() override;
    
    
    private:
    juce::Label label;
    GuitarControllerAudioProcessor& audioProcessor;

    
};
