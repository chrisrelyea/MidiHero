/*
  ==============================================================================

    NoteModePage.h
    Created: 7 Nov 2025 1:17:27pm
    Author:  chris

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h" // so it knows your processor type



class NoteModePage: public juce::Component, private juce::Slider::Listener
{
    public:
    
    NoteModePage(GuitarControllerAudioProcessor& p);
    void resized() override;
    
    private:
    juce::Slider gSlider, rSlider, ySlider, bSlider, oSlider;
    juce::Label gLabel, rLabel, yLabel, bLabel, oLabel;
    
    void sliderValueChanged(juce::Slider* slider) override;
    GuitarControllerAudioProcessor& audioProcessor;


    
};
