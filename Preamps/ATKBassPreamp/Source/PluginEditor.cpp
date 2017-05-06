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
: AudioProcessorEditor (&p), processor (p), level(p.get_level_parameter(), "Gain", -20, 20, 0), stack(p.get_tone_stack_bass_parameter(), p.get_tone_stack_medium_parameter(), p.get_tone_stack_high_parameter()), volume(p.get_volume_parameter(), "Volume", -20, 20, 0), drywet(p.get_dry_wet_parameter())

{
  addAndMakeVisible(level);
  addAndMakeVisible(stack);
  addAndMakeVisible(volume);
  addAndMakeVisible(drywet);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (1200, 250);
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
  level.setBounds(0, 50, 200, 200);
  stack.setBounds(200, 50, 600, 200);
  volume.setBounds(800, 50, 200, 200);
  drywet.setBounds(1000, 50, 200, 200);
}

