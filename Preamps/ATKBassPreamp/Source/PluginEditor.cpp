/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <ATKJUCEComponents/JUCE/LookAndFeel.h>

//==============================================================================
ATKBassPreampAudioProcessorEditor::ATKBassPreampAudioProcessorEditor (ATKBassPreampAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p), levelSlider(juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow)

{
  addAndMakeVisible(levelSlider);
  levelSlider.setRange (-60, 0);
  levelSlider.setTextValueSuffix (" dB");
  levelSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromFloatRGBA(1, 1, 1, 0));
  levelSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
  levelSlider.setLookAndFeel(&ATK::juce::SimpleSliderLookAndFeel::get_instance());
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (400, 300);
}

ATKBassPreampAudioProcessorEditor::~ATKBassPreampAudioProcessorEditor()
{
}

void ATKBassPreampAudioProcessorEditor::resized()
{
  levelSlider.setBoundsRelative(0.1, 0.1, 0.8, 0.8);
}
