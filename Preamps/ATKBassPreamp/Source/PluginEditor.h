/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

#include <ATKJUCEComponents/Tools/VolumeFilter.h>

//==============================================================================
/**
*/
class ATKBassPreampAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKBassPreampAudioProcessorEditor (ATKBassPreampAudioProcessor&);
  ~ATKBassPreampAudioProcessorEditor();

  //==============================================================================
  void paint (Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKBassPreampAudioProcessor& processor;
  
  ATK::juce::VolumeFilterComponent level;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKBassPreampAudioProcessorEditor)
};
