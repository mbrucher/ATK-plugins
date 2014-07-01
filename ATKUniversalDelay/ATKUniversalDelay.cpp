#include "ATKUniversalDelay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kDelay = 0,
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
  kDelayY = 37,
  kBlendX = 94,
  kBlendY = 37,
  kFeedforwardX = 163,
  kFeedforwardY = 37,
  kFeedbackX = 232,
  kFeedbackY = 37,
  kKnobFrames = 43
};

ATKUniversalDelay::ATKUniversalDelay(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false), delayFilter(10240)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDelay)->InitDouble("Delay", 1, 0.1, 100.0, 0.1, "ms");
  GetParam(kDelay)->SetShape(2.);
  GetParam(kBlend)->InitDouble("Blend", 100, 0., 100.0, 0.01, "%");
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
  pGraphics->AttachControl(new IKnobMultiControl(this, kBlendX, kBlendY, kBlend, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardX, kFeedforwardY, kFeedforward, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackX, kFeedbackY, kFeedback, &knob1));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  delayFilter.set_input_port(0, &inFilter, 0);
  outFilter.set_input_port(0, &delayFilter, 0);
  
  Reset();
}

ATKUniversalDelay::~ATKUniversalDelay() {}

void ATKUniversalDelay::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKUniversalDelay::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  inFilter.set_input_sampling_rate(sampling_rate);
//  inFilter.set_output_sampling_rate(sampling_rate);
  delayFilter.set_input_sampling_rate(sampling_rate);
//  delayFilter.set_output_sampling_rate(sampling_rate);
  outFilter.set_input_sampling_rate(sampling_rate);
  outFilter.set_output_sampling_rate(sampling_rate);
}

void ATKUniversalDelay::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDelay:
      delayFilter.set_delay(GetParam(kDelay)->Value() / 1000. * GetSampleRate());
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
