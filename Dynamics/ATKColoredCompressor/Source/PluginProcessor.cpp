/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKColoredCompressorAudioProcessor::ATKColoredCompressorAudioProcessor()
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
  inFilter(nullptr, 1, 0, false), outFilter(nullptr, 1, 0, false), gainCompressorFilter(1, 256*1024), parameters(*this, nullptr), sampleRate(0), lastParameterSet(-1), old_rms(0), old_attack(0), old_release(0), old_threshold(-1), old_slope(-1), old_softness(-5), old_color(-1), old_quality(-1), old_makeup(-1), old_drywet(-1)
{
  powerFilter.set_input_port(0, &inFilter, 0);
  attackReleaseFilter.set_input_port(0, &powerFilter, 0);
  gainCompressorFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(0, &gainCompressorFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &volumeFilter, 0);
  drywetFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &drywetFilter, 0);
  
  parameters.createAndAddParameter("power", "Power", " ms", NormalisableRange<float>(0, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack", "Attack", " ms", NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release", "Release", " ms",  NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold", "Threshold", " dB", NormalisableRange<float>(-40, 0), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope", "Slope", "", NormalisableRange<float>(1.5, 100, 1, 0.3), 2, nullptr, nullptr);
  parameters.createAndAddParameter("color", "Color", "", NormalisableRange<float>(-.5, .5), 0, nullptr, nullptr);
  parameters.createAndAddParameter("quality", "Quality", "", NormalisableRange<float>(0.01, .2, 1, 0.3), .1, nullptr, nullptr);
  parameters.createAndAddParameter("softness", "Softness", "", NormalisableRange<float>(-4, 0), -2, nullptr, nullptr);
  parameters.createAndAddParameter("makeup", "Makeup gain", " dB", NormalisableRange<float>(0, 40), 0, nullptr, nullptr);
  parameters.createAndAddParameter("drywet", "Dry/Wet", "", NormalisableRange<float>(0, 100), 100, nullptr, nullptr);
  
  parameters.state = ValueTree (Identifier ("ATKColoredCompressor"));
}

ATKColoredCompressorAudioProcessor::~ATKColoredCompressorAudioProcessor()
{
}

//==============================================================================
const String ATKColoredCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ATKColoredCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ATKColoredCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ATKColoredCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ATKColoredCompressorAudioProcessor::getNumPrograms()
{
    return 2;
}

int ATKColoredCompressorAudioProcessor::getCurrentProgram()
{
    return lastParameterSet;
}

void ATKColoredCompressorAudioProcessor::setCurrentProgram (int index)
{
  if(index != lastParameterSet)
  {
    lastParameterSet = index;
    if(index == 0)
    {
      const char* preset0 = "<ATKColoredCompressor><PARAM id=\"power\" value=\"10\" /><PARAM id=\"attack\" value=\"10\" /><PARAM id=\"release\" value=\"10\" /> <PARAM id=\"threshold\" value=\"0\" /><PARAM id=\"slope\" value=\"2\" /><PARAM id=\"color\" value = \"0\" /><PARAM id=\"quality\" value=\"0.10000000149011611938\" /><PARAM id=\"softness\" value=\"-2\" /><PARAM id=\"makeup\" value=\"0\" /><PARAM id=\"drywet\" value=\"100\" /></ATKColoredCompressor>";
      XmlDocument doc(preset0);

      parameters.state = ValueTree::fromXml(*doc.getDocumentElement());
    }
    else if (index == 1)
    {
      const char* preset1 = "<ATKColoredCompressor><PARAM id=\"power\" value=\"10\" /><PARAM id=\"attack\" value=\"10\" /><PARAM id=\"release\" value=\"10\" /> <PARAM id=\"threshold\" value=\"0\" /><PARAM id=\"slope\" value=\"2\" /><PARAM id=\"color\" value = \"0\" /><PARAM id=\"quality\" value=\"0.10000000149011611938\" /><PARAM id=\"softness\" value=\"-2\" /><PARAM id=\"makeup\" value=\"0\" /><PARAM id=\"drywet\" value=\"50\" /></ATKColoredCompressor>";
      XmlDocument doc(preset1);

      parameters.state = ValueTree::fromXml(*doc.getDocumentElement());
    }
  }
}

const String ATKColoredCompressorAudioProcessor::getProgramName (int index)
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

void ATKColoredCompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ATKColoredCompressorAudioProcessor::prepareToPlay (double dbSampleRate, int samplesPerBlock)
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
    gainCompressorFilter.set_input_sampling_rate(sampleRate);
    gainCompressorFilter.set_output_sampling_rate(sampleRate);
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

void ATKColoredCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ATKColoredCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ATKColoredCompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
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
    gainCompressorFilter.set_threshold(std::pow(10, old_threshold / 10));
  }
  if(*parameters.getRawParameterValue ("slope") != old_slope)
  {
    old_slope = *parameters.getRawParameterValue ("slope");
    gainCompressorFilter.set_ratio(old_slope);
  }
  if(*parameters.getRawParameterValue ("softness") != old_softness)
  {
    old_softness = *parameters.getRawParameterValue ("softness");
    gainCompressorFilter.set_softness(std::pow(10, old_softness));
  }
  if(*parameters.getRawParameterValue ("color") != old_color)
  {
    old_color = *parameters.getRawParameterValue ("color");
    gainCompressorFilter.set_color(old_color);
  }
  if(*parameters.getRawParameterValue ("quality") != old_quality)
  {
    old_quality = *parameters.getRawParameterValue ("quality");
    gainCompressorFilter.set_quality(old_quality);
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
bool ATKColoredCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ATKColoredCompressorAudioProcessor::createEditor()
{
    return new ATKColoredCompressorAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ATKColoredCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  MemoryOutputStream store(destData, true);
  store.writeInt(0); // version ID
  auto str = parameters.state.toXmlString();
  store.writeString(str);
}

void ATKColoredCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  MemoryInputStream store(data, static_cast<size_t> (sizeInBytes), false);
  int version = store.readInt(); // version ID
  std::unique_ptr<::juce::XmlElement> xml(::juce::XmlDocument::parse(store.readString()));
  if(xml)
    parameters.state = ValueTree::fromXml(*xml);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ATKColoredCompressorAudioProcessor();
}
