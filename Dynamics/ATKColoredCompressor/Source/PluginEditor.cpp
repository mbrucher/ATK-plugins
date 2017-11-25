/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKColoredCompressorAudioProcessorEditor::ATKColoredCompressorAudioProcessorEditor (ATKColoredCompressorAudioProcessor& p, AudioProcessorValueTreeState& paramState)
: AudioProcessorEditor (&p), processor (p), paramState(paramState), power(paramState, "power"), attackrelease(paramState, "attack",  "release"), compressor(paramState, "threshold", "slope", "softness", "color", "quality"), makeup(paramState, "makeup", "Make-up"), drywet(paramState, "drywet")

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

ATKColoredCompressorAudioProcessorEditor::~ATKColoredCompressorAudioProcessorEditor()
{
}

void ATKColoredCompressorAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
  g.setFont(Font("Times New Roman", 30.0f, Font::bold | Font::italic));
  g.setColour(Colours::whitesmoke);
  g.drawText("Colored Compressor", 20, 10, 400, 30, Justification::verticallyCentred);
}

void ATKColoredCompressorAudioProcessorEditor::resized()
{
  power.setBoundsRelative(0, 1. / 4, 1. / 10, 3. / 4);
  attackrelease.setBoundsRelative(1. / 10, 1. / 4, 2. / 10, 3. / 4);
  compressor.setBoundsRelative(3. / 10, 1. / 4, 5. / 10, 3. / 4);
  makeup.setBoundsRelative(8. / 10, 1. / 4, 1. / 10, 3. / 4);
  drywet.setBoundsRelative(9. / 10, 1. / 4, 1. / 10, 3. / 4);
}

