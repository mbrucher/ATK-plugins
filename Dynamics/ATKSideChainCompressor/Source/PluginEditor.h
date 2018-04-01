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
class ATKSideChainCompressorAudioProcessorEditor  : public AudioProcessorEditor, public ::juce::Button::Listener
{
public:
  ATKSideChainCompressorAudioProcessorEditor(ATKSideChainCompressorAudioProcessor& p, AudioProcessorValueTreeState& paramState);
  ~ATKSideChainCompressorAudioProcessorEditor();

  //==============================================================================
  void paint(Graphics&) override;
  void resized() override;
  
  void buttonClicked(Button* button) override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ATKSideChainCompressorAudioProcessor& processor;
  AudioProcessorValueTreeState& paramState;
  
  ::juce::ToggleButton linkButton;
  std::unique_ptr<::juce::AudioProcessorValueTreeState::ButtonAttachment> linkAtt;
  ::juce::ToggleButton middleSideButton;
  std::unique_ptr<::juce::AudioProcessorValueTreeState::ButtonAttachment> middleSideAtt;
  ::juce::ToggleButton enableCh1Button;
  std::unique_ptr<::juce::AudioProcessorValueTreeState::ButtonAttachment> enableCh1Att;
  ::juce::ToggleButton enableCh2Button;
  std::unique_ptr<::juce::AudioProcessorValueTreeState::ButtonAttachment> enableCh2Att;

  ATK::juce::PowerFilterComponent power1;
  ATK::juce::AttackReleaseFilterComponent attackrelease1;
  ATK::juce::GainColoredCompressorFilterComponent compressor1;
  ATK::juce::VolumeFilterComponent makeup1;
  ATK::juce::PowerFilterComponent power2;
  ATK::juce::AttackReleaseFilterComponent attackrelease2;
  ATK::juce::GainColoredCompressorFilterComponent compressor2;
  ATK::juce::VolumeFilterComponent makeup2;

  ATK::juce::DryWetFilterComponent drywet;
  
  bool linked;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATKSideChainCompressorAudioProcessorEditor)
};
