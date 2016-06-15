#include <cmath>

#include "ATKTransientSplitter.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 2;

enum EParams
{
  kPower = 0,
  kAttack,
  kAttackRatio,
  kRelease,
  kReleaseRatio,
  kThreshold,
  kSlope,
  kSoftness,
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
  kAttackRatioX = 233,
  kAttackRatioY = 40,
  kReleaseX = 336,
  kReleaseY = 40,
  kReleaseRatioX = 439,
  kReleaseRatioY = 40,
  kThresholdX = 542,
  kThresholdY = 40,
  kSlopeX = 645,
  kSlopeY = 40,
  kSoftnessX = 748,
  kSoftnessY = 40,
  
  kKnobFrames = 20,
  kKnobFrames1 = 19
};

ATKTransientSplitter::ATKTransientSplitter(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
inFilter(nullptr, 1, 0, false), gainExpanderFilter(1, 256*1024), gainSwellFilter(1, 256*1024), applyGainFilter(2), outTransientFilter(nullptr, 1, 0, false), outSustainFilter(nullptr, 1, 0, false)
{
  TRACE;
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kPower)->InitDouble("Power", 10., 0., 100.0, 0.1, "ms");
  GetParam(kPower)->SetShape(2.);
  GetParam(kAttack)->InitDouble("Attack", 10., 1., 100.0, 0.1, "ms");
  GetParam(kAttack)->SetShape(2.);
  GetParam(kAttackRatio)->InitDouble("Attack Ratio", 10., 0.1, 100.0, 0.1, "%");
  GetParam(kRelease)->InitDouble("Release", 10, 1., 100.0, 0.1, "ms");
  GetParam(kRelease)->SetShape(2.);
  GetParam(kReleaseRatio)->InitDouble("Release Ratio", 100, 0.1, 100.0, 0.1, "%");
  GetParam(kThreshold)->InitDouble("Threshold", -30., -60., -20.0, 0.1, "dB"); // threshold is actually power
  GetParam(kSlope)->InitDouble("Slope", 2., 1, 10, .1, "-");
  GetParam(kSlope)->SetShape(2.);
  GetParam(kSoftness)->InitDouble("Softness", -2, -4, 0, 0.1, "-");
  GetParam(kSoftness)->SetShape(2.);
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(COLORED_COMPRESSOR_ID, COLORED_COMPRESSOR_FN);
  
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IColor color = IColor(255, 255, 255, 255);
  IText text = IText(10, &color, nullptr, IText::kStyleBold);
  
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kPowerX, kPowerY, kPowerX + 78, kPowerY + 78 + 21), kPower, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttackX, kAttackY, kAttackX + 78, kAttackY + 78 + 21), kAttack, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kAttackRatioX, kAttackRatioY, kAttackRatioX + 78, kAttackRatioY + 78 + 21), kAttackRatio, &knob, &text, "%"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseX, kReleaseY, kReleaseX + 78, kReleaseY + 78 + 21), kRelease, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kReleaseRatioX, kReleaseRatioY, kReleaseRatioX + 78, kReleaseRatioY + 78 + 21), kReleaseRatio, &knob, &text, "%"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kThresholdX, kThresholdY, kThresholdX + 78, kThresholdY + 78 + 21), kThreshold, &knob, &text, "dB"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kSlopeX, kSlopeY, kSlopeX + 78, kSlopeY + 78 + 21), kSlope, &knob, &text, ""));
  pGraphics->AttachControl(new IKnobMultiControl(this, kSoftnessX, kSoftnessY, kSoftness, &knob));
  
  AttachGraphics(pGraphics);
  
  //MakePreset("preset 1", ... );
  MakePreset("Serial Shaper", 10., 10., 10., 10., 100., -30., 2., -2.);
  MakePreset("Parallel Shaper", 10., 10., 10., 10., 100., -30., 2., -2.);
  
  powerFilter.set_input_port(0, &inFilter, 0);
  slowAttackReleaseFilter.set_input_port(0, &powerFilter, 0);
  fastAttackReleaseFilter.set_input_port(0, &powerFilter, 0);
  invertFilter.set_input_port(0, &slowAttackReleaseFilter, 0);
  sumFilter.set_input_port(0, &invertFilter, 0);
  sumFilter.set_input_port(1, &fastAttackReleaseFilter, 0);
  gainExpanderFilter.set_input_port(0, &sumFilter, 0);
  gainSwellFilter.set_input_port(0, &sumFilter, 0);
  applyGainFilter.set_input_port(0, &gainExpanderFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  applyGainFilter.set_input_port(2, &gainSwellFilter, 0);
  applyGainFilter.set_input_port(3, &inFilter, 0);
  outTransientFilter.set_input_port(0, &applyGainFilter, 0);
  outSustainFilter.set_input_port(0, &applyGainFilter, 1);
  
  invertFilter.set_volume(-1);
  sinkFilter.add_filter(&outTransientFilter);
  sinkFilter.add_filter(&outSustainFilter);
  
  Reset();
}

ATKTransientSplitter::~ATKTransientSplitter() {}

void ATKTransientSplitter::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  inFilter.set_pointer(inputs[0], nFrames);
  outTransientFilter.set_pointer(outputs[0], nFrames);
  outSustainFilter.set_pointer(outputs[1], nFrames);
  sinkFilter.process(nFrames);
}

void ATKTransientSplitter::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  if(sampling_rate != outTransientFilter.get_output_sampling_rate())
  {
    inFilter.set_input_sampling_rate(sampling_rate);
    inFilter.set_output_sampling_rate(sampling_rate);
    powerFilter.set_input_sampling_rate(sampling_rate);
    powerFilter.set_output_sampling_rate(sampling_rate);
    slowAttackReleaseFilter.set_input_sampling_rate(sampling_rate);
    slowAttackReleaseFilter.set_output_sampling_rate(sampling_rate);
    fastAttackReleaseFilter.set_input_sampling_rate(sampling_rate);
    fastAttackReleaseFilter.set_output_sampling_rate(sampling_rate);
    invertFilter.set_input_sampling_rate(sampling_rate);
    invertFilter.set_output_sampling_rate(sampling_rate);
    sumFilter.set_input_sampling_rate(sampling_rate);
    sumFilter.set_output_sampling_rate(sampling_rate);
    gainExpanderFilter.set_input_sampling_rate(sampling_rate);
    gainExpanderFilter.set_output_sampling_rate(sampling_rate);
    gainSwellFilter.set_input_sampling_rate(sampling_rate);
    gainSwellFilter.set_output_sampling_rate(sampling_rate);
    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    outTransientFilter.set_input_sampling_rate(sampling_rate);
    outTransientFilter.set_output_sampling_rate(sampling_rate);
    outSustainFilter.set_input_sampling_rate(sampling_rate);
    outSustainFilter.set_output_sampling_rate(sampling_rate);
    sinkFilter.set_input_sampling_rate(sampling_rate);
    sinkFilter.set_output_sampling_rate(sampling_rate);
    
    auto power = GetParam(kPower)->Value();
    if (power == 0)
    {
      powerFilter.set_memory(0);
    }
    else
    {
      powerFilter.set_memory(std::exp(-1e3 / (power * sampling_rate)));
    }
    slowAttackReleaseFilter.set_release(std::exp(-1e3/(GetParam(kRelease)->Value() * sampling_rate * GetParam(kReleaseRatio)->Value() / 100))); // in ms
    slowAttackReleaseFilter.set_attack(std::exp(-1e3/(GetParam(kAttack)->Value() * sampling_rate))); // in ms
    fastAttackReleaseFilter.set_release(std::exp(-1e3/(GetParam(kRelease)->Value() * sampling_rate))); // in ms
    fastAttackReleaseFilter.set_attack(std::exp(-1e3/(GetParam(kAttack)->Value() * sampling_rate * GetParam(kAttackRatio)->Value() / 100))); // in ms
  }
  
  powerFilter.full_setup();
  slowAttackReleaseFilter.full_setup();
  fastAttackReleaseFilter.full_setup();
}

void ATKTransientSplitter::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case kPower:
    {
      auto power = GetParam(kPower)->Value();
      if (power == 0)
      {
        powerFilter.set_memory(0);
      }
      else
      {
        powerFilter.set_memory(std::exp(-1e3 / (power * GetSampleRate())));
      }
      break;
    }
    case kThreshold:
      gainExpanderFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      gainSwellFilter.set_threshold(std::pow(10, GetParam(kThreshold)->Value() / 10));
      break;
    case kSlope:
      gainExpanderFilter.set_ratio(GetParam(kSlope)->Value());
      gainSwellFilter.set_ratio(1/GetParam(kSlope)->Value());
      break;
    case kSoftness:
      gainExpanderFilter.set_softness(std::pow(10, GetParam(kSoftness)->Value()));
      gainSwellFilter.set_softness(std::pow(10, GetParam(kSoftness)->Value()));
      break;
    case kAttack:
      slowAttackReleaseFilter.set_attack(std::exp(-1e3/(GetParam(kAttack)->Value() * GetSampleRate()))); // in ms
      fastAttackReleaseFilter.set_attack(std::exp(-1e3/(GetParam(kAttack)->Value() * GetSampleRate() * GetParam(kAttackRatio)->Value() / 100))); // in ms
      break;
    case kAttackRatio:
      fastAttackReleaseFilter.set_attack(std::exp(-1e3/(GetParam(kAttack)->Value() * GetSampleRate() * GetParam(kAttackRatio)->Value() / 100))); // in ms
      break;
    case kRelease:
      fastAttackReleaseFilter.set_release(std::exp(-1e3/(GetParam(kRelease)->Value() * GetSampleRate()))); // in ms
      slowAttackReleaseFilter.set_release(std::exp(-1e3/(GetParam(kRelease)->Value() * GetSampleRate() * GetParam(kReleaseRatio)->Value() / 100))); // in ms
      break;
    case kReleaseRatio:
      slowAttackReleaseFilter.set_release(std::exp(-1e3/(GetParam(kRelease)->Value() * GetSampleRate() * GetParam(kReleaseRatio)->Value() / 100))); // in ms
      break;
      
    default:
      break;
  }
}
