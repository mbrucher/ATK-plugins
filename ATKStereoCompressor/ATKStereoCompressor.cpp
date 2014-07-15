#include <cmath>

#include "ATKStereoCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kMiddleside = 0, // do we process data with middle/side or plain L/R ?
  kLinkChannels,
  kActivateChannel1,
  kActivateChannel2,
  kAttack1,
  kRelease1,
  kThreshold1,
  kSlope1,
  kSoftness1,
  kMakeup1,
  kAttack2,
  kRelease2,
  kThreshold2,
  kSlope2,
  kSoftness2,
  kMakeup2,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kMiddlesideX = 25,
  kMiddlesideY = 36,
  kLinkChannelsX = 94,
  kLinkChannelsY = 36,
  kActivateChannel1X = 25,
  kActivateChannel1Y = 121,
  kActivateChannel2X = 94,
  kActivateChannel2Y = 121,

  kAttack1X = 163,
  kAttack1Y = 26,
  kRelease1X = 232,
  kRelease1Y = 26,
  kThreshold1X = 301,
  kThreshold1Y = 26,
  kSlope1X = 370,
  kSlope1Y = 26,
  kSoftness1X = 439,
  kSoftness1Y = 26,
  kMakeup1X = 508,
  kMakeup1Y = 26,

  kAttack2X = 163,
  kAttack2Y = 111,
  kRelease2X = 232,
  kRelease2Y = 111,
  kThreshold2X = 301,
  kThreshold2Y = 111,
  kSlope2X = 370,
  kSlope2Y = 111,
  kSoftness2X = 439,
  kSoftness2Y = 111,
  kMakeup2X = 508,
  kMakeup2Y = 111,

  kKnobFrames = 43
};

ATKStereoCompressor::ATKStereoCompressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
  inLFilter(NULL, 1, 0, false), inRFilter(NULL, 1, 0, false), volumemergeFilter(2), outLFilter(NULL, 1, 0, false), outRFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kMiddleside)->InitBool("Middle/Side processing", 0, "");
  GetParam(kLinkChannels)->InitBool("Link channels", 0, "");
  GetParam(kActivateChannel1)->InitBool("Enable Channel 1", 1, "");
  GetParam(kActivateChannel2)->InitBool("Enable Channel 2", 1, "");

  GetParam(kAttack1)->InitDouble("Attack ch1", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack1)->SetShape(2.);
  GetParam(kRelease1)->InitDouble("Release ch1", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease1)->SetShape(2.);
  GetParam(kThreshold1)->InitDouble("Threshold ch1", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold1)->SetShape(2.);
  GetParam(kSlope1)->InitDouble("Slope ch1", 2., 1, 100, 1, "-");
  GetParam(kSlope1)->SetShape(2.);
  GetParam(kSoftness1)->InitDouble("Softness ch1", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness1)->SetShape(2.);
  GetParam(kMakeup1)->InitDouble("Makeup Gain ch1", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup1)->SetShape(2.);
  GetParam(kAttack2)->InitDouble("Attack ch2", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack2)->SetShape(2.);
  GetParam(kRelease2)->InitDouble("Release ch2", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease2)->SetShape(2.);
  GetParam(kThreshold2)->InitDouble("Threshold ch2", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold2)->SetShape(2.);
  GetParam(kSlope2)->InitDouble("Slope ch2", 2., 1, 100, 1, "-");
  GetParam(kSlope2)->SetShape(2.);
  GetParam(kSoftness2)->InitDouble("Softness ch2", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness2)->SetShape(2.);
  GetParam(kMakeup2)->InitDouble("Makeup Gain ch2", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup2)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(STEREO_COMPRESSOR_ID, STEREO_COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap myswitch = pGraphics->LoadIBitmap(SWITCH_ID, SWITCH_FN, 2);

  pGraphics->AttachControl(new IKnobMultiControl(this, kAttack1X, kAttack1Y, kAttack1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kRelease1X, kRelease1Y, kRelease1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kThreshold1X, kThreshold1Y, kThreshold1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSlope1X, kSlope1Y, kSlope1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftness1X, kSoftness1Y, kSoftness1, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMakeup1X, kMakeup1Y, kMakeup1, &knob));

  pGraphics->AttachControl(new IKnobMultiControl(this, kAttack2X, kAttack2Y, kAttack2, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kRelease2X, kRelease2Y, kRelease2, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kThreshold2X, kThreshold2Y, kThreshold2, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSlope2X, kSlope2Y, kSlope2, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftness2X, kSoftness2Y, kSoftness2, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kMakeup2X, kMakeup2Y, kMakeup2, &knob));

  pGraphics->AttachControl(new ISwitchControl(this, kMiddlesideX, kMiddlesideY, kMiddleside, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kLinkChannelsX, kLinkChannelsY, kLinkChannels, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kActivateChannel1X, kActivateChannel1Y, kActivateChannel1, &myswitch));
  pGraphics->AttachControl(new ISwitchControl(this, kActivateChannel2X, kActivateChannel2Y, kActivateChannel2, &myswitch));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  volumemergeFilter.set_volume(.5);
  endpoint.add_filter(&outLFilter);
  endpoint.add_filter(&outRFilter);

  powerFilter1.set_input_port(0, &inLFilter, 0);
  gainCompressorFilter1.set_input_port(0, &powerFilter1, 0);
  attackReleaseFilter1.set_input_port(0, &gainCompressorFilter1, 0);
  applyGainFilter1.set_input_port(0, &attackReleaseFilter1, 0);
  applyGainFilter1.set_input_port(1, &inLFilter, 0);
  makeupFilter1.set_input_port(0, &applyGainFilter1, 0);
  outLFilter.set_input_port(0, &makeupFilter1, 0);
  
  powerFilter2.set_input_port(0, &inRFilter, 0);
  gainCompressorFilter2.set_input_port(0, &powerFilter2, 0);
  attackReleaseFilter2.set_input_port(0, &gainCompressorFilter2, 0);
  applyGainFilter2.set_input_port(0, &attackReleaseFilter2, 0);
  applyGainFilter2.set_input_port(1, &inRFilter, 0);
  makeupFilter2.set_input_port(0, &applyGainFilter2, 0);
  outRFilter.set_input_port(0, &makeupFilter2, 0);

  middlesidesplitFilter.set_input_port(0, &inLFilter, 0);
  middlesidesplitFilter.set_input_port(1, &inRFilter, 0);
  middlesidemergeFilter.set_input_port(0, &makeupFilter1, 0);
  middlesidemergeFilter.set_input_port(1, &makeupFilter2, 0);
  volumemergeFilter.set_input_port(0, &middlesidemergeFilter, 0);
  volumemergeFilter.set_input_port(1, &middlesidemergeFilter, 1);
  sumFilter.set_input_port(0, &powerFilter1, 0);
  sumFilter.set_input_port(1, &powerFilter2, 0);

  Reset();
}

ATKStereoCompressor::~ATKStereoCompressor() {}

void ATKStereoCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inLFilter.set_pointer(inputs[0], nFrames);
  outLFilter.set_pointer(outputs[0], nFrames);
  inRFilter.set_pointer(inputs[1], nFrames);
  outRFilter.set_pointer(outputs[1], nFrames);
  endpoint.process(nFrames);
}

void ATKStereoCompressor::Reset()
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
    outLFilter.set_input_sampling_rate(sampling_rate);
    outLFilter.set_output_sampling_rate(sampling_rate);
    outRFilter.set_input_sampling_rate(sampling_rate);
    outRFilter.set_output_sampling_rate(sampling_rate);

    middlesidesplitFilter.set_input_sampling_rate(sampling_rate);
    middlesidesplitFilter.set_output_sampling_rate(sampling_rate);
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
    gainCompressorFilter1.set_input_sampling_rate(sampling_rate);
    gainCompressorFilter1.set_output_sampling_rate(sampling_rate);
    applyGainFilter1.set_input_sampling_rate(sampling_rate);
    applyGainFilter1.set_output_sampling_rate(sampling_rate);
    makeupFilter1.set_input_sampling_rate(sampling_rate);
    makeupFilter1.set_output_sampling_rate(sampling_rate);

    powerFilter2.set_input_sampling_rate(sampling_rate);
    powerFilter2.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter2.set_output_sampling_rate(sampling_rate);
    gainCompressorFilter2.set_input_sampling_rate(sampling_rate);
    gainCompressorFilter2.set_output_sampling_rate(sampling_rate);
    applyGainFilter2.set_input_sampling_rate(sampling_rate);
    applyGainFilter2.set_output_sampling_rate(sampling_rate);
    makeupFilter2.set_input_sampling_rate(sampling_rate);
    makeupFilter2.set_output_sampling_rate(sampling_rate);

    endpoint.set_input_sampling_rate(sampling_rate);
    endpoint.set_output_sampling_rate(sampling_rate);

    powerFilter1.set_memory(std::exp(-1 / 1e-3 * GetSampleRate()));
    attackReleaseFilter1.set_release(std::exp(-1 / (GetParam(kAttack1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    attackReleaseFilter1.set_attack(std::exp(-1 / (GetParam(kRelease1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    powerFilter2.set_memory(std::exp(-1 / 1e-3 * GetSampleRate()));
    attackReleaseFilter2.set_release(std::exp(-1 / (GetParam(kAttack2)->Value() * 1e-3 * GetSampleRate()))); // in ms
    attackReleaseFilter2.set_attack(std::exp(-1 / (GetParam(kRelease2)->Value() * 1e-3 * GetSampleRate()))); // in ms
  }
}

void ATKStereoCompressor::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
  case kMiddleside:
    if (GetParam(kMiddleside)->Bool())
    {
      powerFilter1.set_input_port(0, &middlesidesplitFilter, 0);
      powerFilter2.set_input_port(0, &middlesidesplitFilter, 1);
      outLFilter.set_input_port(0, &volumemergeFilter, 0);
      outRFilter.set_input_port(0, &volumemergeFilter, 1);
    }
    else
    {
      powerFilter1.set_input_port(0, &inLFilter, 0);
      powerFilter2.set_input_port(0, &inRFilter, 0);
      outLFilter.set_input_port(0, &makeupFilter1, 0);
      outRFilter.set_input_port(0, &makeupFilter2, 0);
    }
    break;
  case kLinkChannels:
    if (GetParam(kLinkChannels)->Bool())
    {
      attackReleaseFilter1.set_input_port(0, &sumFilter, 0);
      applyGainFilter2.set_input_port(0, &attackReleaseFilter1, 0);
    }
    else
    {
      attackReleaseFilter1.set_input_port(0, &gainCompressorFilter1, 0);
      attackReleaseFilter2.set_input_port(0, &gainCompressorFilter2, 0);
      applyGainFilter2.set_input_port(0, &attackReleaseFilter2, 0);
    }
    break;
  case kActivateChannel1:
    if (GetParam(kActivateChannel1)->Bool())
    {
      outLFilter.set_input_port(0, &makeupFilter1, 0);
    }
    else
    {
      outLFilter.set_input_port(0, &inLFilter, 0);
    }
    break;
  case kActivateChannel2:
    if (GetParam(kActivateChannel2)->Bool())
    {
      outRFilter.set_input_port(0, &makeupFilter2, 0);
    }
    else
    {
      outRFilter.set_input_port(0, &inRFilter, 0);
    }
    break;

  case kThreshold1:
    gainCompressorFilter1.set_threshold(std::pow(10, GetParam(kThreshold1)->Value() / 10));
    break;
  case kSlope1:
    gainCompressorFilter1.set_slope(GetParam(kSlope1)->Value());
    break;
  case kSoftness1:
    gainCompressorFilter1.set_softness(std::pow(10, GetParam(kSoftness1)->Value()));
    break;
  case kAttack1:
    attackReleaseFilter1.set_release(std::exp(-1 / (GetParam(kAttack1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kRelease1:
    attackReleaseFilter1.set_attack(std::exp(-1 / (GetParam(kRelease1)->Value() * 1e-3 * GetSampleRate()))); // in ms
    break;
  case kMakeup1:
    makeupFilter1.set_volume_db(GetParam(kMakeup1)->Value());
    break;
  case kThreshold2:
    gainCompressorFilter2.set_threshold(std::pow(10, GetParam(kThreshold2)->Value() / 10));
    break;
  case kSlope2:
    gainCompressorFilter2.set_slope(GetParam(kSlope2)->Value());
    break;
  case kSoftness2:
    gainCompressorFilter2.set_softness(std::pow(10, GetParam(kSoftness2)->Value()));
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

    default:
      break;
  }
}
