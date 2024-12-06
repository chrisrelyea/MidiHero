/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <hidapi/hidapi.h>
#include <iostream>
#include <bitset>
#include <queue>
#include <chrono>

//==============================================================================
/**
*/
class GuitarControllerAudioProcessor  : public juce::AudioProcessor, public juce::Timer
{
public:
    //==============================================================================
    GuitarControllerAudioProcessor();
    ~GuitarControllerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void timerCallback() override;  // Override the timerCallback
    
    void updateMidiNote(int buttonIndex, double newMidiNote);
    

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarControllerAudioProcessor)

        // HID-related members
        hid_device* hidDevice = nullptr;
        unsigned char buf[65];
        std::atomic<bool> isRunning{false}; // To signal the HID thread to run/stop
        std::thread hidThread;
        
        // Shared state
        std::mutex mutex;
        int currentStatus = 0;
        
    
        // array of booleans corresponding to GH controller buttons, starting at G
        // G R Y B O
        std::bitset<5> currentButtonStates;
        
        std::bitset<5> newButtonStates;
    
        std::bitset<5> temp;
    

        double midiNotes[5] = {62, 60, 61, 63, 64};    // YGRBO ordering

        std::atomic<bool> newHIDDataAvailable{false};

        // Methods
        void hidPollingThread(); // Thread function for HID polling

    
};
