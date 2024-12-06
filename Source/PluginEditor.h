/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GuitarControllerAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    GuitarControllerAudioProcessorEditor (GuitarControllerAudioProcessor&);
    ~GuitarControllerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarControllerAudioProcessor& audioProcessor;
    
    juce::Slider gSlider, rSlider, ySlider, bSlider, oSlider;
    juce::Label gLabel, rLabel, yLabel, bLabel, oLabel;
    void sliderValueChanged(juce::Slider* slider);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarControllerAudioProcessorEditor)
};
