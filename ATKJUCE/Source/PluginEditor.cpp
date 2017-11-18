/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ATKJUCEAudioProcessorEditor::ATKJUCEAudioProcessorEditor (ATKJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), view(&p)
{
  addAndMakeVisible(&view);
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (800, 600);
}

ATKJUCEAudioProcessorEditor::~ATKJUCEAudioProcessorEditor()
{
}

//==============================================================================
void ATKJUCEAudioProcessorEditor::paint (Graphics& g)
{
}

void ATKJUCEAudioProcessorEditor::resized()
{
  view.setBoundsRelative(0, 0, 1, 1);
}
