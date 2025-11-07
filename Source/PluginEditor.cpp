#include "PluginProcessor.h"
#include "PluginEditor.h"

GuitarControllerAudioProcessorEditor::GuitarControllerAudioProcessorEditor (GuitarControllerAudioProcessor& p)
    : AudioProcessorEditor (p), audioProcessor(p), noteModePage(p), scaleModePage(p)
{
   
    addAndMakeVisible(scaleModePage);
    addAndMakeVisible(noteModePage);
    // Set the size of the editor window
    
    noteModePage.setVisible(false);
    
    setSize(600, 400);
}

GuitarControllerAudioProcessorEditor::~GuitarControllerAudioProcessorEditor()
{
}

void GuitarControllerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);  // Change this color to suit your theme

    g.setColour(juce::Colours::white);  // Set the drawing color to white for labels and sliders
}

void GuitarControllerAudioProcessorEditor::resized()
{
    // Define the position and size of the sliders and labels
  
    auto bounds = getLocalBounds();
    scaleModePage.setBounds(bounds);
    noteModePage.setBounds(bounds);
}


