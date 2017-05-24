/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKBassPreampAudioProcessorEditor::ATKBassPreampAudioProcessorEditor (ATKBassPreampAudioProcessor& p, AudioProcessorValueTreeState& paramState)
: AudioProcessorEditor (&p), processor (p), paramState(paramState), gain(paramState, "gain", "Gain", minGain, maxGain), stack(paramState, "bass", "medium", "high"), volume(paramState, "volume", "Volume", minVolume, maxVolume), drywet(paramState, "drywet")

{
  addAndMakeVisible(gain);
  addAndMakeVisible(stack);
  addAndMakeVisible(volume);
  addAndMakeVisible(drywet);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (900, 200);
}

ATKBassPreampAudioProcessorEditor::~ATKBassPreampAudioProcessorEditor()
{
}

void ATKBassPreampAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
  g.setFont(Font("Times New Roman", 30.0f, Font::bold | Font::italic));
  g.setColour(Colours::whitesmoke);
  g.drawText("Bass Preamp", 20, 10, 200, 30, Justification::verticallyCentred);
}

void ATKBassPreampAudioProcessorEditor::resized()
{
  gain.setBoundsRelative(0, 1. / 4, 1. / 6, 3. / 4);
  stack.setBoundsRelative(1. / 6, 1. / 4, 3. / 6, 3. / 4);
  volume.setBoundsRelative(4. / 6, 1. / 4, 1. / 6, 3. / 4);
  drywet.setBoundsRelative(5. / 6, 1. / 4, 1. / 6, 3. / 4);
}

