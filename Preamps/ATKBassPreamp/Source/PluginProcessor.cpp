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
  inFilter(nullptr, 1, 0, false), overdriveFilter(ATK::Triode2Filter<float, ATK::DempwolfTriodeFunction<float>>::build_standard_filter(100e3, 1e6, 1e6, 820, 300, 22e-9, 250e-6)), toneFilter(ATK::ToneStackCoefficients<float>::buildBassmanStack()), outFilter(nullptr, 1, 0, false), parameters(*this, nullptr)
{  
  levelFilter.set_input_port(0, &inFilter, 0);
  oversamplingFilter.set_input_port(0, &levelFilter, 0);
  overdriveFilter.set_input_port(0, &oversamplingFilter, 0);
  lowpassFilter.set_input_port(0, &overdriveFilter, 0);
  decimationFilter.set_input_port(0, &lowpassFilter, 0);
  toneFilter.set_input_port(0, &decimationFilter, 0);
  volumeFilter.set_input_port(0, &toneFilter, 0);
  dryWetFilter.set_input_port(0, &volumeFilter, 0);
  dryWetFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &dryWetFilter, 0);

  levelFilter.set_volume_db(originGain);
  volumeFilter.set_volume(-1);
  dryWetFilter.set_dry(1);
  lowpassFilter.set_order(4);
  lowpassFilter.set_cut_frequency(20000);
  
  parameters.createAndAddParameter("gain", "Gain", " dB", NormalisableRange<float>(minGain, maxGain), originGain, nullptr, nullptr);
  parameters.createAndAddParameter("bass", "Bass", "", NormalisableRange<float>(-1.0f, 1.0f), 0.f, nullptr, nullptr);
  parameters.createAndAddParameter("medium", "Medium", "",  NormalisableRange<float>(-1.0f, 1.0f), 0.f, nullptr, nullptr);
  parameters.createAndAddParameter("high", "High", "", NormalisableRange<float>(-1.0f, 1.0f), 0.f, nullptr, nullptr);
  parameters.createAndAddParameter("volume", "Volume", " dB", NormalisableRange<float>(minVolume, maxVolume), (minVolume + maxVolume) / 2, nullptr, nullptr);
  parameters.createAndAddParameter("drywet", "Dry/Wet", " dB", NormalisableRange<float>(0.0f, 100.0f), 100.0f, nullptr, nullptr);

  parameters.state = ValueTree (Identifier ("ATKBassPreamp"));;
  
  old_gain = *parameters.getRawParameterValue ("gain");
  old_bass = *parameters.getRawParameterValue ("bass");
  old_medium = *parameters.getRawParameterValue ("medium");
  old_high = *parameters.getRawParameterValue ("high");
  old_volume = *parameters.getRawParameterValue ("volume");
  old_drywet = *parameters.getRawParameterValue ("drywet");
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
  dryWetFilter.set_input_sampling_rate(intsamplerate);
  dryWetFilter.set_output_sampling_rate(intsamplerate);
  outFilter.set_input_sampling_rate(intsamplerate);
	outFilter.set_output_sampling_rate(intsamplerate);
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
  if (*parameters.getRawParameterValue ("gain") != old_gain)
  {
    old_gain = *parameters.getRawParameterValue ("gain");
    levelFilter.set_volume_db(old_gain);
  }
  if(*parameters.getRawParameterValue ("bass") != old_bass)
  {
    old_bass = *parameters.getRawParameterValue ("bass");
    toneFilter.set_low((old_bass + 1) / 2);
  }
  if(*parameters.getRawParameterValue ("medium") != old_medium)
  {
    old_medium = *parameters.getRawParameterValue ("medium");
    toneFilter.set_middle((old_medium + 1) / 2);
  }
  if(*parameters.getRawParameterValue ("high") != old_high)
  {
    old_high = *parameters.getRawParameterValue ("high");
    toneFilter.set_high((old_high + 1) / 2);
  }
  if (*parameters.getRawParameterValue ("volume") != old_volume)
  {
    old_volume = *parameters.getRawParameterValue ("volume");
    volumeFilter.set_volume(-std::pow(10., old_volume / 20));
  }

  if(*parameters.getRawParameterValue ("drywet") != old_drywet)
  {
    old_drywet = *parameters.getRawParameterValue ("drywet");
    dryWetFilter.set_dry(old_drywet / 100);
  }
  
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
    return new ATKBassPreampAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ATKBassPreampAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  MemoryOutputStream store(destData, true);
  store.writeInt(0); // version ID
  store.writeFloat(*parameters.getRawParameterValue ("gain"));
  store.writeFloat(*parameters.getRawParameterValue ("bass"));
  store.writeFloat(*parameters.getRawParameterValue ("medium"));
  store.writeFloat(*parameters.getRawParameterValue ("high"));
  store.writeFloat(*parameters.getRawParameterValue ("volume"));
  store.writeFloat(*parameters.getRawParameterValue ("drywet"));
}

void ATKBassPreampAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  MemoryInputStream store(data, static_cast<size_t> (sizeInBytes), false);
  int version = store.readInt(); // version ID
  *parameters.getRawParameterValue ("gain") = store.readFloat();
  *parameters.getRawParameterValue ("bass") = store.readFloat();
  *parameters.getRawParameterValue ("medium") = store.readFloat();
  *parameters.getRawParameterValue ("high") = store.readFloat();
  *parameters.getRawParameterValue ("volume") = store.readFloat();
  *parameters.getRawParameterValue ("drywet") = store.readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ATKBassPreampAudioProcessor();
}
