/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutCircularPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>

#include <ATKJUCEComponents/Tools/FFTViewerInterface.h>

//==============================================================================
/**
*/
class ATKJUCEAudioProcessor  : public AudioProcessor, public ::ATK::juce::FFTViewerInterface
{
public:
  //==============================================================================
  ATKJUCEAudioProcessor();
  ~ATKJUCEAudioProcessor();
  
  //==============================================================================
  void prepareToPlay (double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  
#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
  
  void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
  
  //==============================================================================
  AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;
  
  //==============================================================================
  const String getName() const override;
  
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  double getTailLengthSeconds() const override;
  
  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram (int index) override;
  const String getProgramName (int index) override;
  void changeProgramName (int index, const String& newName) override;
  
  //==============================================================================
  void getStateInformation (MemoryBlock& destData) override;
  void setStateInformation (const void* data, int sizeInBytes) override;

  int get_sampling_rate() const override;
  std::size_t get_nb_channels() const override;
  const std::vector<double>& get_last_slice(std::size_t index, bool& process) override;
  
private:
  void build_window(std::size_t size);
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKJUCEAudioProcessor)
  
  ATK::InPointerFilter<float> inL;
  ATK::InPointerFilter<float> inR;
  ATK::OutPointerFilter<float> outL;
  ATK::OutPointerFilter<float> outR;

  ATK::OutCircularPointerFilter<float> bufferFilterL;
  ATK::OutCircularPointerFilter<float> bufferFilterR;
  ATK::PipelineGlobalSinkFilter pipeline;
  
  std::vector<float> window;
  std::vector<double> windowedDataL;
  std::vector<double> windowedDataR;
  
  bool process_slice(ATK::OutCircularPointerFilter<float>& filter, std::vector<double>& windowedData);
 
  int sampling_rate;
};
