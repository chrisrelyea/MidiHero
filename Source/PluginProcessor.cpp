/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "hidapi/hidapi.h"
#include <iostream>
#include <bitset>

//==============================================================================
GuitarControllerAudioProcessor::GuitarControllerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    startTimer(100);
}

GuitarControllerAudioProcessor::~GuitarControllerAudioProcessor()
{
    stopTimer();
    if (hidDevice)
    {
        hid_close(hidDevice);
    }
    hid_exit();
}

//==============================================================================
const juce::String GuitarControllerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarControllerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarControllerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarControllerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarControllerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarControllerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarControllerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarControllerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarControllerAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarControllerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarControllerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DBG("Preparing to play...");
    int res = hid_init();
    if (res != 0)
    {
        DBG("Failed to initialize HID library!");
        return;
    }

    hidDevice = hid_open(0x12ba, 0x100, NULL);
    if (hidDevice)
    {
        DBG("Guitar connected!");
        isRunning.store(true);
        hidThread = std::thread(&GuitarControllerAudioProcessor::hidPollingThread, this);
    }
    else
    {
        DBG("Could not connect to guitar!");
        hid_exit();
    }
}


void GuitarControllerAudioProcessor::releaseResources()
{
    isRunning.store(false); // Signal the thread to stop
    if (hidThread.joinable())
        hidThread.join(); // Wait for the thread to finish

    if (hidDevice)
    {
        hid_close(hidDevice);
        hidDevice = nullptr;
    }
    hid_exit();
}


#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarControllerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GuitarControllerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);
    
    // Flush pending midi message (off events for changed notes)
    midiMessages.addEvents(pendingMidiMessages, 0, buffer.getNumSamples(), 0);
    pendingMidiMessages.clear();
    
    
    double scaleNotes[5] = {67, 65, 66, 68, 69}; // YGRBO ordering
    int strumMod = 0;

    switch (strumRaw)
    {
        case 15: strumMod = 0; break;
        case 4:  strumMod = 5; break;
        case 0:  strumMod = -5; break;
    }

    

    // Process HID button data
    if (newHIDDataAvailable.load())
    {
        std::lock_guard<std::mutex> lock(mutex); // Access shared data
        
        for (int i = 0; i < 5; ++i)
        {
            double noteToPlay = scaleMode ? (scaleNotes[i] + strumMod) : midiNotes[i];


            if (noteCurrentlyOn[i] != -1 && noteCurrentlyOn[i] != noteToPlay)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, noteCurrentlyOn[i]), 0);
                noteCurrentlyOn[i] = -1; // clear
            }

            if (newButtonStates[i] && noteCurrentlyOn[i] == -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOn(1, noteToPlay, 1.0f), 0);
                noteCurrentlyOn[i] = noteToPlay;
            }
            else if (!newButtonStates[i] && noteCurrentlyOn[i] != -1)
            {
                midiMessages.addEvent(juce::MidiMessage::noteOff(1, noteCurrentlyOn[i]), 0);
                noteCurrentlyOn[i] = -1;
            }

        }

        
        currentButtonStates = newButtonStates;
        
        int whammyToMidi = juce::jlimit(0, 16383, (255 - whammyRaw) * 64);
        DBG(whammyToMidi);
        
        midiMessages.addEvent(juce::MidiMessage::pitchWheel(1, whammyToMidi), 0);
        
        newHIDDataAvailable.store(false); // Reset flag
    }

    // Audio processing logic (if any)
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        // Process audio data...
    }
}




void GuitarControllerAudioProcessor::timerCallback()
{
    // read event
    // add to shared queue
}


//==============================================================================
bool GuitarControllerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarControllerAudioProcessor::createEditor()
{
    return new GuitarControllerAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarControllerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GuitarControllerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


void GuitarControllerAudioProcessor::hidPollingThread()
{
    while (isRunning.load())
    {
        if (hidDevice)
        {
            unsigned char localBuf[65];
            // Use a short timeout (e.g. 1 ms) so we don't block
            int res = hid_read_timeout(hidDevice, localBuf, sizeof(localBuf), 1);

            if (res > 0)
            {
                std::lock_guard<std::mutex> lock(mutex); // Lock shared data
                std::copy(std::begin(localBuf), std::end(localBuf), std::begin(buf));
                
                
                // PITCH BEND
                if (whammyRaw != buf[5]) {
                    whammyRaw = buf[5]; // 0–255
                    DBG("whammy raw: " + juce::String(whammyRaw));
                    newHIDDataAvailable.store(true);

                    
                }
                
                
                // Strum Bar
                if (strumRaw != buf[2]) {
                    strumRaw = buf[2];
                    DBG("strum raw: " + juce::String(strumRaw));
                    newHIDDataAvailable.store(true);

                    
                }
                
//                for (int i = 0; i < 5; i++) {
//                    DBG(juce::String(i) + ": " + juce::String(buf[i]));
//                }
                


                
                if (currentStatus != buf[0]) // Button state has changed
                {
                    currentStatus = buf[0];
                    newButtonStates = currentStatus;
                    //DBG(newButtonStates.to_string());
                    newHIDDataAvailable.store(true);
                }
            }
            else if (res < 0)
            {
                DBG("hid_read_timeout error");
                break; // exit thread if device error
            }

        }
        //std::this_thread::sleep_for(std::chrono::milliseconds()); // Polling interval
    }
}


void GuitarControllerAudioProcessor::updateMidiNote(int buttonIndex, double newMidiNote)
{
    if (buttonIndex >= 0 && buttonIndex < 5)
    {
        if (currentButtonStates[buttonIndex])
                {
                    auto noteOff = juce::MidiMessage::noteOff(1, midiNotes[buttonIndex]);
                    pendingMidiMessages.addEvent(noteOff, 0);
                }

        
        midiNotes[buttonIndex] = newMidiNote;
        
        if (currentButtonStates[buttonIndex]) {
            auto noteOn = juce::MidiMessage::noteOn(1, midiNotes[buttonIndex], 1.0f);
            pendingMidiMessages.addEvent(noteOn, 0);
        }


        
    }
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarControllerAudioProcessor();
}
