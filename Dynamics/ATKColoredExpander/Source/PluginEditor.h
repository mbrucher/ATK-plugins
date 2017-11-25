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
#include <ATKJUCEComponents/Dynamic/GainMaxColoredExpanderFilter.h>
#include <ATKJUCEComponents/Dynamic/PowerFilter.h>
#include <ATKJUCEComponents/Tools/DryWetFilter.h>
#include <ATKJUCEComponents/Tools/VolumeFilter.h>

//==============================================================================
/**
*/
class ATKColoredExpanderAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKColoredExpanderAudioProcessorEditor(ATKColoredExpanderAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKColoredExpanderAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKColoredExpanderAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ATK::juce::PowerFilterComponent power;
  ATK::juce::AttackReleaseFilterComponent attackrelease;
  ATK::juce::GainMaxColoredExpanderFilterComponent expander;
  ATK::juce::VolumeFilterComponent makeup;
  ATK::juce::DryWetFilterComponent drywet;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKColoredExpanderAudioProcessorEditor)
};
