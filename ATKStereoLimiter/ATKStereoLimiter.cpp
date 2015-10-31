#include <cmath>

#include "ATKStereoLimiter.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kRelease = 0,
  kThreshold,
  kSoftness,
  kDryWet,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kReleaseX = 27,
  kReleaseY = 40,
  kThresholdX = 130,
  kThresholdY = 40,
  kSoftnessX = 233,
  kSoftnessY = 40,

  kDryWetX = 336,
  kDryWetY = 40,

  kKnobFrames = 20,
  kKnobFrames1 = 19
};

ATKStereoLimiter::ATKStereoLimiter(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
  inLFilter(NULL, 1, 0, false), inRFilter(NULL, 1, 0, false), applyGainFilter(2), drywetFilter(2), outLFilter(NULL, 1, 0, false), outRFilter(NULL, 1, 0, false)
{
  TRACE;

  GetParam(kRelease)->InitDouble("Release", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kThreshold)->InitDouble("Threshold", 0., -20., 0.0, 0.1, "dB"); // threshold is actually power
  GetParam(kThreshold)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Softness", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);
  GetParam(kDryWet)->InitDouble("Dry/Wet", 1, 0, 1, 0.01, "-");
  GetParam(kDryWet)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(STEREO_COMPRESSOR_ID, STEREO_COMPRESSOR_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames1);
  IText text = IText(10, 0, 0, IText::kStyleBold);
  
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseX, kReleaseY, kReleaseX + 78, kReleaseY + 78 + 21), kRelease, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThresholdX, kThresholdY, kThresholdX + 78, kThresholdY + 78 + 21), kThreshold, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));

  pGraphics->AttachControl(new IKnobMultiControl(this, kDryWetX, kDryWetY, kDryWet, &knob1));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  endpoint.add_filter(&outLFilter);
  endpoint.add_filter(&outRFilter);

  powerFilter1.set_input_port(0, &inLFilter, 0);
  powerFilter2.set_input_port(0, &inRFilter, 0);
  maxFilter.set_input_port(0, &powerFilter1, 0);
  maxFilter.set_input_port(1, &powerFilter2, 0);
  attackReleaseFilter.set_input_port(0, &maxFilter, 0);
  gainLimiterFilter.set_input_port(0, &attackReleaseFilter, 0);

  applyGainFilter.set_input_port(0, &gainLimiterFilter, 0);
  applyGainFilter.set_input_port(1, &inLFilter, 0);
  applyGainFilter.set_input_port(2, &gainLimiterFilter, 0);
  applyGainFilter.set_input_port(3, &inRFilter, 0);
  
  drywetFilter.set_input_port(0, &applyGainFilter, 0);
  drywetFilter.set_input_port(1, &inLFilter, 0);
  drywetFilter.set_input_port(2, &applyGainFilter, 1);
  drywetFilter.set_input_port(3, &inRFilter, 0);
  outLFilter.set_input_port(0, &drywetFilter, 0);
  outRFilter.set_input_port(0, &drywetFilter, 1);
  
  powerFilter1.set_memory(0);
  powerFilter2.set_memory(0);
  attackReleaseFilter.set_attack(0);

  Reset();
}

ATKStereoLimiter::~ATKStereoLimiter() {}

void ATKStereoLimiter::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inLFilter.set_pointer(inputs[0], nFrames);
  outLFilter.set_pointer(outputs[0], nFrames);
  inRFilter.set_pointer(inputs[1], nFrames);
  outRFilter.set_pointer(outputs[1], nFrames);
  endpoint.process(nFrames);
}

void ATKStereoLimiter::Reset()
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

    powerFilter1.set_input_sampling_rate(sampling_rate);
    powerFilter1.set_output_sampling_rate(sampling_rate);
    powerFilter2.set_input_sampling_rate(sampling_rate);
    powerFilter2.set_output_sampling_rate(sampling_rate);

    maxFilter.set_input_sampling_rate(sampling_rate);
    maxFilter.set_output_sampling_rate(sampling_rate);
    attackReleaseFilter.set_input_sampling_rate(sampling_rate);
    attackReleaseFilter.set_output_sampling_rate(sampling_rate);
    gainLimiterFilter.set_input_sampling_rate(sampling_rate);
    gainLimiterFilter.set_output_sampling_rate(sampling_rate);

    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    drywetFilter.set_input_sampling_rate(sampling_rate);
    drywetFilter.set_output_sampling_rate(sampling_rate);
    endpoint.set_input_sampling_rate(sampling_rate);
    endpoint.set_output_sampling_rate(sampling_rate);

    attackReleaseFilter.set_release(std::exp(-1e3 / (GetParam(kRelease)->Value() * sampling_rate))); // in ms
  }
}

void ATKStereoLimiter::OnParamChange(int paramIdx)
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
    case kRelease:
      attackReleaseFilter.set_attack(std::exp(-1 / (GetParam(kRelease)->Value() * 1e-3 * GetSampleRate()))); // in ms
      break;
    case kDryWet:
      drywetFilter.set_dry(GetParam(kDryWet)->Value());
      break;

    default:
      break;
  }
}
