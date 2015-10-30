#include <cmath>

#include "ATKSideChainCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 3;

enum EParams
{
  kMiddleside = 0, // do we process data with middle/side or plain L/R ?
  kLinkChannels,
  kActivateChannel1,
  kActivateChannel2,
  kPower1,
  kAttack1,
  kRelease1,
  kThreshold1,
  kRatio1,
  kSoftness1,
  kColor1,
  kQuality1,
  kMakeup1,
  kPower2,
  kAttack2,
  kRelease2,
  kThreshold2,
  kRatio2,
  kSoftness2,
  kColor2,
  kQuality2,
  kMakeup2,
  kSideChain,
  kDryWet,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kMiddlesideX = 40,
  kMiddlesideY = 82,
  kLinkChannelsX = 40,
  kLinkChannelsY = 202,

  kActivateChannel1X = 143,
  kActivateChannel1Y = 82,
  kActivateChannel2X = 143,
  kActivateChannel2Y = 202,

  kPower1X = 233,
  kPower1Y = 40,
  kAttack1X = 336,
  kAttack1Y = 40,
  kRelease1X = 439,
  kRelease1Y = 40,
  kThreshold1X = 542,
  kThreshold1Y = 40,
  kRatio1X = 645,
  kRatio1Y = 40,
  kSoftness1X = 748,
  kSoftness1Y = 40,
  kColor1X = 851,
  kColor1Y = 40,
  kQuality1X = 954,
  kQuality1Y = 40,
  kMakeup1X = 1057,
  kMakeup1Y = 40,

  kPower2X = 233,
  kPower2Y = 160,
  kAttack2X = 336,
  kAttack2Y = 160,
  kRelease2X = 439,
  kRelease2Y = 160,
  kThreshold2X = 542,
  kThreshold2Y = 160,
  kRatio2X = 645,
  kRatio2Y = 160,
  kSoftness2X = 748,
  kSoftness2Y = 160,
  kColor2X = 851,
  kColor2Y = 160,
  kQuality2X = 954,
  kQuality2Y = 160,
  kMakeup2X = 1057,
  kMakeup2Y = 160,

  kSideChainX = 1160,
  kSideChainY = 82,
  kDryWetX = 1160,
  kDryWetY = 160,

  kKnobFrames = 20,
  kKnobFrames1 = 19
};

ATKSideChainCompressor::ATKSideChainCompressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
  inLFilter(nullptr, 1, 0, false), inRFilter(nullptr, 1, 0, false), inSideChainLFilter(nullptr, 1, 0, false), inSideChainRFilter(nullptr, 1, 0, false),
  volumesplitFilter(4), applyGainFilter(2), volumemergeFilter(2), drywetFilter(2), outLFilter(nullptr, 1, 0, false), outRFilter(nullptr, 1, 0, false), sidechain(false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kMiddleside)->InitBool("Middle/Side processing", 0, "");
  GetParam(kLinkChannels)->InitBool("Link channels", 0, "");
  GetParam(kActivateChannel1)->InitBool("Enable Channel 1", 1, "");
  GetParam(kActivateChannel2)->InitBool("Enable Channel 2", 1, "");

  GetParam(kPower1)->InitDouble("RMS ch1", 10., 1., 100.0, 0.1, "ms");
  GetParam(kPower1)->SetShape(2.);
  GetParam(kAttack1)->InitDouble("Attack ch1", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack1)->SetShape(2.);
  GetParam(kRelease1)->InitDouble("Release ch1", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease1)->SetShape(2.);
  GetParam(kThreshold1)->InitDouble("Threshold ch1", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kRatio1)->InitDouble("Ratio ch1", 2., .1, 100, .1, "-");
  GetParam(kRatio1)->SetShape(2.);
  GetParam(kSoftness1)->InitDouble("Softness ch1", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness1)->SetShape(2.);
  GetParam(kColor1)->InitDouble("Color ch1", 0, -.5, .5, 0.01, "-");
  GetParam(kQuality1)->InitDouble("Quality ch1", 0.1, 0., .2, 0.01, "-");
  GetParam(kMakeup1)->InitDouble("Makeup Gain ch1", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup1)->SetShape(2.);
  GetParam(kPower2)->InitDouble("RMS ch2", 10., 1., 100.0, 0.1, "ms");
  GetParam(kPower2)->SetShape(2.);
  GetParam(kAttack2)->InitDouble("Attack ch2", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack2)->SetShape(2.);
  GetParam(kRelease2)->InitDouble("Release ch2", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease2)->SetShape(2.);
  GetParam(kThreshold2)->InitDouble("Threshold ch2", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kRatio2)->InitDouble("Ratio ch2", 2., .1, 100, .1, "-");
  GetParam(kRatio2)->SetShape(2.);
  GetParam(kSoftness2)->InitDouble("Softness ch2", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness2)->SetShape(2.);
  GetParam(kColor2)->InitDouble("Color ch2", 0, -.5, .5, 0.01, "-");
  GetParam(kQuality2)->InitDouble("Quality ch2", 0.1, 0., .2, 0.01, "-");
  GetParam(kMakeup2)->InitDouble("Makeup Gain ch2", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup2)->SetShape(2.);
  GetParam(kDryWet)->InitDouble("Dry/Wet", 1, 0, 1, 0.01, "-");
  GetParam(kDryWet)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(STEREO_COMPRESSOR_ID, STEREO_COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames1);
  IBitmap myswitch = pGraphics->LoadIBitmap(SWITCH_ID, SWITCH_FN, 2);
  IColor color = IColor(255, 255, 255, 255);
  IText text = IText(10, &color, nullptr, IText::kStyleBold);

  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kPower1X, kPower1Y, kPower1X + 78, kPower1Y + 78 + 21), kPower1, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttack1X, kAttack1Y, kAttack1X + 78, kAttack1Y + 78 + 21), kAttack1, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kRelease1X, kRelease1Y, kRelease1X + 78, kRelease1Y + 78 + 21), kRelease1, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThreshold1X, kThreshold1Y, kThreshold1X + 78, kThreshold1Y + 78 + 21), kThreshold1, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kRatio1X, kRatio1Y, kRatio1X + 78, kRatio1Y + 78 + 21), kRatio1, &knob, &text, ""));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftness1X, kSoftness1Y, kSoftness1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kColor1X, kColor1Y, kColor1, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kQuality1X, kQuality1Y, kQuality1, &knob));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kMakeup1X, kMakeup1Y, kMakeup1X + 78, kMakeup1Y + 78 + 21), kMakeup1, &knob, &text, "dB"));

  power2 = new IKnobMultiControlText(this, IRECT(kPower2X, kPower2Y, kPower2X + 78, kPower2Y + 78 + 21), kPower2, &knob, &text, "ms");
  pGraphics->AttachControl(power2);
  attack2 = new IKnobMultiControlText(this, IRECT(kAttack2X, kAttack2Y, kAttack2X + 78, kAttack2Y + 78 + 21), kAttack2, &knob, &text, "ms");
  pGraphics->AttachControl(attack2);
  release2 = new IKnobMultiControlText(this, IRECT(kRelease2X, kRelease2Y, kRelease2X + 78, kRelease2Y + 78 + 21), kRelease2, &knob, &text, "ms");
  pGraphics->AttachControl(release2);
  threshold2 = new IKnobMultiControlText(this, IRECT(kThreshold2X, kThreshold2Y, kThreshold2X + 78, kThreshold2Y + 78 + 21), kThreshold2, &knob, &text, "dB");
  pGraphics->AttachControl(threshold2);
  ratio2 = new IKnobMultiControlText(this, IRECT(kRatio2X, kRatio2Y, kRatio2X + 78, kRatio2Y + 78 + 21), kRatio2, &knob, &text, "");
  pGraphics->AttachControl(ratio2);
  softness2 = new IKnobMultiControl(this, kSoftness2X, kSoftness2Y, kSoftness2, &knob);
  pGraphics->AttachControl(softness2);
  color2 = new IKnobMultiControl(this, kColor2X, kColor2Y, kColor2, &knob1);
  pGraphics->AttachControl(color2);
  quality2 = new IKnobMultiControl(this, kQuality2X, kQuality2Y, kQuality2, &knob);
  pGraphics->AttachControl(quality2);
  makeup2 = new IKnobMultiControlText(this, IRECT(kMakeup2X, kMakeup2Y, kMakeup2X + 78, kMakeup2Y + 78 + 21), kMakeup2, &knob, &text, "dB");
  pGraphics->AttachControl(makeup2);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDryWetX, kDryWetY, kDryWet, &knob1));

  pGraphics->AttachControl(new ISwitchControl(this, kMiddlesideX, kMiddlesideY, kMiddleside, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kLinkChannelsX, kLinkChannelsY, kLinkChannels, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kActivateChannel1X, kActivateChannel1Y, kActivateChannel1, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kActivateChannel2X, kActivateChannel2Y, kActivateChannel2, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kSideChainX, kSideChainY, kSideChain, &myswitch));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakePreset("Serial compression", false, false, true, true, 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 0., 0.);
  MakePreset("Middle/side compression", true, false, true, true, 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 0., 0.);
  MakePreset("Parallel compression", false, false, false, false, 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 10., 10., 10., 0., 2., -2., 0., 0.1, 0., 0.5, 0.);

  volumesplitFilter.set_volume(std::sqrt(.5));
  volumemergeFilter.set_volume(std::sqrt(.5));
  endpoint.add_filter(&outLFilter);
  endpoint.add_filter(&outRFilter);

  powerFilter1.set_input_port(0, &inSideChainLFilter, 0);
  gainColoredCompressorFilter1.set_input_port(0, &powerFilter1, 0);
  attackReleaseFilter1.set_input_port(0, &gainColoredCompressorFilter1, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter1, 0);
  applyGainFilter.set_input_port(1, &inLFilter, 0);
  makeupFilter1.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &makeupFilter1, 0);
  drywetFilter.set_input_port(1, &inLFilter, 0);
  outLFilter.set_input_port(0, &drywetFilter, 0);

  powerFilter2.set_input_port(0, &inSideChainRFilter, 0);
  gainColoredCompressorFilter2.set_input_port(0, &powerFilter2, 0);
  attackReleaseFilter2.set_input_port(0, &gainColoredCompressorFilter2, 0);
  applyGainFilter.set_input_port(2, &attackReleaseFilter2, 0);
  applyGainFilter.set_input_port(3, &inRFilter, 0);
  makeupFilter2.set_input_port(0, &applyGainFilter, 1);
  drywetFilter.set_input_port(2, &makeupFilter2, 0);
  drywetFilter.set_input_port(3, &inRFilter, 0);
  outRFilter.set_input_port(0, &drywetFilter, 1);

  middlesidesplitFilter.set_input_port(0, &inLFilter, 0);
  middlesidesplitFilter.set_input_port(1, &inRFilter, 0);
  sidechainmiddlesidesplitFilter.set_input_port(0, &inSideChainLFilter, 0);
  sidechainmiddlesidesplitFilter.set_input_port(1, &inSideChainRFilter, 0);
  volumesplitFilter.set_input_port(0, &middlesidesplitFilter, 0);
  volumesplitFilter.set_input_port(1, &middlesidesplitFilter, 1);
  volumesplitFilter.set_input_port(2, &sidechainmiddlesidesplitFilter, 0);
  volumesplitFilter.set_input_port(3, &sidechainmiddlesidesplitFilter, 1);
  middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
  middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
  volumemergeFilter.set_input_port(0, &middlesidemergeFilter, 0);
  volumemergeFilter.set_input_port(1, &middlesidemergeFilter, 1);
  sumFilter.set_input_port(0, &powerFilter1, 0);
  sumFilter.set_input_port(1, &powerFilter2, 0);

  Reset();
}

ATKSideChainCompressor::~ATKSideChainCompressor() {}

void ATKSideChainCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  if (sidechain)
  {
    inSideChainLFilter.set_pointer(inputs[2], nFrames);
  }
  else
  {
    inSideChainLFilter.set_pointer(inputs[0], nFrames);
  }
  if (sidechain)
  {
    inSideChainRFilter.set_pointer(inputs[3], nFrames);
  }
  else
  {
    inSideChainRFilter.set_pointer(inputs[1], nFrames);
  }
  inLFilter.set_pointer(inputs[0], nFrames);
  outLFilter.set_pointer(outputs[0], nFrames);
  inRFilter.set_pointer(inputs[1], nFrames);
  outRFilter.set_pointer(outputs[1], nFrames);
  endpoint.process(nFrames);
}

void ATKSideChainCompressor::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();

  if (sampling_rate != endpoint.get_input_sampling_rate())
  {
    inLFilter.set_input_sampling_rate(sampling_rate);
    inLFilter.set_output_sampling_rate(sampling_rate);
    inRFilter.set_input_sampling_rate(sampling_rate);
    inRFilter.set_output_sampling_rate(sampling_rate);
    inSideChainLFilter.set_input_sampling_rate(sampling_rate);
    inSideChainLFilter.set_output_sampling_rate(sampling_rate);
    inSideChainRFilter.set_input_sampling_rate(sampling_rate);
    inSideChainRFilter.set_output_sampling_rate(sampling_rate);
    outLFilter.set_input_sampling_rate(sampling_rate);
    outLFilter.set_output_sampling_rate(sampling_rate);
    outRFilter.set_input_sampling_rate(sampling_rate);
    outRFilter.set_output_sampling_rate(sampling_rate);

    middlesidesplitFilter.set_input_sampling_rate(sampling_rate);
    middlesidesplitFilter.set_output_sampling_rate(sampling_rate);
    sidechainmiddlesidesplitFilter.set_input_sampling_rate(sampling_rate);
    sidechainmiddlesidesplitFilter.set_output_sampling_rate(sampling_rate);
    volumesplitFilter.set_input_sampling_rate(sampling_rate);
    volumesplitFilter.set_output_sampling_rate(sampling_rate);
    middlesidemergeFilter.set_input_sampling_rate(sampling_rate);
    middlesidemergeFilter.set_output_sampling_rate(sampling_rate);
    volumemergeFilter.set_input_sampling_rate(sampling_rate);
    volumemergeFilter.set_output_sampling_rate(sampling_rate);
    sumFilter.set_input_sampling_rate(sampling_rate);
    sumFilter.set_output_sampling_rate(sampling_rate);

    powerFilter1.set_input_sampling_rate(sampling_rate);
    powerFilter1.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter1.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter1.set_output_sampling_rate(sampling_rate);
    gainColoredCompressorFilter1.set_input_sampling_rate(sampling_rate);
    gainColoredCompressorFilter1.set_output_sampling_rate(sampling_rate);
    makeupFilter1.set_input_sampling_rate(sampling_rate);
    makeupFilter1.set_output_sampling_rate(sampling_rate);

    powerFilter2.set_input_sampling_rate(sampling_rate);
    powerFilter2.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_output_sampling_rate(sampling_rate);
    gainColoredCompressorFilter2.set_input_sampling_rate(sampling_rate);
    gainColoredCompressorFilter2.set_output_sampling_rate(sampling_rate);
    makeupFilter2.set_input_sampling_rate(sampling_rate);
    makeupFilter2.set_output_sampling_rate(sampling_rate);

    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    drywetFilter.set_input_sampling_rate(sampling_rate);
    drywetFilter.set_output_sampling_rate(sampling_rate);
    endpoint.set_input_sampling_rate(sampling_rate);
    endpoint.set_output_sampling_rate(sampling_rate);

    auto power = GetParam(kPower1)->Value();
    if (power == 0)
    {
      powerFilter1.set_memory(0);
    }
    else
    {
      powerFilter1.set_memory(std::exp(-1e3 / (power * sampling_rate)));
    }
    power = GetParam(kPower2)->Value();
    if (power == 0)
    {
      powerFilter2.set_memory(0);
    }
    else
    {
      powerFilter2.set_memory(std::exp(-1e3 / (power * sampling_rate)));
    }

    attackReleaseFilter1.set_release(std::exp(-1 / (GetParam(kAttack1)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter1.set_attack(std::exp(-1 / (GetParam(kRelease1)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter2.set_release(std::exp(-1 / (GetParam(kAttack2)->Value() * 1e-3 * sampling_rate))); // in ms
    attackReleaseFilter2.set_attack(std::exp(-1 / (GetParam(kRelease2)->Value() * 1e-3 * sampling_rate))); // in ms
  }
  attackReleaseFilter1.full_setup();
  attackReleaseFilter2.full_setup();
}

void ATKSideChainCompressor::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
  case kMiddleside:
    if (GetParam(kMiddleside)->Bool())
    {
      powerFilter1.set_input_port(0, &volumesplitFilter, 2);
      powerFilter2.set_input_port(0, &volumesplitFilter, 3);
      drywetFilter.set_input_port(0, &volumemergeFilter, 0);
      drywetFilter.set_input_port(2, &volumemergeFilter, 1);
      applyGainFilter.set_input_port(1, &volumesplitFilter, 0);
      applyGainFilter.set_input_port(3, &volumesplitFilter, 1);
      if (GetParam(kActivateChannel1)->Bool())
      {
        middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
      }
      else
      {
        middlesidemergeFilter.set_input_port(0, &volumesplitFilter, 0);
      }
      if (GetParam(kActivateChannel2)->Bool())
      {
        middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
      }
      else
      {
        middlesidemergeFilter.set_input_port(1, &volumesplitFilter, 1);
      }
    }
    else
    {
      powerFilter1.set_input_port(0, &inSideChainLFilter, 0);
      powerFilter2.set_input_port(0, &inSideChainRFilter, 0);
      applyGainFilter.set_input_port(1, &inLFilter, 0);
      applyGainFilter.set_input_port(3, &inRFilter, 0);
      if (GetParam(kActivateChannel1)->Bool())
      {
        drywetFilter.set_input_port(0, &makeupFilter1, 0);
      }
      else
      {
        drywetFilter.set_input_port(0, &inLFilter, 0);
      }
      if (GetParam(kActivateChannel2)->Bool())
      {
        drywetFilter.set_input_port(2, &makeupFilter2, 0);
      }
      else
      {
        drywetFilter.set_input_port(2, &inRFilter, 0);
      }
    }
    break;
  case kLinkChannels:
    if (GetParam(kLinkChannels)->Bool())
    {
      gainColoredCompressorFilter1.set_input_port(0, &sumFilter, 0);
      applyGainFilter.set_input_port(2, &attackReleaseFilter1, 0);
      makeupFilter2.set_volume_db(GetParam(kMakeup1)->Value());

      power2->GrayOut(true);
      attack2->GrayOut(true);
      release2->GrayOut(true);
      threshold2->GrayOut(true);
      ratio2->GrayOut(true);
      softness2->GrayOut(true);
      color2->GrayOut(true);
      quality2->GrayOut(true);
      makeup2->GrayOut(true);
    }
    else
    {
      gainColoredCompressorFilter1.set_input_port(0, &powerFilter1, 0);
      applyGainFilter.set_input_port(2, &attackReleaseFilter2, 0);
      makeupFilter2.set_volume_db(GetParam(kMakeup2)->Value());

      power2->GrayOut(false);
      attack2->GrayOut(false);
      release2->GrayOut(false);
      threshold2->GrayOut(false);
      ratio2->GrayOut(false);
      softness2->GrayOut(false);
      color2->GrayOut(false);
      quality2->GrayOut(false);
      makeup2->GrayOut(false);
    }
    break;
  case kActivateChannel1:
    if (GetParam(kActivateChannel1)->Bool())
    {
      middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
      if (GetParam(kMiddleside)->Bool())
      {
        drywetFilter.set_input_port(0, &volumemergeFilter, 0);
      }
      else
      {
        drywetFilter.set_input_port(0, &makeupFilter1, 0);
      }
    }
    else
    {
      middlesidemergeFilter.set_input_port(0, &volumesplitFilter, 0);
      if (GetParam(kMiddleside)->Bool())
      {
        drywetFilter.set_input_port(0, &volumemergeFilter, 0);
      }
      else
      {
        drywetFilter.set_input_port(0, &inLFilter, 0);
      }
    }
    break;
  case kActivateChannel2:
    if (GetParam(kActivateChannel2)->Bool())
    {
      middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
      if (GetParam(kMiddleside)->Bool())
      {
        drywetFilter.set_input_port(2, &volumemergeFilter, 1);
      }
      else
      {
        drywetFilter.set_input_port(2, &makeupFilter2, 0);
      }
    }
    else
    {
      middlesidemergeFilter.set_input_port(1, &volumesplitFilter, 1);
      if (GetParam(kMiddleside)->Bool())
      {
        drywetFilter.set_input_port(2, &volumemergeFilter, 1);
      }
      else
      {
        drywetFilter.set_input_port(2, &inRFilter, 0);
      }
    }
    break;

  case kThreshold1:
    gainColoredCompressorFilter1.set_threshold(std::pow(10, GetParam(kThreshold1)->Value() / 10));
    break;
  case kRatio1:
    gainColoredCompressorFilter1.set_ratio(GetParam(kRatio1)->Value());
    break;
  case kSoftness1:
    gainColoredCompressorFilter1.set_softness(std::pow(10, GetParam(kSoftness1)->Value()));
    break;
  case kColor1:
    gainColoredCompressorFilter1.set_color(GetParam(kColor1)->Value());
    break;
  case kQuality1:
    gainColoredCompressorFilter1.set_quality(GetParam(kQuality1)->Value());
    break;
  case kPower1:
    {
      auto power = GetParam(kPower1)->Value();
      if (power == 0)
      {
        powerFilter1.set_memory(0);
      }
      else
      {
        powerFilter1.set_memory(std::exp(-1e3 / (power * GetSampleRate())));
      }
    }
    break;
  case kAttack1:
    attackReleaseFilter1.set_release(std::exp(-1 / (GetParam(kAttack1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kRelease1:
    attackReleaseFilter1.set_attack(std::exp(-1 / (GetParam(kRelease1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kMakeup1:
    makeupFilter1.set_volume_db(GetParam(kMakeup1)->Value());
    if (GetParam(kLinkChannels)->Bool())
    {
      makeupFilter2.set_volume_db(GetParam(kMakeup1)->Value());
    }
    break;
  case kThreshold2:
    gainColoredCompressorFilter2.set_threshold(std::pow(10, GetParam(kThreshold2)->Value() / 10));
    break;
  case kRatio2:
    gainColoredCompressorFilter2.set_ratio(GetParam(kRatio2)->Value());
    break;
  case kSoftness2:
    gainColoredCompressorFilter2.set_softness(std::pow(10, GetParam(kSoftness2)->Value()));
    break;
  case kColor2:
    gainColoredCompressorFilter2.set_color(GetParam(kColor2)->Value());
    break;
  case kQuality2:
    gainColoredCompressorFilter2.set_quality(GetParam(kQuality2)->Value());
    break;
  case kPower2:
    {
      auto power = GetParam(kPower2)->Value();
      if (power == 0)
      {
        powerFilter2.set_memory(0);
      }
      else
      {
        powerFilter2.set_memory(std::exp(-1e3 / (power * GetSampleRate())));
      }
    }
    break;
  case kAttack2:
    attackReleaseFilter2.set_release(std::exp(-1 / (GetParam(kAttack2)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kRelease2:
    attackReleaseFilter2.set_attack(std::exp(-1 / (GetParam(kRelease2)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kMakeup2:
    makeupFilter2.set_volume_db(GetParam(kMakeup2)->Value());
    break;
  case kDryWet:
    drywetFilter.set_dry(GetParam(kDryWet)->Value());
    break;
  case kSideChain:
    sidechain = GetParam(kSideChain)->Bool();
    break;

  default:
    break;
  }
}
