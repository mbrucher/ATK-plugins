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
parameters(*this, nullptr), sampleRate(0), lastParameterSet(-1),
old_link(false), old_middleside(false), old_enableCh1(true), old_enableCh2(true),
old_rms1(0), old_attack1(0), old_release1(0), old_threshold1(-1), old_slope1(-1), old_softness1(-5), old_makeup1(-1),
old_rms2(0), old_attack2(0), old_release2(0), old_threshold2(-1), old_slope2(-1), old_softness2(-5), old_makeup2(-1),
old_drywet(-1)
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

  parameters.createAndAddParameter("middleside", "Middle/Side", "", NormalisableRange<float>(0, 1, 1), 0, nullptr, nullptr);
  parameters.createAndAddParameter("link", "Link channels", "", NormalisableRange<float>(0, 1, 1), 0, nullptr, nullptr);
  parameters.createAndAddParameter("enable1", "Enable ch1", "", NormalisableRange<float>(0, 1, 1), 1, nullptr, nullptr);
  parameters.createAndAddParameter("enable2", "Enable ch2", "", NormalisableRange<float>(0, 1, 1), 1, nullptr, nullptr);
  
  parameters.createAndAddParameter("power1", "Power ch1", " ms", NormalisableRange<float>(0, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack1", "Attack ch1", " ms", NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release1", "Release ch1", " ms",  NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold1", "Threshold", " dB", NormalisableRange<float>(-40, 0, 0.1), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope1", "Slope ch1", "", NormalisableRange<float>(0.1, 10, .01, 0.3), 2, nullptr, nullptr);
  parameters.createAndAddParameter("softness1", "Softness ch1", "", NormalisableRange<float>(-4, 0, 0.1), -2, nullptr, nullptr);
  parameters.createAndAddParameter("makeup1", "Makeup gain ch1", " dB", NormalisableRange<float>(-20, 20, 0.1), 0, nullptr, nullptr);
  
  parameters.createAndAddParameter("power2", "Power ch2", " ms", NormalisableRange<float>(0, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("attack2", "Attack ch2", " ms", NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("release2", "Release ch2", " ms",  NormalisableRange<float>(1, 100, 1, 0.3), 10, nullptr, nullptr);
  parameters.createAndAddParameter("threshold2", "Threshold ch2", " dB", NormalisableRange<float>(-40, 0, 0.1), 0, nullptr, nullptr);
  parameters.createAndAddParameter("slope2", "Slope ch2", "", NormalisableRange<float>(0.1, 10, .01, 0.3), 2, nullptr, nullptr);
  parameters.createAndAddParameter("softness2", "Softness ch2", "", NormalisableRange<float>(-4, 0, 0.1), -2, nullptr, nullptr);
  parameters.createAndAddParameter("makeup2", "Makeup gain ch2", " dB", NormalisableRange<float>(-20, 20, 0.1), 0, nullptr, nullptr);

  parameters.createAndAddParameter("drywet", "Dry/Wet", "", NormalisableRange<float>(0, 100, 0.1), 100, nullptr, nullptr);

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
      const char* preset0 = "<ATKSideChainCompressor><PARAM id=\"link\" value=\"0\" /><PARAM id=\"middleside\" value=\"0\" /><PARAM id=\"enableCh1\" value=\"1\" /><PARAM id=\"enableCh2\" value=\"1\" /><PARAM id=\"power1\" value=\"10\" /><PARAM id=\"attack1\" value=\"10\" /><PARAM id=\"release1\" value=\"10\" /> <PARAM id=\"threshold1\" value=\"0\" /><PARAM id=\"slope1\" value=\"2\" /><PARAM id=\"softness1\" value=\"-2\" /><PARAM id=\"makeup1\" value=\"0\" /><PARAM id=\"power2\" value=\"10\" /><PARAM id=\"attack2\" value=\"10\" /><PARAM id=\"release2\" value=\"10\" /> <PARAM id=\"threshold2\" value=\"0\" /><PARAM id=\"slope2\" value=\"2\" /><PARAM id=\"softness2\" value=\"-2\" /><PARAM id=\"makeup2\" value=\"0\" /><PARAM id=\"drywet\" value=\"100\" /></ATKSideChainCompressor>";
      XmlDocument doc(preset0);

      auto el = doc.getDocumentElement();
      parameters.state = ValueTree::fromXml(*el);
      delete el;
    }
    else if (index == 1)
    {
      const char* preset1 = "<ATKSideChainCompressor><PARAM id=\"link\" value=\"0\" /><PARAM id=\"middleside\" value=\"0\" /><PARAM id=\"enableCh1\" value=\"1\" /><PARAM id=\"enableCh2\" value=\"1\" /><PARAM id=\"power1\" value=\"10\" /><PARAM id=\"attack1\" value=\"10\" /><PARAM id=\"release1\" value=\"10\" /> <PARAM id=\"threshold1\" value=\"0\" /><PARAM id=\"slope1\" value=\"2\" /><PARAM id=\"softness1\" value=\"-2\" /><PARAM id=\"makeup1\" value=\"0\" /><PARAM id=\"power2\" value=\"10\" /><PARAM id=\"attack2\" value=\"10\" /><PARAM id=\"release2\" value=\"10\" /> <PARAM id=\"threshold2\" value=\"0\" /><PARAM id=\"slope2\" value=\"2\" /><PARAM id=\"softness2\" value=\"-2\" /><PARAM id=\"makeup2\" value=\"0\" /><PARAM id=\"drywet\" value=\"50\" /></ATKSideChainCompressor>";
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
	auto sampleRate = std::lround(dbSampleRate);
  
  if (sampleRate != endpoint.get_input_sampling_rate())
  {
    inLFilter.set_input_sampling_rate(sampleRate);
    inLFilter.set_output_sampling_rate(sampleRate);
    inRFilter.set_input_sampling_rate(sampleRate);
    inRFilter.set_output_sampling_rate(sampleRate);
    inSideChainLFilter.set_input_sampling_rate(sampleRate);
    inSideChainLFilter.set_output_sampling_rate(sampleRate);
    inSideChainRFilter.set_input_sampling_rate(sampleRate);
    inSideChainRFilter.set_output_sampling_rate(sampleRate);
    outLFilter.set_input_sampling_rate(sampleRate);
    outLFilter.set_output_sampling_rate(sampleRate);
    outRFilter.set_input_sampling_rate(sampleRate);
    outRFilter.set_output_sampling_rate(sampleRate);
    
    middlesidesplitFilter.set_input_sampling_rate(sampleRate);
    middlesidesplitFilter.set_output_sampling_rate(sampleRate);
    sidechainmiddlesidesplitFilter.set_input_sampling_rate(sampleRate);
    sidechainmiddlesidesplitFilter.set_output_sampling_rate(sampleRate);
    volumesplitFilter.set_input_sampling_rate(sampleRate);
    volumesplitFilter.set_output_sampling_rate(sampleRate);
    middlesidemergeFilter.set_input_sampling_rate(sampleRate);
    middlesidemergeFilter.set_output_sampling_rate(sampleRate);
    volumemergeFilter.set_input_sampling_rate(sampleRate);
    volumemergeFilter.set_output_sampling_rate(sampleRate);
    sumFilter.set_input_sampling_rate(sampleRate);
    sumFilter.set_output_sampling_rate(sampleRate);
    
    powerFilter1.set_input_sampling_rate(sampleRate);
    powerFilter1.set_output_sampling_rate(sampleRate);
    attackReleaseFilter1.set_input_sampling_rate(sampleRate);
    attackReleaseFilter1.set_output_sampling_rate(sampleRate);
    gainColoredCompressorFilter1.set_input_sampling_rate(sampleRate);
    gainColoredCompressorFilter1.set_output_sampling_rate(sampleRate);
    makeupFilter1.set_input_sampling_rate(sampleRate);
    makeupFilter1.set_output_sampling_rate(sampleRate);
    
    powerFilter2.set_input_sampling_rate(sampleRate);
    powerFilter2.set_output_sampling_rate(sampleRate);
    attackReleaseFilter2.set_input_sampling_rate(sampleRate);
    attackReleaseFilter2.set_output_sampling_rate(sampleRate);
    gainColoredCompressorFilter2.set_input_sampling_rate(sampleRate);
    gainColoredCompressorFilter2.set_output_sampling_rate(sampleRate);
    makeupFilter2.set_input_sampling_rate(sampleRate);
    makeupFilter2.set_output_sampling_rate(sampleRate);
    
    applyGainFilter.set_input_sampling_rate(sampleRate);
    applyGainFilter.set_output_sampling_rate(sampleRate);
    drywetFilter.set_input_sampling_rate(sampleRate);
    drywetFilter.set_output_sampling_rate(sampleRate);
    endpoint.set_input_sampling_rate(sampleRate);
    endpoint.set_output_sampling_rate(sampleRate);
    
    if (old_rms1 == 0)
    {
      powerFilter1.set_memory(0);
    }
    else
    {
      powerFilter1.set_memory(std::exp(-1e3 / (old_rms1 * sampleRate)));
    }
    if (old_rms2 == 0)
    {
      powerFilter2.set_memory(0);
    }
    else
    {
      powerFilter2.set_memory(std::exp(-1e3 / (old_rms2 * sampleRate)));
    }
    
    attackReleaseFilter1.set_release(std::exp(-1 / (old_attack1* 1e-3 * sampleRate))); // in ms
    attackReleaseFilter1.set_attack(std::exp(-1 / (old_release1 * 1e-3 * sampleRate))); // in ms
    attackReleaseFilter2.set_release(std::exp(-1 / (old_attack2 * 1e-3 * sampleRate))); // in ms
    attackReleaseFilter2.set_attack(std::exp(-1 / (old_release2 * 1e-3 * sampleRate))); // in ms
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
  void check_power(float new_value, int sample_rate, float& old_value, ATK::PowerFilter<double>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      if (old_value == 0)
      {
        filter.set_memory(0);
      }
      else
      {
        filter.set_memory(std::exp(-1e3 / (old_value * sample_rate)));
      }
    }
  }
  
  // release targets attack because if the inversion dur to the gain filter
  void check_release(float new_value, int sample_rate, float& old_value, ATK::AttackReleaseFilter<double>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_attack(std::exp(-1e3/(old_value * sample_rate))); // in ms
    }
  }

  void check_attack(float new_value, int sample_rate, float& old_value, ATK::AttackReleaseFilter<double>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_release(std::exp(-1e3/(old_value * sample_rate))); // in ms
    }
  }
  
  void check_threshold(float new_value, float& old_value, ATK::GainFilter<ATK::GainColoredCompressorFilter<double>>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_threshold(std::pow(10, old_value / 10));
    }
  }
  
  void check_slope(float new_value, float& old_value, ATK::GainFilter<ATK::GainColoredCompressorFilter<double>>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_ratio(old_value);
    }
  }
  
  void check_softness(float new_value, float& old_value, ATK::GainFilter<ATK::GainColoredCompressorFilter<double>>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_softness(std::pow(10, old_value));
    }
  }
  
  void check_makeup(float new_value, float& old_value, ATK::VolumeFilter<double>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_volume_db(old_value);
    }
  }
  
  void check_drywet(float new_value, float& old_value, ATK::DryWetFilter<double>& filter)
  {
    if (new_value != old_value)
    {
      old_value = new_value;
      filter.set_dry(old_value / 100);
    }
  }
  
  void check_link(float new_value, bool& old_value, ATKSideChainCompressorAudioProcessor& processor)
  {
    if ((new_value > 0.5) != old_value)
    {
      old_value = (new_value > 0.5);
      if(old_value)
      {
        processor.link();
      }
      else
      {
        processor.unlink();
      }
    }
  }
  
  void check_middleside(float new_value, bool& old_value, ATKSideChainCompressorAudioProcessor& processor)
  {
    if ((new_value > 0.5) != old_value)
    {
      old_value = (new_value > 0.5);
      if(old_value)
      {
        processor.setMS();
      }
      else
      {
        processor.setStereo();
      }
    }
  }
  
  void check_enable1(float new_value, bool& old_value, ATKSideChainCompressorAudioProcessor& processor)
  {
    if ((new_value > 0.5) != old_value)
    {
      old_value = (new_value > 0.5);
      if(old_value)
      {
        processor.enableCh1();
      }
      else
      {
        processor.disableCh1();
      }
    }
  }
  
  void check_enable2(float new_value, bool& old_value, ATKSideChainCompressorAudioProcessor& processor)
  {
    if ((new_value > 0.5) != old_value)
    {
      old_value = (new_value > 0.5);
      if(old_value)
      {
        processor.enableCh2();
      }
      else
      {
        processor.disableCh2();
      }
    }
  }
}

void ATKSideChainCompressorAudioProcessor::link()
{
  gainColoredCompressorFilter1.set_input_port(0, &sumFilter, 0);
  applyGainFilter.set_input_port(2, &attackReleaseFilter1, 0);
}

void ATKSideChainCompressorAudioProcessor::unlink()
{
  gainColoredCompressorFilter1.set_input_port(0, &powerFilter1, 0);
  applyGainFilter.set_input_port(2, &attackReleaseFilter2, 0);
}

void ATKSideChainCompressorAudioProcessor::setMS()
{
  powerFilter1.set_input_port(0, &volumesplitFilter, 2);
  powerFilter2.set_input_port(0, &volumesplitFilter, 3);
  drywetFilter.set_input_port(0, &volumemergeFilter, 0);
  drywetFilter.set_input_port(2, &volumemergeFilter, 1);
  applyGainFilter.set_input_port(1, &volumesplitFilter, 0);
  applyGainFilter.set_input_port(3, &volumesplitFilter, 1);
  if (old_enableCh1)
  {
    middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
  }
  else
  {
    middlesidemergeFilter.set_input_port(0, &volumesplitFilter, 0);
  }
  if (old_enableCh2)
  {
    middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
  }
  else
  {
    middlesidemergeFilter.set_input_port(1, &volumesplitFilter, 1);
  }
}

void ATKSideChainCompressorAudioProcessor::setStereo()
{
  powerFilter1.set_input_port(0, &inSideChainLFilter, 0);
  powerFilter2.set_input_port(0, &inSideChainRFilter, 0);
  applyGainFilter.set_input_port(1, &inLFilter, 0);
  applyGainFilter.set_input_port(3, &inRFilter, 0);
  if (old_enableCh1)
  {
    drywetFilter.set_input_port(0, &makeupFilter1, 0);
  }
  else
  {
    drywetFilter.set_input_port(0, &inLFilter, 0);
  }
  if (old_enableCh2)
  {
    drywetFilter.set_input_port(2, &makeupFilter2, 0);
  }
  else
  {
    drywetFilter.set_input_port(2, &inRFilter, 0);
  }
}

void ATKSideChainCompressorAudioProcessor::enableCh1()
{
  middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
  if (old_middleside)
  {
    drywetFilter.set_input_port(0, &volumemergeFilter, 0);
  }
  else
  {
    drywetFilter.set_input_port(0, &makeupFilter1, 0);
  }
}

void ATKSideChainCompressorAudioProcessor::disableCh1()
{
  middlesidemergeFilter.set_input_port(0, &volumesplitFilter, 0);
  if (old_middleside)
  {
    drywetFilter.set_input_port(0, &volumemergeFilter, 0);
  }
  else
  {
    drywetFilter.set_input_port(0, &inLFilter, 0);
  }
}

void ATKSideChainCompressorAudioProcessor::enableCh2()
{
  middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
  if (old_middleside)
  {
    drywetFilter.set_input_port(0, &volumemergeFilter, 0);
  }
  else
  {
    drywetFilter.set_input_port(0, &makeupFilter1, 0);
  }
}

void ATKSideChainCompressorAudioProcessor::disableCh2()
{
  middlesidemergeFilter.set_input_port(1, &volumesplitFilter, 1);
  if (old_middleside)
  {
    drywetFilter.set_input_port(1, &volumemergeFilter, 1);
  }
  else
  {
    drywetFilter.set_input_port(1, &inRFilter, 0);
  }
}

void ATKSideChainCompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  check_link(*parameters.getRawParameterValue ("link"), old_link, *this);
  check_middleside(*parameters.getRawParameterValue ("middleside"), old_middleside, *this);
  check_enable1(*parameters.getRawParameterValue ("enable1"), old_enableCh1, *this);
  check_enable2(*parameters.getRawParameterValue ("enable2"), old_enableCh2, *this);

  check_power(*parameters.getRawParameterValue ("power1"), sampleRate, old_rms1, powerFilter1);
  check_power(*parameters.getRawParameterValue ("power2"), sampleRate, old_rms2, powerFilter2);
  check_attack(*parameters.getRawParameterValue ("attack1"), sampleRate, old_attack1, attackReleaseFilter1);
  check_attack(*parameters.getRawParameterValue ("attack2"), sampleRate, old_attack2, attackReleaseFilter2);
  check_release(*parameters.getRawParameterValue ("release1"), sampleRate, old_release1, attackReleaseFilter1);
  check_release(*parameters.getRawParameterValue ("release2"), sampleRate, old_release2, attackReleaseFilter2);
  check_threshold(*parameters.getRawParameterValue ("threshold1"), old_threshold1, gainColoredCompressorFilter1);
  check_threshold(*parameters.getRawParameterValue ("threshold2"), old_threshold2, gainColoredCompressorFilter2);
  check_slope(*parameters.getRawParameterValue ("slope1"), old_slope1, gainColoredCompressorFilter1);
  check_slope(*parameters.getRawParameterValue ("slope2"), old_slope2, gainColoredCompressorFilter2);
  check_softness(*parameters.getRawParameterValue ("softness1"), old_softness1, gainColoredCompressorFilter1);
  check_softness(*parameters.getRawParameterValue ("softness2"), old_softness2, gainColoredCompressorFilter2);
  check_makeup(*parameters.getRawParameterValue ("makeup1"), old_makeup1, makeupFilter1);
  check_makeup(*parameters.getRawParameterValue ("makeup2"), old_makeup2, makeupFilter2);

  check_drywet(*parameters.getRawParameterValue ("drywet"), old_drywet, drywetFilter);

  const int totalNumInputChannels  = getTotalNumInputChannels();
  const int totalNumOutputChannels = getTotalNumOutputChannels();

  assert(totalNumInputChannels == 2 || totalNumInputChannels == 4);
  assert(totalNumOutputChannels == 2);
  
  if (totalNumInputChannels == 4)
  {
    inSideChainLFilter.set_pointer(buffer.getReadPointer(2), buffer.getNumSamples());
    inSideChainRFilter.set_pointer(buffer.getReadPointer(3), buffer.getNumSamples());
  }
  else
  {
    inSideChainLFilter.set_pointer(buffer.getReadPointer(0), buffer.getNumSamples());
    inSideChainRFilter.set_pointer(buffer.getReadPointer(1), buffer.getNumSamples());
  }
 
  inLFilter.set_pointer(buffer.getReadPointer(0), buffer.getNumSamples());
  inRFilter.set_pointer(buffer.getReadPointer(1), buffer.getNumSamples());

  outLFilter.set_pointer(buffer.getWritePointer(0), buffer.getNumSamples());
  outRFilter.set_pointer(buffer.getWritePointer(1), buffer.getNumSamples());
  endpoint.process(buffer.getNumSamples());
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
