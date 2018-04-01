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
class ATKSideChainCompressorAudioProcessor  : public AudioProcessor
{
public:
  //==============================================================================
  ATKSideChainCompressorAudioProcessor();
  ~ATKSideChainCompressorAudioProcessor();

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
  
  void link();
  void unlink();
  void setMS();
  void setStereo();
  void enableCh1();
  void disableCh1();
  void enableCh2();
  void disableCh2();
  
private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKSideChainCompressorAudioProcessor)
    
  ATK::InPointerFilter<float> inLFilter;
  ATK::InPointerFilter<float> inRFilter;
  ATK::InPointerFilter<float> inSideChainLFilter;
  ATK::InPointerFilter<float> inSideChainRFilter;
  
  ATK::MiddleSideFilter<double> middlesidesplitFilter;
  ATK::MiddleSideFilter<double> sidechainmiddlesidesplitFilter;
  ATK::VolumeFilter<double> volumesplitFilter;
  
  ATK::PowerFilter<double> powerFilter1;
  ATK::PowerFilter<double> powerFilter2;
  ATK::SumFilter<double> sumFilter; // in case we link both channels
  
  ATK::AttackReleaseFilter<double> attackReleaseFilter1;
  ATK::AttackReleaseFilter<double> attackReleaseFilter2;
  ATK::GainFilter<ATK::GainColoredCompressorFilter<double>> gainColoredCompressorFilter1;
  ATK::GainFilter<ATK::GainColoredCompressorFilter<double>> gainColoredCompressorFilter2;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> makeupFilter1;
  ATK::VolumeFilter<double> makeupFilter2;
  
  ATK::MiddleSideFilter<double> middlesidemergeFilter;
  ATK::VolumeFilter<double> volumemergeFilter;
  
  ATK::DryWetFilter<double> drywetFilter;
  
  ATK::OutPointerFilter<float> outLFilter;
  ATK::OutPointerFilter<float> outRFilter;
  
  ATK::PipelineGlobalSinkFilter endpoint;
  
  AudioProcessorValueTreeState parameters;
  int sampleRate;
  int lastParameterSet;
  
  bool old_link;
  bool old_middleside;
  bool old_enableCh1;
  bool old_enableCh2;
  
  float old_rms1;
  float old_attack1;
  float old_release1;
  float old_threshold1;
  float old_slope1;
  float old_softness1;
  float old_color1;
  float old_quality1;
  float old_makeup1;
  
  float old_rms2;
  float old_attack2;
  float old_release2;
  float old_threshold2;
  float old_slope2;
  float old_softness2;
  float old_color2;
  float old_quality2;
  float old_makeup2;

  float old_drywet;
};
