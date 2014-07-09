#include <cmath>

#include "ATKExpander.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kAttack = 0,
  kRelease,
  kThreshold,
  kSlope,
  kSoftness,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kAttackX = 25,
  kAttackY = 31,
  kReleaseX = 94,
  kReleaseY = 31,
  kThresholdX = 163,
  kThresholdY = 31,
  kSlopeX = 232,
  kSlopeY = 31,
  kSoftnessX = 301,
  kSoftnessY = 31,
  kKnobFrames = 43
};

ATKExpander::ATKExpander(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
    inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kAttack)->InitDouble("Attack", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack)->SetShape(2.);
  GetParam(kRelease)->InitDouble("Release", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kThreshold)->InitDouble("Threshold", 0., -60., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold)->SetShape(2.);
  GetParam(kSlope)->InitDouble("Slope", 2., 1, 100, 1, "-");
  GetParam(kSlope)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Softness", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(COMPRESSOR_ID, COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX, kAttackY, kAttack, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kReleaseX, kReleaseY, kRelease, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSlopeX, kSlopeY, kSlope, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  powerFilter.set_input_port(0, &inFilter, 0);
  gainExpanderFilter.set_input_port(0, &powerFilter, 0);
  attackReleaseFilter.set_input_port(0, &gainExpanderFilter, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &applyGainFilter, 0);
  
  Reset();
}

ATKExpander::~ATKExpander() {}

void ATKExpander::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKExpander::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  inFilter.set_input_sampling_rate(sampling_rate);
  inFilter.set_output_sampling_rate(sampling_rate);
  powerFilter.set_input_sampling_rate(sampling_rate);
  powerFilter.set_output_sampling_rate(sampling_rate);
  attackReleaseFilter.set_input_sampling_rate(sampling_rate);
  attackReleaseFilter.set_output_sampling_rate(sampling_rate);
  gainExpanderFilter.set_input_sampling_rate(sampling_rate);
  gainExpanderFilter.set_output_sampling_rate(sampling_rate);
  applyGainFilter.set_input_sampling_rate(sampling_rate);
  applyGainFilter.set_output_sampling_rate(sampling_rate);
  outFilter.set_input_sampling_rate(sampling_rate);
  outFilter.set_output_sampling_rate(sampling_rate);

  powerFilter.set_memory(std::exp(-1/1e-3 * GetSampleRate()));
  attackReleaseFilter.set_attack(std::exp(-1 / (GetParam(kAttack)->Value() * 1e-3 * GetSampleRate()))); // in ms
  attackReleaseFilter.set_release(std::exp(-1 / (GetParam(kRelease)->Value() * 1e-3 * GetSampleRate()))); // in ms
}

void ATKExpander::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      gainExpanderFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      break;
    case kSlope:
      gainExpanderFilter.set_slope(GetParam(kSlope)->Value());
      break;
    case kSoftness:
      gainExpanderFilter.set_softness(std::pow(10, GetParam(kSoftness)->Value()));
      break;
    case kAttack:
      attackReleaseFilter.set_attack(std::exp(-1 / (GetParam(kAttack)->Value() * 1e-3 * GetSampleRate()))); // in ms
      break;
    case kRelease:
      attackReleaseFilter.set_release(std::exp(-1 / (GetParam(kRelease)->Value() * 1e-3 * GetSampleRate()))); // in ms
      break;

    default:
      break;
  }
}
