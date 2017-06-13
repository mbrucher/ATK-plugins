/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKColoredExpanderAudioProcessor::ATKColoredExpanderAudioProcessor()
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
  inFilter(nullptr, 1, 0, false), outFilter(nullptr, 1, 0, false), gainExpanderFilter(1, 256*1024), parameters(*this, nullptr), sampleRate(0), lastParameterSet(-1), old_rms(0), old_attack(0), old_release(0), old_threshold(-1), old_slope(-1), old_softness(-5), old_color(-1), old_quality(-1), old_maxreduc(1), old_makeup(-1), old_drywet(-1)
{
  powerFilter.set_input_port(0, &inFilter, 0);
  attackReleaseFilter.set_input_port(0, &powerFilter, 0);
  gainExpanderFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(0, &gainExpanderFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &volumeFilter, 0);
  drywetFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &drywetFilter, 0);
  
  parameters.createAndAddParameter("power", "Power", " ms", NormalisableRange<float>(0, 100), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack", "Attack", " ms", NormalisableRange<float>(1, 100), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release", "Release", " ms",  NormalisableRange<float>(1, 100), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold", "Threshold", " dB", NormalisableRange<float>(-40, 0), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope", "Slope", "", NormalisableRange<float>(1.5, 100), 2, nullptr, nullptr);
  parameters.createAndAddParameter("color", "Color", "", NormalisableRange<float>(-.5, .5), 0, nullptr, nullptr);
  parameters.createAndAddParameter("quality", "Quality", "", NormalisableRange<float>(0.01, .2), .1, nullptr, nullptr);
  parameters.createAndAddParameter("softness", "Softness", "", NormalisableRange<float>(-4, 0), -2, nullptr, nullptr);
  parameters.createAndAddParameter("maxreduc", "Maximum reduction", " dB", NormalisableRange<float>(-60, 0), -60, nullptr, nullptr);
  parameters.createAndAddParameter("makeup", "Makeup gain", " dB", NormalisableRange<float>(0, 40), 0, nullptr, nullptr);
  parameters.createAndAddParameter("drywet", "Dry/Wet", "", NormalisableRange<float>(0, 100), 100, nullptr, nullptr);
  
  parameters.state = ValueTree (Identifier ("ATKColoredExpander"));;
}

ATKColoredExpanderAudioProcessor::~ATKColoredExpanderAudioProcessor()
{
}

//==============================================================================
const String ATKColoredExpanderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ATKColoredExpanderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ATKColoredExpanderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ATKColoredExpanderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ATKColoredExpanderAudioProcessor::getNumPrograms()
{
    return 2;
}

int ATKColoredExpanderAudioProcessor::getCurrentProgram()
{
    return lastParameterSet;
}

void ATKColoredExpanderAudioProcessor::setCurrentProgram (int index)
{
  if(index != lastParameterSet)
  {
    lastParameterSet = index;
    if(index == 0)
    {
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter("power")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("attack")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("release")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("threshold")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("slope")) = 2;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("softness")) = .1;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("color")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("quality")) = .01;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("maxreduc")) = -60;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("makeup")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("drywet")) = 100;
    }
    else if (index == 1)
    {
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter("power")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("attack")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("release")) = 10;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("threshold")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("slope")) = 2;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("softness")) = .1;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("color")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("quality")) = .01;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("maxreduc")) = -60;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("makeup")) = 0;
      *static_cast<::juce::AudioParameterFloat*>(parameters.getParameter ("drywet")) = 50;
    }
  }
}

const String ATKColoredExpanderAudioProcessor::getProgramName (int index)
{
  if(index == 0)
  {
    return "Serial Compression";
  }
  else if(index == 1)
  {
    return "Parallel Compression";
  }
  return {};
}

void ATKColoredExpanderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ATKColoredExpanderAudioProcessor::prepareToPlay (double dbSampleRate, int samplesPerBlock)
{
	sampleRate = std::lround(dbSampleRate);
  
  if(sampleRate != inFilter.get_output_sampling_rate())
  {
    inFilter.set_input_sampling_rate(sampleRate);
    inFilter.set_output_sampling_rate(sampleRate);
    powerFilter.set_input_sampling_rate(sampleRate);
    powerFilter.set_output_sampling_rate(sampleRate);
    attackReleaseFilter.set_input_sampling_rate(sampleRate);
    attackReleaseFilter.set_output_sampling_rate(sampleRate);
    gainExpanderFilter.set_input_sampling_rate(sampleRate);
    gainExpanderFilter.set_output_sampling_rate(sampleRate);
    applyGainFilter.set_input_sampling_rate(sampleRate);
    applyGainFilter.set_output_sampling_rate(sampleRate);
    volumeFilter.set_input_sampling_rate(sampleRate);
    volumeFilter.set_output_sampling_rate(sampleRate);
    drywetFilter.set_input_sampling_rate(sampleRate);
    drywetFilter.set_output_sampling_rate(sampleRate);
    outFilter.set_input_sampling_rate(sampleRate);
    outFilter.set_output_sampling_rate(sampleRate);
    outFilter.set_input_sampling_rate(sampleRate);
    outFilter.set_output_sampling_rate(sampleRate);
    
    if (old_rms == 0)
    {
      powerFilter.set_memory(0);
    }
    else
    {
      powerFilter.set_memory(std::exp(-1e3 / (old_rms * sampleRate)));
    }
    attackReleaseFilter.set_release(std::exp(-1e3/(old_release * sampleRate))); // in ms
    attackReleaseFilter.set_attack(std::exp(-1e3/(old_attack * sampleRate))); // in ms
  }
  powerFilter.full_setup();
  attackReleaseFilter.full_setup();
}

void ATKColoredExpanderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ATKColoredExpanderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ATKColoredExpanderAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  if (*parameters.getRawParameterValue ("power") != old_rms)
  {
    old_rms = *parameters.getRawParameterValue ("power");
    if (old_rms == 0)
    {
      powerFilter.set_memory(0);
    }
    else
    {
      powerFilter.set_memory(std::exp(-1e3 / (old_rms * sampleRate)));
    }
  }
  if(*parameters.getRawParameterValue ("release") != old_release)
  {
    old_release = *parameters.getRawParameterValue ("release");
    attackReleaseFilter.set_release(std::exp(-1e3/(old_release * sampleRate))); // in ms
  }
  if(*parameters.getRawParameterValue ("attack") != old_attack)
  {
    old_attack = *parameters.getRawParameterValue ("attack");
    attackReleaseFilter.set_attack(std::exp(-1e3/(old_attack * sampleRate))); // in ms
  }
  if(*parameters.getRawParameterValue ("threshold") != old_threshold)
  {
    old_threshold = *parameters.getRawParameterValue ("threshold");
    gainExpanderFilter.set_threshold(std::pow(10, old_threshold / 10));
  }
  if(*parameters.getRawParameterValue ("slope") != old_slope)
  {
    old_slope = *parameters.getRawParameterValue ("slope");
    gainExpanderFilter.set_ratio(old_slope);
  }
  if(*parameters.getRawParameterValue ("softness") != old_softness)
  {
    old_softness = *parameters.getRawParameterValue ("softness");
    gainExpanderFilter.set_softness(std::pow(10, old_softness));
  }
  if(*parameters.getRawParameterValue ("color") != old_color)
  {
    old_color = *parameters.getRawParameterValue ("color");
    gainExpanderFilter.set_color(old_color);
  }
  if(*parameters.getRawParameterValue ("quality") != old_quality)
  {
    old_quality = *parameters.getRawParameterValue ("quality");
    gainExpanderFilter.set_quality(old_quality);
  }
  if(*parameters.getRawParameterValue ("maxreduc") != old_maxreduc)
  {
    old_maxreduc = *parameters.getRawParameterValue ("maxreduc");
    gainExpanderFilter.set_max_reduction_db(old_maxreduc);
  }
  if(*parameters.getRawParameterValue ("makeup") != old_makeup)
  {
    old_makeup = *parameters.getRawParameterValue ("makeup");
    volumeFilter.set_volume_db(old_makeup);
  }
  
  if(*parameters.getRawParameterValue ("drywet") != old_drywet)
  {
    old_drywet = *parameters.getRawParameterValue ("drywet");
    drywetFilter.set_dry(old_drywet / 100);
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
bool ATKColoredExpanderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ATKColoredExpanderAudioProcessor::createEditor()
{
    return new ATKColoredExpanderAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ATKColoredExpanderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  MemoryOutputStream store(destData, true);
  store.writeInt(0); // version ID
  store.writeFloat(*parameters.getRawParameterValue ("power"));
  store.writeFloat(*parameters.getRawParameterValue ("attack"));
  store.writeFloat(*parameters.getRawParameterValue ("release"));
  store.writeFloat(*parameters.getRawParameterValue ("threshold"));
  store.writeFloat(*parameters.getRawParameterValue ("slope"));
  store.writeFloat(*parameters.getRawParameterValue ("softness"));
  store.writeFloat(*parameters.getRawParameterValue ("color"));
  store.writeFloat(*parameters.getRawParameterValue ("quality"));
  store.writeFloat(*parameters.getRawParameterValue ("maxreduc"));
  store.writeFloat(*parameters.getRawParameterValue ("makeup"));
  store.writeFloat(*parameters.getRawParameterValue ("drywet"));
}

void ATKColoredExpanderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  if(sizeInBytes != 12 * 4)
    return;
  MemoryInputStream store(data, static_cast<size_t> (sizeInBytes), false);
  int version = store.readInt(); // version ID
  *parameters.getRawParameterValue ("power") = store.readFloat();
  *parameters.getRawParameterValue ("attack") = store.readFloat();
  *parameters.getRawParameterValue ("release") = store.readFloat();
  *parameters.getRawParameterValue ("threshold") = store.readFloat();
  *parameters.getRawParameterValue ("slope") = store.readFloat();
  *parameters.getRawParameterValue ("softness") = store.readFloat();
  *parameters.getRawParameterValue ("color") = store.readFloat();
  *parameters.getRawParameterValue ("quality") = store.readFloat();
  *parameters.getRawParameterValue ("maxreduc") = store.readFloat();
  *parameters.getRawParameterValue ("makeup") = store.readFloat();
  *parameters.getRawParameterValue ("drywet") = store.readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ATKColoredExpanderAudioProcessor();
}
