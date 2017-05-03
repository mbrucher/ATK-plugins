/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKBassPreampAudioProcessorEditor::ATKBassPreampAudioProcessorEditor (ATKBassPreampAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p), level(p.get_level_filter(), "Gain", -60, 0, 0), stack(p.get_tone_stack_filter()), volume(p.get_volume_filter(), "Volume", -60, 0, 0)

{
  addAndMakeVisible(level);
  addAndMakeVisible(stack);
  addAndMakeVisible(volume);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (1000, 200);
}

ATKBassPreampAudioProcessorEditor::~ATKBassPreampAudioProcessorEditor()
{
}

void ATKBassPreampAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void ATKBassPreampAudioProcessorEditor::resized()
{
  level.setBoundsRelative(0, 0, .2, 1);
  stack.setBoundsRelative(0.2, 0, .6, 1);
  volume.setBoundsRelative(0.8, 0, .2, 1);
}

