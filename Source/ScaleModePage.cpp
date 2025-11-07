/*
  ==============================================================================

    ScaleModePage.cpp
    Created: 7 Nov 2025 1:17:18pm
    Author:  chris

  ==============================================================================
*/

#include "ScaleModePage.h"

ScaleModePage::ScaleModePage(GuitarControllerAudioProcessor& p)
: audioProcessor(p) // <-- initializer list

{
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setText("Scale Mode", juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::outlineColourId, juce::Colours::darkblue); // box color
    addAndMakeVisible(label);


}

void ScaleModePage::resized()
{
    auto parentWidth = getWidth();
    auto labelWidth = 200;  // or whatever width you want
    auto labelHeight = 30;

    label.setBounds((parentWidth - labelWidth) / 2, 20, labelWidth, labelHeight);
    
}
