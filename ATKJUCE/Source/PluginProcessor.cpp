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
inL(nullptr, 1, 0, false), inR(nullptr, 1, 0, false), outL(nullptr, 1, 0, false), outR(nullptr, 1, 0, false), buffer_filter(nullptr, 1, 0, false)
{
  outL.set_input_port(0, &inL, 0);
  outR.set_input_port(0, &inR, 0);
  buffer_filter.set_input_port(0, &inL, 0);
  pipeline.add_filter(&outL);
  pipeline.add_filter(&buffer_filter);
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
	auto intsamplerate = std::lround(sampleRate);

	inL.set_input_sampling_rate(intsamplerate);
	inL.set_output_sampling_rate(intsamplerate);
	inR.set_input_sampling_rate(intsamplerate);
	inR.set_output_sampling_rate(intsamplerate);

	outL.set_input_sampling_rate(intsamplerate);
	outL.set_output_sampling_rate(intsamplerate);
	outR.set_input_sampling_rate(intsamplerate);
	outR.set_output_sampling_rate(intsamplerate);
  buffer_filter.set_input_sampling_rate(intsamplerate);
  buffer_filter.set_output_sampling_rate(intsamplerate);
  buffer_filter.set_pointer(fft_buffer.data(), fft_buffer.size());

  pipeline.set_input_sampling_rate(intsamplerate);

  if(intsamplerate > 48000)
  {
    fft_buffer.assign(1024*32*4, 0);
    slize_size = 1024*32;
  }
  else
  {
    fft_buffer.assign(1024*16*4, 0);
    slize_size = 1024*16;
  }
  current_buffer_index = 0;
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
  
  auto nb_samples = buffer.getNumSamples();

  assert(totalNumInputChannels == totalNumOutputChannels);
  assert(totalNumOutputChannels == 2);
  
  inL.set_pointer(buffer.getReadPointer(0), nb_samples);
  inR.set_pointer(buffer.getReadPointer(1), nb_samples);
  outL.set_pointer(buffer.getWritePointer(0), nb_samples);
  outR.set_pointer(buffer.getWritePointer(1), nb_samples);
 
  outR.process(nb_samples);

  auto size = std::min(nb_samples, slize_size * 4 - current_buffer_index);
  
  pipeline.process(size);
  
  current_buffer_index += size;
  if(current_buffer_index == slize_size * 4)
  {
    buffer_filter.set_pointer(fft_buffer.data(), fft_buffer.size());
    pipeline.process(nb_samples - size);
  }
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
