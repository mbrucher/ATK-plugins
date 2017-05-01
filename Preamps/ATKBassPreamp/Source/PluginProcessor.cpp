/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKBassPreampAudioProcessor::ATKBassPreampAudioProcessor()
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
  inFilter(nullptr, 1, 0, false), overdriveFilter(ATK::Triode2Filter<double, ATK::DempwolfTriodeFunction<double>>::build_standard_filter()), outFilter(nullptr, 1, 0, false)
{  
  levelFilter.set_input_port(0, &inFilter, 0);
  oversamplingFilter.set_input_port(0, &levelFilter, 0);
  overdriveFilter.set_input_port(0, &oversamplingFilter, 0);
  lowpassFilter.set_input_port(0, &overdriveFilter, 0);
  decimationFilter.set_input_port(0, &lowpassFilter, 0);
  toneFilter.set_input_port(0, &decimationFilter, 0);
  volumeFilter.set_input_port(0, &toneFilter, 0);
  outFilter.set_input_port(0, &inFilter, 0);

  levelFilter.set_volume(1);
  volumeFilter.set_volume(1);

  addParameter (level = new AudioParameterFloat ("level", "Level", 0.0f, 1.0f, 1.0f));
  addParameter (bass = new AudioParameterFloat ("bass", "Bass", 0.0f, 1.0f, 1.0f));
  addParameter (medium = new AudioParameterFloat ("medium", "Medium", 0.0f, 1.0f, 1.0f));
  addParameter (high = new AudioParameterFloat ("high", "High", 0.0f, 1.0f, 1.0f));
  addParameter (gain = new AudioParameterFloat ("gain", "Gain", 0.0f, 1.0f, 1.0f));
}

ATKBassPreampAudioProcessor::~ATKBassPreampAudioProcessor()
{
}

//==============================================================================
const String ATKBassPreampAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ATKBassPreampAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ATKBassPreampAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ATKBassPreampAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ATKBassPreampAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ATKBassPreampAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ATKBassPreampAudioProcessor::setCurrentProgram (int index)
{
}

const String ATKBassPreampAudioProcessor::getProgramName (int index)
{
    return {};
}

void ATKBassPreampAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ATKBassPreampAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	auto intsamplerate = std::lround(sampleRate);

  inFilter.set_input_sampling_rate(intsamplerate);
  inFilter.set_output_sampling_rate(intsamplerate);
  levelFilter.set_input_sampling_rate(intsamplerate);
  levelFilter.set_output_sampling_rate(intsamplerate);
  oversamplingFilter.set_input_sampling_rate(intsamplerate);
  oversamplingFilter.set_output_sampling_rate(intsamplerate * 4);
  overdriveFilter.set_input_sampling_rate(intsamplerate * 4);
  overdriveFilter.set_output_sampling_rate(intsamplerate * 4);
  lowpassFilter.set_input_sampling_rate(intsamplerate * 4);
  lowpassFilter.set_output_sampling_rate(intsamplerate * 4);
  decimationFilter.set_input_sampling_rate(intsamplerate * 4);
  decimationFilter.set_output_sampling_rate(intsamplerate);
  toneFilter.set_input_sampling_rate(intsamplerate);
  toneFilter.set_output_sampling_rate(intsamplerate);
  volumeFilter.set_input_sampling_rate(intsamplerate);
  volumeFilter.set_output_sampling_rate(intsamplerate);
  outFilter.set_input_sampling_rate(intsamplerate);
	outFilter.set_output_sampling_rate(intsamplerate);
  
  lowpassFilter.set_cut_frequency(20000);
}

void ATKBassPreampAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ATKBassPreampAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ATKBassPreampAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  const int totalNumInputChannels  = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();

  assert(totalNumInputChannels == totalNumOutputChannels);
  assert(totalNumOutputChannels == 1);
  
  inFilter.set_pointer(buffer.getReadPointer(0), buffer.getNumSamples());
  outFilter.set_pointer(buffer.getWritePointer(0), buffer.getNumSamples());
 
  outFilter.process(buffer.getNumSamples());
}

//==============================================================================
bool ATKBassPreampAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ATKBassPreampAudioProcessor::createEditor()
{
    return new ATKBassPreampAudioProcessorEditor (*this);
}

//==============================================================================
void ATKBassPreampAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ATKBassPreampAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ATK::VolumeFilter<double>& ATKBassPreampAudioProcessor::get_level_filter()
{
  return levelFilter;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ATKBassPreampAudioProcessor();
}
