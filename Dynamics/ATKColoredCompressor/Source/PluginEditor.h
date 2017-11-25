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
class ATKColoredCompressorAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKColoredCompressorAudioProcessorEditor(ATKColoredCompressorAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKColoredCompressorAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKColoredCompressorAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ATK::juce::PowerFilterComponent power;
  ATK::juce::AttackReleaseFilterComponent attackrelease;
  ATK::juce::GainColoredCompressorFilterComponent compressor;
  ATK::juce::VolumeFilterComponent makeup;
  ATK::juce::DryWetFilterComponent drywet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKColoredCompressorAudioProcessorEditor)
};
