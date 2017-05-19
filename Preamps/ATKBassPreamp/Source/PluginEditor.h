/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

#include <ATKJUCEComponents/EQ/ToneStackFilter.h>
#include <ATKJUCEComponents/Tools/DryWetFilter.h>
#include <ATKJUCEComponents/Tools/VolumeFilter.h>

//==============================================================================
/**
*/
class ATKBassPreampAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKBassPreampAudioProcessorEditor(ATKBassPreampAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKBassPreampAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKBassPreampAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ATK::juce::VolumeFilterComponent gain;
  ATK::juce::ToneStackFilterComponent stack;
  ATK::juce::VolumeFilterComponent volume;
  ATK::juce::DryWetFilterComponent drywet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKBassPreampAudioProcessorEditor)
};
