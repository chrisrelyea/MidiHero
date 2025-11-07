/*
  ==============================================================================

    NoteModePage.cpp
    Created: 7 Nov 2025 1:17:27pm
    Author:  chris

  ==============================================================================
*/

#include "NoteModePage.h"

NoteModePage::NoteModePage(GuitarControllerAudioProcessor& p)
: audioProcessor(p) // <-- initializer list

{
    // Set slider properties
    gSlider.setRange(0, 127, 1);  // MIDI note range
    gSlider.addListener(this);
    gSlider.setValue(60);  // Default MIDI note for G
    addAndMakeVisible(gSlider);

    rSlider.setRange(0, 127, 1);
    rSlider.addListener(this);
    rSlider.setValue(61);  // Default MIDI note for R
    addAndMakeVisible(rSlider);

    ySlider.setRange(0, 127, 1);
    ySlider.addListener(this);
    ySlider.setValue(62);  // Default MIDI note for Y
    addAndMakeVisible(ySlider);

    bSlider.setRange(0, 127, 1);
    bSlider.addListener(this);
    bSlider.setValue(63);  // Default MIDI note for B
    addAndMakeVisible(bSlider);

    oSlider.setRange(0, 127, 1);
    oSlider.addListener(this);
    oSlider.setValue(64);  // Default MIDI note for O
    addAndMakeVisible(oSlider);

    // Set slider labels
    gLabel.setText("G", juce::dontSendNotification);
    gLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    addAndMakeVisible(gLabel);

    rLabel.setText("R", juce::dontSendNotification);
    rLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    addAndMakeVisible(rLabel);

    yLabel.setText("Y", juce::dontSendNotification);
    yLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    addAndMakeVisible(yLabel);

    bLabel.setText("B", juce::dontSendNotification);
    bLabel.setColour(juce::Label::textColourId, juce::Colours::blue);
    addAndMakeVisible(bLabel);

    oLabel.setText("O", juce::dontSendNotification);
    oLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
    addAndMakeVisible(oLabel);
}
void NoteModePage::resized()
{
    gSlider.setBounds(40, 40, 300, 20);
    rSlider.setBounds(40, 80, 300, 20);
    ySlider.setBounds(40, 120, 300, 20);
    bSlider.setBounds(40, 160, 300, 20);
    oSlider.setBounds(40, 200, 300, 20);

    gLabel.setBounds(10, 40, 30, 20);
    rLabel.setBounds(10, 80, 30, 20);
    yLabel.setBounds(10, 120, 30, 20);
    bLabel.setBounds(10, 160, 30, 20);
    oLabel.setBounds(10, 200, 30, 20);
}

void NoteModePage::sliderValueChanged(juce::Slider* slider)
{
    // Update the MIDI notes in the processor when a slider value changes
    if (slider == &gSlider)
    {
        audioProcessor.updateMidiNote(1, gSlider.getValue());  // Update MIDI note for Y
    }
    else if (slider == &rSlider)
    {
        audioProcessor.updateMidiNote(2, rSlider.getValue());  // Update MIDI note for G
    }
    else if (slider == &ySlider)
    {
        audioProcessor.updateMidiNote(0, ySlider.getValue());  // Update MIDI note for R
    }
    else if (slider == &bSlider)
    {
        audioProcessor.updateMidiNote(3, bSlider.getValue());  // Update MIDI note for B
    }
    else if (slider == &oSlider)
    {
        audioProcessor.updateMidiNote(4, oSlider.getValue());  // Update MIDI note for O
    }
}


