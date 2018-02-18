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
#include <ATKJUCEComponents/Dynamic/GainSwellFilter.h>
#include <ATKJUCEComponents/Dynamic/PowerFilter.h>
#include <ATKJUCEComponents/Tools/DryWetFilter.h>
#include <ATKJUCEComponents/Tools/VolumeFilter.h>

//==============================================================================
/**
*/
class ATKAutoSwellAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKAutoSwellAudioProcessorEditor(ATKAutoSwellAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKAutoSwellAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKAutoSwellAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ATK::juce::PowerFilterComponent power;
  ATK::juce::AttackReleaseFilterComponent attackrelease;
  ATK::juce::GainSwellFilterComponent compressor;
  ATK::juce::VolumeFilterComponent makeup;
  ATK::juce::DryWetFilterComponent drywet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKAutoSwellAudioProcessorEditor)
};
