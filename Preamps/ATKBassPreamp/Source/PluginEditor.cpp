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
: AudioProcessorEditor (&p), processor (p), level(p.get_level_filter(), "Volume", -60, 0, 0)

{
  addAndMakeVisible(level);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (200, 200);
}

ATKBassPreampAudioProcessorEditor::~ATKBassPreampAudioProcessorEditor()
{
}

void ATKBassPreampAudioProcessorEditor::resized()
{
  level.setBoundsRelative(0, 0, 1, 1);
}
