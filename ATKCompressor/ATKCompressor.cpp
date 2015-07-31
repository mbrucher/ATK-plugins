#include <cmath>

#include "ATKCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 2;

enum EParams
{
  kAttack = 0,
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

  kAttackX = 25,
  kAttackY = 26,
  kReleaseX = 94,
  kReleaseY = 26,
  kThresholdX = 163,
  kThresholdY = 26,
  kSlopeX = 232,
  kSlopeY = 26,
  kSoftnessX = 301,
  kSoftnessY = 26,
  kMakeupX = 370,
  kMakeupY = 26,
  kDryWetX = 439,
  kDryWetY = 26,
  kKnobFrames = 43
};

ATKCompressor::ATKCompressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
    inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kAttack)->InitDouble("Attack", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack)->SetShape(2.);
  GetParam(kRelease)->InitDouble("Release", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kThreshold)->InitDouble("Threshold", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold)->SetShape(2.);
  GetParam(kSlope)->InitDouble("Slope", 2., 1, 100, 1, "-");
  GetParam(kSlope)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Softness", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);
  GetParam(kMakeup)->InitDouble("Makeup Gain", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup)->SetShape(2.);
  GetParam(kDryWet)->InitDouble("Dry/Wet", 1, 0, 1, 0.01, "-");
  GetParam(kDryWet)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(COMPRESSOR_ID, COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames);
  IText text = IText(10, 0, 0, IText::kStyleBold);
  
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttackX, kAttackY, kAttackX + 43, kAttackY + 43 + 21), kAttack, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseX, kReleaseY, kReleaseX + 43, kReleaseY + 43 + 21), kRelease, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThresholdX, kThresholdY, kThresholdX + 43, kThresholdY + 43 + 21), kThreshold, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kSlopeX, kSlopeY, kSlopeX + 43, kSlopeY + 43 + 21), kSlope, &knob, &text, ""));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kMakeupX, kMakeupY, kMakeupX + 43, kMakeupY + 43 + 21), kMakeup, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDryWetX, kDryWetY, kDryWet, &knob1));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakePreset("Serial Compression", 10., 10., 0., 2., -2., 0., 0.);
  MakePreset("Parallel Compression", 10., 10., 0., 2., -2., 0., 0.5);
  
  powerFilter.set_input_port(0, &inFilter, 0);
  gainCompressorFilter.set_input_port(0, &powerFilter, 0);
  attackReleaseFilter.set_input_port(0, &gainCompressorFilter, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(0, &volumeFilter, 0);
  drywetFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &drywetFilter, 0);
  
  powerFilter.set_memory(0);

  Reset();
}

ATKCompressor::~ATKCompressor() {}

void ATKCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKCompressor::Reset()
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
    
    attackReleaseFilter.set_release(std::exp(-1e3 / (GetParam(kAttack)->Value() * sampling_rate))); // in ms
    attackReleaseFilter.set_attack(std::exp(-1e3 / (GetParam(kRelease)->Value() * sampling_rate))); // in ms
  }
  
  powerFilter.full_setup();
  attackReleaseFilter.full_setup();
}

void ATKCompressor::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      gainCompressorFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      break;
    case kSlope:
      gainCompressorFilter.set_ratio(GetParam(kSlope)->Value());
      break;
    case kSoftness:
      gainCompressorFilter.set_softness(std::pow(10, GetParam(kSoftness)->Value()));
      break;
    case kAttack:
      attackReleaseFilter.set_release(std::exp(-1e3 / (GetParam(kAttack)->Value() * GetSampleRate()))); // in ms
      break;
    case kRelease:
      attackReleaseFilter.set_attack(std::exp(-1e3 / (GetParam(kRelease)->Value() * GetSampleRate()))); // in ms
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
