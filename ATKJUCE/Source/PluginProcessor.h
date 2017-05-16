/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <ATK/Core/InPointerFilter.h>
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
  const std::vector<double>& get_last_slice(bool& process) override;
  
private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKJUCEAudioProcessor)
  
  ATK::InPointerFilter<float> inL;
  ATK::InPointerFilter<float> inR;
  ATK::OutPointerFilter<float> outL;
  ATK::OutPointerFilter<float> outR;
  ATK::OutPointerFilter<float> buffer_filter;
  ATK::PipelineGlobalSinkFilter pipeline;
  
  std::vector<float> full_buffer;
  std::vector<double> fft_buffer;
  std::vector<float> window;
  int sampling_rate;
  int current_buffer_index;
  int current_slice;
  int last_checked_out_buffer;

  static const unsigned int slice_size = 1024 * 4;
  static const unsigned int nb_slices = 4;
};
