/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKAutoSwellAudioProcessorEditor::ATKAutoSwellAudioProcessorEditor (ATKAutoSwellAudioProcessor& p, AudioProcessorValueTreeState& paramState)
: AudioProcessorEditor (&p), processor (p), paramState(paramState), power(paramState, "power"), attackrelease(paramState, "attack",  "release"), compressor(paramState, "threshold", "slope", "softness"), makeup(paramState, "makeup", "Make-up"), drywet(paramState, "drywet")

{
  addAndMakeVisible(power);
  addAndMakeVisible(attackrelease);
  addAndMakeVisible(compressor);
  addAndMakeVisible(makeup);
  addAndMakeVisible(drywet);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (1000, 200);
}

ATKAutoSwellAudioProcessorEditor::~ATKAutoSwellAudioProcessorEditor()
{
}

void ATKAutoSwellAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
  g.setFont(Font("Times New Roman", 30.0f, Font::bold | Font::italic));
  g.setColour(Colours::whitesmoke);
  g.drawText("Auto Swell", 20, 10, 400, 30, Justification::verticallyCentred);
}

void ATKAutoSwellAudioProcessorEditor::resized()
{
  power.setBoundsRelative(0, 1. / 4, 1. / 8, 3. / 4);
  attackrelease.setBoundsRelative(1. / 8, 1. / 4, 2. / 8, 3. / 4);
  compressor.setBoundsRelative(3. / 8, 1. / 4, 3. / 8, 3. / 4);
  makeup.setBoundsRelative(6. / 8, 1. / 4, 1. / 8, 3. / 4);
  drywet.setBoundsRelative(7. / 8, 1. / 4, 1. / 8, 3. / 4);
}

