#include <cmath>

#include "ATKColoredCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 2;

enum EParams
{
  kPower = 0,
  kAttack,
  kRelease,
  kThreshold,
  kSlope,
  kSoftness,
  kMakeup,
  kDryWet,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kPowerX = 27,
  kPowerY = 40,
  kAttackX = 130,
  kAttackY = 40,
  kReleaseX = 233,
  kReleaseY = 40,
  kThresholdX = 336,
  kThresholdY = 40,
  kSlopeX = 439,
  kSlopeY = 40,
  kSoftnessX = 542,
  kSoftnessY = 40,
  kMakeupX = 645,
  kMakeupY = 40,
  kDryWetX = 748,
  kDryWetY = 40,
  
  kKnobFrames = 20,
  kKnobFrames1 = 19
};

ATKColoredCompressor::ATKColoredCompressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
inFilter(nullptr, 1, 0, false), outFilter(nullptr, 1, 0, false)
{
  TRACE;
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kPower)->InitDouble("Power", 10., 0.1, 100.0, 0.1, "ms");
  GetParam(kPower)->SetShape(2.);
  GetParam(kAttack)->InitDouble("Attack", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack)->SetShape(2.);
  GetParam(kRelease)->InitDouble("Release", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kThreshold)->InitDouble("Threshold", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold)->SetShape(2.);
  GetParam(kSlope)->InitDouble("Slope", 2., .1, 100, .1, "-");
  GetParam(kSlope)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Color", 1, -.5, 2, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);
  GetParam(kMakeup)->InitDouble("Makeup Gain", 0, 0, 40, 0.1, "dB"); // Makeup is expressed in amplitude
  GetParam(kMakeup)->SetShape(2.);
  GetParam(kDryWet)->InitDouble("Dry/Wet", 1, 0, 1, 0.01, "-");
  GetParam(kDryWet)->SetShape(1.);
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(COLORED_COMPRESSOR_ID, COLORED_COMPRESSOR_FN);
  
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames1);
  IColor color = IColor(255, 255, 255, 255);
  IText text = IText(10, &color, nullptr, IText::kStyleBold);
  
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kPowerX, kPowerY, kPowerX + 78, kPowerY + 78 + 21), kPower, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttackX, kAttackY, kAttackX + 78, kAttackY + 78 + 21), kAttack, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseX, kReleaseY, kReleaseX + 78, kReleaseY + 78 + 21), kRelease, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThresholdX, kThresholdY, kThresholdX + 78, kThresholdY + 78 + 21), kThreshold, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kSlopeX, kSlopeY, kSlopeX + 78, kSlopeY + 78 + 21), kSlope, &knob, &text, ""));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kMakeupX, kMakeupY, kMakeupX + 78, kMakeupY + 78 + 21), kMakeup, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDryWetX, kDryWetY, kDryWet, &knob1));
  
  AttachGraphics(pGraphics);
  
  //MakePreset("preset 1", ... );
  MakePreset("Serial Compression", 10., 10., 10., 0., 2., 1., 0., 1.);
  MakePreset("Parallel Compression", 10., 10., 10., 0., 2., 1., 0., 0.5);
  
  powerFilter.set_input_port(0, &inFilter, 0);
  gainCompressorFilter.set_input_port(0, &powerFilter, 0);
  attackReleaseFilter.set_input_port(0, &gainCompressorFilter, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &volumeFilter, 0);
  drywetFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &drywetFilter, 0);
  
  Reset();
}

ATKColoredCompressor::~ATKColoredCompressor() {}

void ATKColoredCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKColoredCompressor::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  if(sampling_rate != outFilter.get_output_sampling_rate())
  {
    inFilter.set_input_sampling_rate(sampling_rate);
    inFilter.set_output_sampling_rate(sampling_rate);
    powerFilter.set_input_sampling_rate(sampling_rate);
    powerFilter.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter.set_output_sampling_rate(sampling_rate);
    gainCompressorFilter.set_input_sampling_rate(sampling_rate);
    gainCompressorFilter.set_output_sampling_rate(sampling_rate);
    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    volumeFilter.set_input_sampling_rate(sampling_rate);
    volumeFilter.set_output_sampling_rate(sampling_rate);
    drywetFilter.set_input_sampling_rate(sampling_rate);
    drywetFilter.set_output_sampling_rate(sampling_rate);
    outFilter.set_input_sampling_rate(sampling_rate);
    outFilter.set_output_sampling_rate(sampling_rate);
    
    powerFilter.set_memory(std::exp(-GetSampleRate()/(GetParam(kPower)->Value() * 1e-3)));
    attackReleaseFilter.set_release(std::exp(-GetSampleRate() / (GetParam(kAttack)->Value() * 1e-3))); // in ms
    attackReleaseFilter.set_attack(std::exp(-GetSampleRate() / (GetParam(kRelease)->Value() * 1e-3))); // in ms
  }
  
  powerFilter.full_setup();
  attackReleaseFilter.full_setup();
}

void ATKColoredCompressor::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case kPower:
      powerFilter.set_memory(std::exp(-GetSampleRate()/(GetParam(kPower)->Value() * 1e-3)));
      break;
    case kThreshold:
      gainCompressorFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      break;
    case kSlope:
      gainCompressorFilter.set_ratio(GetParam(kSlope)->Value());
      break;
    case kSoftness:
      gainCompressorFilter.set_color(std::pow(10, GetParam(kSoftness)->Value()));
      break;
    case kAttack:
      attackReleaseFilter.set_release(std::exp(-GetSampleRate() / (GetParam(kAttack)->Value() * 1e-3))); // in ms
      break;
    case kRelease:
      attackReleaseFilter.set_attack(std::exp(-GetSampleRate() / (GetParam(kRelease)->Value() * 1e-3))); // in ms
      break;
    case kMakeup:
      volumeFilter.set_volume_db(GetParam(kMakeup)->Value());
      break;
    case kDryWet:
      drywetFilter.set_dry(GetParam(kDryWet)->Value());
      break;
      
    default:
      break;
  }
}
