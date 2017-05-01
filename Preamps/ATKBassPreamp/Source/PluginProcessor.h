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

#include <ATK/Tools/OversamplingFilter.h>
#include <ATK/Tools/DecimationFilter.h>
#include <ATK/Tools/VolumeFilter.h>

#include <ATK/EQ/ButterworthFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/ToneStackFilter.h>

#include <ATK/Preamplifier/Triode2Filter.h>
#include <ATK/Preamplifier/DempwolfTriodeFunction.h>


//==============================================================================
/**
*/
class ATKBassPreampAudioProcessor  : public AudioProcessor
{
public:
  //==============================================================================
  ATKBassPreampAudioProcessor();
  ~ATKBassPreampAudioProcessor();

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

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKBassPreampAudioProcessor)
    
  ATK::InPointerFilter<float> inFilter;
  ATK::VolumeFilter<double> levelFilter;
  ATK::OversamplingFilter<double, ATK::Oversampling6points5order_4<double> > oversamplingFilter;
  ATK::Triode2Filter<double, ATK::DempwolfTriodeFunction<double>> overdriveFilter;
  ATK::IIRFilter<ATK::ButterworthLowPassCoefficients<double> > lowpassFilter;
  ATK::DecimationFilter<double> decimationFilter;
  ATK::IIRFilter<ATK::ToneStackCoefficients<double> > toneFilter;
  ATK::VolumeFilter<double> volumeFilter;
  ATK::OutPointerFilter<float> outFilter;

  AudioParameterFloat* level;
  AudioParameterFloat* bass;
  AudioParameterFloat* medium;
  AudioParameterFloat* high;
  AudioParameterFloat* gain;
};
