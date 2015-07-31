#include <cmath>

#include "ATKLimiter.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "controls.h"

const int kNumPrograms = 2;

enum EParams
{
  kAttack = 0,
  kRelease,
  kThreshold,
  kSoftness,
  kMakeup,
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
  kSoftnessX = 232,
  kSoftnessY = 26,
  kMakeupX = 301,
  kMakeupY = 26,
  kKnobFrames = 43
};

ATKLimiter::ATKLimiter(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
    inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kAttack)->InitDouble("Attack", 10., 0., 100.0, 0.1, "ms");
  GetParam(kAttack)->SetShape(2.);
  GetParam(kRelease)->InitDouble("Release", 10, 0., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kThreshold)->InitDouble("Threshold", 0., -40., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Softness", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);
  GetParam(kMakeup)->InitDouble("Makeup Gain", 0, 0, 40, 0.1, "-"); // Makeup is expressed in amplitude
  GetParam(kMakeup)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(COMPRESSOR_ID, COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IText text = IText(10, 0, 0, IText::kStyleBold);
  
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttackX, kAttackY, kAttackX + 43, kAttackY + 43 + 21), kAttack, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseX, kReleaseY, kReleaseX + 43, kReleaseY + 43 + 21), kRelease, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThresholdX, kThresholdY, kThresholdX + 43, kThresholdY + 43 + 21), kThreshold, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kMakeupX, kMakeupY, kMakeupX + 43, kMakeupY + 43 + 21), kMakeup, &knob, &text, "dB"));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakePreset("Custom", 10., 10., 0., -2., 0.);
  MakePreset("Brick wall limiter", 0., 10., -0.1, -2., 0.);

  powerFilter.set_input_port(0, &inFilter, 0);
  gainLimiterFilter.set_input_port(0, &powerFilter, 0);
  attackReleaseFilter.set_input_port(0, &gainLimiterFilter, 0);
  applyGainFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 0);
  outFilter.set_input_port(0, &volumeFilter, 0);

  powerFilter.set_memory(0);

  Reset();
}

ATKLimiter::~ATKLimiter() {}

void ATKLimiter::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKLimiter::Reset()
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
  gainLimiterFilter.set_input_sampling_rate(sampling_rate);
  gainLimiterFilter.set_output_sampling_rate(sampling_rate);
  applyGainFilter.set_input_sampling_rate(sampling_rate);
  applyGainFilter.set_output_sampling_rate(sampling_rate);
  volumeFilter.set_input_sampling_rate(sampling_rate);
  volumeFilter.set_output_sampling_rate(sampling_rate);
  outFilter.set_input_sampling_rate(sampling_rate);
  outFilter.set_output_sampling_rate(sampling_rate);

  attackReleaseFilter.set_release(std::exp(-1e3 / (GetParam(kAttack)->Value() * sampling_rate))); // in ms
  attackReleaseFilter.set_attack(std::exp(-1e3 / (GetParam(kRelease)->Value() * sampling_rate))); // in ms
}

void ATKLimiter::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      gainLimiterFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      break;
    case kSoftness:
      gainLimiterFilter.set_softness(std::pow(10, GetParam(kSoftness)->Value()));
      break;
    case kAttack:
      if (GetParam(kAttack)->Value() == 0)
      {
        attackReleaseFilter.set_release(0); // in ms
      }
      else
      {
        attackReleaseFilter.set_release(std::exp(-1e3 / (GetParam(kAttack)->Value() * GetSampleRate()))); // in ms
      }
      break;
    case kRelease:
      if (GetParam(kRelease)->Value() == 0)
      {
        attackReleaseFilter.set_attack(0); // in ms
      }
      else
      {
        attackReleaseFilter.set_attack(std::exp(-1e3 / (GetParam(kRelease)->Value() * GetSampleRate()))); // in ms
      }
      break;
    case kMakeup:
      volumeFilter.set_volume_db(GetParam(kMakeup)->Value());
      break;

    default:
      break;
  }
}
