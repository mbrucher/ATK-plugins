/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKGuitarPreampAudioProcessorEditor::ATKGuitarPreampAudioProcessorEditor (ATKGuitarPreampAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p), gain(p.get_gain_parameter(), "Gain", minGain, maxGain, (minGain + maxGain) / 2), stack(p.get_tone_stack_bass_parameter(), p.get_tone_stack_medium_parameter(), p.get_tone_stack_high_parameter()), volume(p.get_volume_parameter(), "Volume", minVolume, maxVolume, (minVolume + maxVolume) / 2), drywet(p.get_dry_wet_parameter())

{
  addAndMakeVisible(gain);
  addAndMakeVisible(stack);
  addAndMakeVisible(volume);
  addAndMakeVisible(drywet);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (900, 200);
}

ATKGuitarPreampAudioProcessorEditor::~ATKGuitarPreampAudioProcessorEditor()
{
}

void ATKGuitarPreampAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
  g.setFont(Font("Times New Roman", 30.0f, Font::bold | Font::italic));
  g.setColour(Colours::whitesmoke);
  g.drawText("Guitar Preamp", 20, 10, 200, 30, Justification::verticallyCentred);
}

void ATKGuitarPreampAudioProcessorEditor::resized()
{
  gain.setBoundsRelative(0, 1. / 4, 1. / 6, 3. / 4);
  stack.setBoundsRelative(1. / 6, 1. / 4, 3. / 6, 3. / 4);
  volume.setBoundsRelative(4. / 6, 1. / 4, 1. / 6, 3. / 4);
  drywet.setBoundsRelative(5. / 6, 1. / 4, 1. / 6, 3. / 4);
}

