#include "ATKUniversalVariableDelay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kDelay = 0,
  kDepth,
  kMod,
  kBlend,
  kFeedforward,
  kFeedback,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kDelayX = 25,
  kDelayY = 32,
  kDepthX = 94,
  kDepthY = 32,
  kModX = 163,
  kModY = 32,
  kBlendX = 232,
  kBlendY = 32,
  kFeedforwardX = 301,
  kFeedforwardY = 32,
  kFeedbackX = 370,
  kFeedbackY = 32,
  kKnobFrames = 43
};

ATKUniversalVariableDelay::ATKUniversalVariableDelay(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), inFilter(nullptr, 1, 0, false), outFilter(nullptr, 1, 0, false), delayFilter(1000)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDelay)->InitDouble("Delay", 0.2, 0.2, 3.0, 0.1, "ms");
  GetParam(kDelay)->SetShape(2.);
  GetParam(kDepth)->InitDouble("Depth", 0.1, 0.1, 3.0, 0.1, "ms");
  GetParam(kDepth)->SetShape(2.);
  GetParam(kMod)->InitDouble("Modulation", 1, 0.1, 5.0, 0.1, "Hz");
  GetParam(kMod)->SetShape(1.);
  GetParam(kBlend)->InitDouble("Blend", 100, -100, 100, 0.01, "%");
  GetParam(kBlend)->SetShape(1.);
  GetParam(kFeedforward)->InitDouble("Feedforward", 50., -100, 100, 0.01, "%");
  GetParam(kFeedforward)->SetShape(1.);
  GetParam(kFeedback)->InitDouble("Feedback", 0., -90., 90., 0.01, "%");
  GetParam(kFeedback)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(UNIVERSALDELAY_ID, UNIVERSALDELAY_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDelayX, kDelayY, kDelay, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kDepthX, kDepthY, kDepth, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kModX, kModY, kMod, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kBlendX, kBlendY, kBlend, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardX, kFeedforwardY, kFeedforward, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackX, kFeedbackY, kFeedback, &knob1));

  AttachGraphics(pGraphics);

  MakePreset("Vibrato", 2, 1, 2, 0, 1, 0);
  MakePreset("Flanger", 2, 1, 2, 0.7, 0.7, 0.7);

  delayFilter.set_input_port(0, &inFilter, 0);
  delayFilter.set_input_port(1, &sinusGenerator, 0);
  outFilter.set_input_port(0, &delayFilter, 0);
  
  Reset();
}

ATKUniversalVariableDelay::~ATKUniversalVariableDelay() {}

void ATKUniversalVariableDelay::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKUniversalVariableDelay::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();

  if (sampling_rate != outFilter.get_input_sampling_rate())
  {
    inFilter.set_input_sampling_rate(sampling_rate);
    inFilter.set_output_sampling_rate(sampling_rate);
    sinusGenerator.set_output_sampling_rate(sampling_rate);
    delayFilter.set_input_sampling_rate(sampling_rate);
    delayFilter.set_output_sampling_rate(sampling_rate);
    outFilter.set_input_sampling_rate(sampling_rate);
    outFilter.set_output_sampling_rate(sampling_rate);
  }
  sinusGenerator.full_setup();
}

void ATKUniversalVariableDelay::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
  case kDelay:
    if (GetParam(kDepth)->Value() > GetParam(kDelay)->Value() - 0.1)
    {
      GetParam(kDelay)->Set(GetParam(kDepth)->Value() + 0.1);
    }
    sinusGenerator.set_offset(GetParam(kDelay)->Value() / 1000. * GetSampleRate());
    delayFilter.set_central_delay(GetParam(kDelay)->Value() / 1000. * GetSampleRate());
    break;
  case kDepth:
    if (GetParam(kDepth)->Value() > GetParam(kDelay)->Value() - 0.1)
    {
      GetParam(kDepth)->Set(GetParam(kDelay)->Value() - 0.1);
    }
    sinusGenerator.set_volume(GetParam(kDepth)->Value() / 1000. * GetSampleRate());
    break;
  case kMod:
    sinusGenerator.set_frequency(GetParam(kMod)->Value());
    break;
  case kBlend:
    delayFilter.set_blend((GetParam(kBlend)->Value()) / 100.);
    break;
  case kFeedforward:
    delayFilter.set_feedforward((GetParam(kFeedforward)->Value()) / 100.);
    break;
  case kFeedback:
    delayFilter.set_feedback((GetParam(kFeedback)->Value()) / 100.);
    break;

  default:
    break;
  }
}
