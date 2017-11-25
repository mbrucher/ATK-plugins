/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <atk_core/atk_core.h>
#include <atk_dynamic/atk_dynamic.h>
#include <atk_tools/atk_tools.h>


//==============================================================================
/**
*/
class ATKColoredExpanderAudioProcessor  : public AudioProcessor
{
public:
  //==============================================================================
  ATKColoredExpanderAudioProcessor();
  ~ATKColoredExpanderAudioProcessor();

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
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKColoredExpanderAudioProcessor)
    
  ATK::InPointerFilter<float> inFilter;
  ATK::OutPointerFilter<float> outFilter;
  ATK::PowerFilter<float> powerFilter;
  ATK::AttackReleaseFilter<float> attackReleaseFilter;
  ATK::GainFilter<ATK::GainMaxColoredExpanderFilter<float>> gainExpanderFilter;
  ATK::ApplyGainFilter<float> applyGainFilter;
  ATK::VolumeFilter<float> volumeFilter;
  ATK::DryWetFilter<float> drywetFilter;

  AudioProcessorValueTreeState parameters;
  int sampleRate;
  int lastParameterSet;

  float old_rms;
  float old_attack;
  float old_release;
  float old_threshold;
  float old_slope;
  float old_softness;
  float old_color;
  float old_quality;
  float old_maxreduc;
  float old_makeup;
  float old_drywet;
};
