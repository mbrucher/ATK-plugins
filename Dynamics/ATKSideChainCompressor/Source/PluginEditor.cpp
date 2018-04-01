/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ATKSideChainCompressorAudioProcessorEditor::ATKSideChainCompressorAudioProcessorEditor (ATKSideChainCompressorAudioProcessor& p, AudioProcessorValueTreeState& paramState)
: AudioProcessorEditor (&p), processor (p), paramState(paramState), power1(paramState, "power1"), attackrelease1(paramState, "attack1",  "release1"), compressor1(paramState, "threshold1", "slope1", "softness1", "color1", "quality1"), makeup1(paramState, "makeup1", "Make-up"),  power2(paramState, "power2"), attackrelease2(paramState, "attack2",  "release2"), compressor2(paramState, "threshold2", "slope2", "softness2", "color2", "quality2"), makeup2(paramState, "makeup2", "Make-up"), drywet(paramState, "drywet")

{
  // Link will make attack/release and gain compressor ch2 disapear. We keep power2 and makeup 2
  
  addAndMakeVisible(power1);
  addAndMakeVisible(attackrelease1);
  addAndMakeVisible(compressor1);
  addAndMakeVisible(makeup1);
  addAndMakeVisible(power2);
  addAndMakeVisible(attackrelease2);
  addAndMakeVisible(compressor2);
  addAndMakeVisible(makeup2);
  addAndMakeVisible(drywet);
  
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (1100, 350);
}

ATKSideChainCompressorAudioProcessorEditor::~ATKSideChainCompressorAudioProcessorEditor()
{
}

void ATKSideChainCompressorAudioProcessorEditor::paint (Graphics& g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
  g.setFont(Font("Times New Roman", 30.0f, Font::bold | Font::italic));
  g.setColour(Colours::whitesmoke);
  g.drawText("Side Chain Compressor", 20, 10, 400, 30, Justification::verticallyCentred);
}

void ATKSideChainCompressorAudioProcessorEditor::resized()
{
  constexpr auto nb_horiz = 11;
  constexpr auto nb_vert = 7;
  
  power1.setBoundsRelative(1. / nb_horiz, 1. / nb_vert, 1. / nb_horiz, 3. / nb_vert);
  attackrelease1.setBoundsRelative(2. / nb_horiz, 1. / nb_vert, 2. / nb_horiz, 3. / nb_vert);
  compressor1.setBoundsRelative(4. / 11, 1. / nb_vert, 5. / nb_horiz, 3. / nb_vert);
  makeup1.setBoundsRelative(9. / nb_horiz, 1. / nb_vert, 1. / nb_horiz, 3. / nb_vert);

  power2.setBoundsRelative(1. / nb_horiz, 4. / nb_vert, 1. / nb_horiz, 3. / nb_vert);
  attackrelease2.setBoundsRelative(2. / nb_horiz, 4. / nb_vert, 2. / nb_horiz, 3. / nb_vert);
  compressor2.setBoundsRelative(4. / 11, 4. / nb_vert, 5. / nb_horiz, 3. / nb_vert);
  makeup2.setBoundsRelative(9. / nb_horiz, 4. / nb_vert, 1. / nb_horiz, 3. / nb_vert);

  drywet.setBoundsRelative(10. / nb_horiz, 3. / nb_vert, 1. / nb_horiz, 3. / nb_vert);
}

