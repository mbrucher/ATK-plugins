/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

#include <ATKJUCEComponents/Tools/FFTViewer.h>


//==============================================================================
/**
*/
class ATKJUCEAudioProcessorEditor  : public AudioProcessorEditor
{
public:
  ATKJUCEAudioProcessorEditor (ATKJUCEAudioProcessor&);
  ~ATKJUCEAudioProcessorEditor();
  
  //==============================================================================
  void paint (Graphics&) override;
  void resized() override;
  
private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKJUCEAudioProcessor& processor;
  
  ATK::juce::FFTViewerComponent view;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKJUCEAudioProcessorEditor)
};
