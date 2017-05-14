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


//==============================================================================
/**
*/
class ATKJUCEAudioProcessor  : public AudioProcessor
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
  
  const std::vector<float>& get_last_slice();
  
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
  std::vector<float> fft_buffer;
  int slice_size;
  int current_buffer_index;
  int current_slice;
  int last_checked_out_buffer;
};
