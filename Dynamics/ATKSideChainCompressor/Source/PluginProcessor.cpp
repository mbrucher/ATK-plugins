/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <string>

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKSideChainCompressorAudioProcessor::ATKSideChainCompressorAudioProcessor()
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
inLFilter(nullptr, 1, 0, false), inRFilter(nullptr, 1, 0, false), inSideChainLFilter(nullptr, 1, 0, false), inSideChainRFilter(nullptr, 1, 0, false),
volumesplitFilter(4), applyGainFilter(2), volumemergeFilter(2), drywetFilter(2), outLFilter(nullptr, 1, 0, false), outRFilter(nullptr, 1, 0, false),
parameters(*this, nullptr), lastParameterSet(-1), sidechain(false),
old_rms1(0), old_attack1(0), old_release1(0), old_threshold1(-1), old_slope1(-1), old_softness1(-5), old_makeup1(-1), old_drywet1(-1),
old_rms2(0), old_attack2(0), old_release2(0), old_threshold2(-1), old_slope2(-1), old_softness2(-5), old_makeup2(-1), old_drywet2(-1)
{
  volumesplitFilter.set_volume(std::sqrt(.5));
  volumemergeFilter.set_volume(std::sqrt(.5));
  endpoint.add_filter(&outLFilter);
  endpoint.add_filter(&outRFilter);
  
  powerFilter1.set_input_port(0, &inSideChainLFilter, 0);
  gainColoredCompressorFilter1.set_input_port(0, &powerFilter1, 0);
  attackReleaseFilter1.set_input_port(0, &gainColoredCompressorFilter1, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter1, 0);
  applyGainFilter.set_input_port(1, &inLFilter, 0);
  makeupFilter1.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &makeupFilter1, 0);
  drywetFilter.set_input_port(1, &inLFilter, 0);
  outLFilter.set_input_port(0, &drywetFilter, 0);
  
  powerFilter2.set_input_port(0, &inSideChainRFilter, 0);
  gainColoredCompressorFilter2.set_input_port(0, &powerFilter2, 0);
  attackReleaseFilter2.set_input_port(0, &gainColoredCompressorFilter2, 0);
  applyGainFilter.set_input_port(2, &attackReleaseFilter2, 0);
  applyGainFilter.set_input_port(3, &inRFilter, 0);
  makeupFilter2.set_input_port(0, &applyGainFilter, 1);
  drywetFilter.set_input_port(2, &makeupFilter2, 0);
  drywetFilter.set_input_port(3, &inRFilter, 0);
  outRFilter.set_input_port(0, &drywetFilter, 1);
  
  middlesidesplitFilter.set_input_port(0, &inLFilter, 0);
  middlesidesplitFilter.set_input_port(1, &inRFilter, 0);
  sidechainmiddlesidesplitFilter.set_input_port(0, &inSideChainLFilter, 0);
  sidechainmiddlesidesplitFilter.set_input_port(1, &inSideChainRFilter, 0);
  volumesplitFilter.set_input_port(0, &middlesidesplitFilter, 0);
  volumesplitFilter.set_input_port(1, &middlesidesplitFilter, 1);
  volumesplitFilter.set_input_port(2, &sidechainmiddlesidesplitFilter, 0);
  volumesplitFilter.set_input_port(3, &sidechainmiddlesidesplitFilter, 1);
  middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
  middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
  volumemergeFilter.set_input_port(0, &middlesidemergeFilter, 0);
  volumemergeFilter.set_input_port(1, &middlesidemergeFilter, 1);
  sumFilter.set_input_port(0, &powerFilter1, 0);
  sumFilter.set_input_port(1, &powerFilter2, 0);

  
  parameters.createAndAddParameter("power1", "Power ch1", " ms", NormalisableRange<float>(0, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack1", "Attack ch1", " ms", NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release1", "Release ch1", " ms",  NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold1", "Threshold", " dB", NormalisableRange<float>(-40, 0), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope1", "Slope ch1", "", NormalisableRange<float>(0.1, 10, .01, 0.3), 2, nullptr, nullptr);
  parameters.createAndAddParameter("softness1", "Softness ch1", "", NormalisableRange<float>(-4, 0), -2, nullptr, nullptr);
  parameters.createAndAddParameter("makeup1", "Makeup gain ch1", " dB", NormalisableRange<float>(-20, 20), 0, nullptr, nullptr);
  parameters.createAndAddParameter("drywet1", "Dry/Wet ch1", "", NormalisableRange<float>(0, 100), 100, nullptr, nullptr);
  
  parameters.createAndAddParameter("power2", "Power ch2", " ms", NormalisableRange<float>(0, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack2", "Attack ch2", " ms", NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release2", "Release ch2", " ms",  NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold2", "Threshold ch2", " dB", NormalisableRange<float>(-40, 0), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope2", "Slope ch2", "", NormalisableRange<float>(0.1, 10, .01, 0.3), 2, nullptr, nullptr);
  parameters.createAndAddParameter("softness2", "Softness ch2", "", NormalisableRange<float>(-4, 0), -2, nullptr, nullptr);
  parameters.createAndAddParameter("makeup2", "Makeup gain ch2", " dB", NormalisableRange<float>(-20, 20), 0, nullptr, nullptr);
  parameters.createAndAddParameter("drywet2", "Dry/Wet ch2", "", NormalisableRange<float>(0, 100), 100, nullptr, nullptr);
  
  parameters.state = ValueTree (Identifier ("ATKSideChainCompressor"));
}

ATKSideChainCompressorAudioProcessor::~ATKSideChainCompressorAudioProcessor()
{
}

//==============================================================================
const String ATKSideChainCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ATKSideChainCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ATKSideChainCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double ATKSideChainCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ATKSideChainCompressorAudioProcessor::getNumPrograms()
{
    return 2;
}

int ATKSideChainCompressorAudioProcessor::getCurrentProgram()
{
    return lastParameterSet;
}

void ATKSideChainCompressorAudioProcessor::setCurrentProgram (int index)
{
  if(index != lastParameterSet)
  {
    lastParameterSet = index;
    if(index == 0)
    {
      const char* preset0 = "<ATKSideChainCompressor><PARAM id=\"power\" value=\"10\" /><PARAM id=\"attack\" value=\"10\" /><PARAM id=\"release\" value=\"10\" /> <PARAM id=\"threshold\" value=\"0\" /><PARAM id=\"slope\" value=\"2\" /><PARAM id=\"softness\" value=\"-2\" /><PARAM id=\"makeup\" value=\"0\" /><PARAM id=\"drywet\" value=\"100\" /></ATKSideChainCompressor>";
      XmlDocument doc(preset0);

      auto el = doc.getDocumentElement();
      parameters.state = ValueTree::fromXml(*el);
      delete el;
    }
    else if (index == 1)
    {
      const char* preset1 = "<ATKSideChainCompressor><PARAM id=\"power\" value=\"10\" /><PARAM id=\"attack\" value=\"10\" /><PARAM id=\"release\" value=\"10\" /> <PARAM id=\"threshold\" value=\"0\" /><PARAM id=\"slope\" value=\"2\" /><PARAM id=\"softness\" value=\"-2\" /><PARAM id=\"makeup\" value=\"0\" /><PARAM id=\"drywet\" value=\"50\" /></ATKSideChainCompressor>";
      XmlDocument doc(preset1);

      auto el = doc.getDocumentElement();
      parameters.state = ValueTree::fromXml(*el);
      delete el;
    }
  }
}

const String ATKSideChainCompressorAudioProcessor::getProgramName (int index)
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

void ATKSideChainCompressorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ATKSideChainCompressorAudioProcessor::prepareToPlay (double dbSampleRate, int samplesPerBlock)
{
	auto sampling_rate = std::lround(dbSampleRate);
  
  if (sampling_rate != endpoint.get_input_sampling_rate())
  {
    inLFilter.set_input_sampling_rate(sampling_rate);
    inLFilter.set_output_sampling_rate(sampling_rate);
    inRFilter.set_input_sampling_rate(sampling_rate);
    inRFilter.set_output_sampling_rate(sampling_rate);
    inSideChainLFilter.set_input_sampling_rate(sampling_rate);
    inSideChainLFilter.set_output_sampling_rate(sampling_rate);
    inSideChainRFilter.set_input_sampling_rate(sampling_rate);
    inSideChainRFilter.set_output_sampling_rate(sampling_rate);
    outLFilter.set_input_sampling_rate(sampling_rate);
    outLFilter.set_output_sampling_rate(sampling_rate);
    outRFilter.set_input_sampling_rate(sampling_rate);
    outRFilter.set_output_sampling_rate(sampling_rate);
    
    middlesidesplitFilter.set_input_sampling_rate(sampling_rate);
    middlesidesplitFilter.set_output_sampling_rate(sampling_rate);
    sidechainmiddlesidesplitFilter.set_input_sampling_rate(sampling_rate);
    sidechainmiddlesidesplitFilter.set_output_sampling_rate(sampling_rate);
    volumesplitFilter.set_input_sampling_rate(sampling_rate);
    volumesplitFilter.set_output_sampling_rate(sampling_rate);
    middlesidemergeFilter.set_input_sampling_rate(sampling_rate);
    middlesidemergeFilter.set_output_sampling_rate(sampling_rate);
    volumemergeFilter.set_input_sampling_rate(sampling_rate);
    volumemergeFilter.set_output_sampling_rate(sampling_rate);
    sumFilter.set_input_sampling_rate(sampling_rate);
    sumFilter.set_output_sampling_rate(sampling_rate);
    
    powerFilter1.set_input_sampling_rate(sampling_rate);
    powerFilter1.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter1.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter1.set_output_sampling_rate(sampling_rate);
    gainColoredCompressorFilter1.set_input_sampling_rate(sampling_rate);
    gainColoredCompressorFilter1.set_output_sampling_rate(sampling_rate);
    makeupFilter1.set_input_sampling_rate(sampling_rate);
    makeupFilter1.set_output_sampling_rate(sampling_rate);
    
    powerFilter2.set_input_sampling_rate(sampling_rate);
    powerFilter2.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_output_sampling_rate(sampling_rate);
    gainColoredCompressorFilter2.set_input_sampling_rate(sampling_rate);
    gainColoredCompressorFilter2.set_output_sampling_rate(sampling_rate);
    makeupFilter2.set_input_sampling_rate(sampling_rate);
    makeupFilter2.set_output_sampling_rate(sampling_rate);
    
    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    drywetFilter.set_input_sampling_rate(sampling_rate);
    drywetFilter.set_output_sampling_rate(sampling_rate);
    endpoint.set_input_sampling_rate(sampling_rate);
    endpoint.set_output_sampling_rate(sampling_rate);
    
    auto power = GetParam(kPower1)->Value();
    if (power == 0)
    {
      powerFilter1.set_memory(0);
    }
    else
    {
      powerFilter1.set_memory(std::exp(-1e3 / (power * sampling_rate)));
    }
    power = GetParam(kPower2)->Value();
    if (power == 0)
    {
      powerFilter2.set_memory(0);
    }
    else
    {
      powerFilter2.set_memory(std::exp(-1e3 / (power * sampling_rate)));
    }
    
    attackReleaseFilter1.set_release(std::exp(-1 / (GetParam(kAttack1)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter1.set_attack(std::exp(-1 / (GetParam(kRelease1)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter2.set_release(std::exp(-1 / (GetParam(kAttack2)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter2.set_attack(std::exp(-1 / (GetParam(kRelease2)->Value() * 1e-3 * sampling_rate))); // in ms
  }
  powerFilter1.full_setup();
  powerFilter2.full_setup();
  attackReleaseFilter1.full_setup();
  attackReleaseFilter2.full_setup();
  
  endpoint.dryrun(samplesPerBlock);
}

void ATKSideChainCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ATKSideChainCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

namespace {
  void check_power(const std::string_view& var, float& old_value, ATK::PowerFilter<double>& filter)
  {
    
  }
}

void ATKSideChainCompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
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

  assert(totalNumInputChannels == 2 || totalNumInputChannels == 4);
  assert(totalNumOutputChannels == 2);
  
  if (sidechain && totalNumInputChannels == 4)
  {
    inSideChainLFilter.set_pointer(inputs[2], nFrames);
  }
  else
  {
    inSideChainLFilter.set_pointer(inputs[0], nFrames);
  }
  if (sidechain && totalNumInputChannels == 4)
  {
    inSideChainRFilter.set_pointer(inputs[3], nFrames);
  }
  else
  {
    inSideChainRFilter.set_pointer(inputs[1], nFrames);
  }
  inLFilter.set_pointer(inputs[0], nFrames);
  outLFilter.set_pointer(outputs[0], nFrames);
  inRFilter.set_pointer(inputs[1], nFrames);
  outRFilter.set_pointer(outputs[1], nFrames);
  endpoint.process(nFrames);
}

//==============================================================================
bool ATKSideChainCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ATKSideChainCompressorAudioProcessor::createEditor()
{
    return new ATKSideChainCompressorAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void ATKSideChainCompressorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  MemoryOutputStream store(destData, true);
  store.writeInt(0); // version ID
  auto str = parameters.state.toXmlString();
  store.writeString(str);
}

void ATKSideChainCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new ATKSideChainCompressorAudioProcessor();
}
