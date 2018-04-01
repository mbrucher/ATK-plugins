/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

#include <ATKJUCEComponents/Dynamic/AttackReleaseFilter.h>
#include <ATKJUCEComponents/Dynamic/GainColoredCompressorFilter.h>
#include <ATKJUCEComponents/Dynamic/PowerFilter.h>
#include <ATKJUCEComponents/Tools/DryWetFilter.h>
#include <ATKJUCEComponents/Tools/VolumeFilter.h>

//==============================================================================
/**
*/
class ATKSideChainCompressorAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKSideChainCompressorAudioProcessorEditor(ATKSideChainCompressorAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKSideChainCompressorAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKSideChainCompressorAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ATK::juce::PowerFilterComponent power1;
  ATK::juce::AttackReleaseFilterComponent attackrelease1;
  ATK::juce::GainColoredCompressorFilterComponent compressor1;
  ATK::juce::VolumeFilterComponent makeup1;
  ATK::juce::PowerFilterComponent power2;
  ATK::juce::AttackReleaseFilterComponent attackrelease2;
  ATK::juce::GainColoredCompressorFilterComponent compressor2;
  ATK::juce::VolumeFilterComponent makeup2;

  ATK::juce::DryWetFilterComponent drywet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKSideChainCompressorAudioProcessorEditor)
};
