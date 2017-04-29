/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKJUCEAudioProcessor::ATKJUCEAudioProcessor()
:
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
inL(nullptr, 1, 0, false), inR(nullptr, 1, 0, false), outL(nullptr, 1, 0, false), outR(nullptr, 1, 0, false)
{
}

ATKJUCEAudioProcessor::~ATKJUCEAudioProcessor()
{
}

//==============================================================================
const String ATKJUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ATKJUCEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ATKJUCEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ATKJUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ATKJUCEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ATKJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ATKJUCEAudioProcessor::setCurrentProgram (int index)
{
}

const String ATKJUCEAudioProcessor::getProgramName (int index)
{
    return {};
}

void ATKJUCEAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ATKJUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ATKJUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ATKJUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void ATKJUCEAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  assert(totalNumInputChannels == totalNumOutputChannels);
  assert(totalNumOutputChannels == 2);
  
  inL.set_pointer(buffer.getReadPointer(0), buffer.getNumSamples());
  inR.set_pointer(buffer.getReadPointer(1), buffer.getNumSamples());
  outL.set_pointer(buffer.getWritePointer(0), buffer.getNumSamples());
  outR.set_pointer(buffer.getWritePointer(1), buffer.getNumSamples());
 
  outL.process(buffer.getNumSamples());
  outR.process(buffer.getNumSamples());
}

//==============================================================================
bool ATKJUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ATKJUCEAudioProcessor::createEditor()
{
    return new ATKJUCEAudioProcessorEditor (*this);
}

//==============================================================================
void ATKJUCEAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ATKJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ATKJUCEAudioProcessor();
}
