#include "ATKStereoUniversalDelay.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "controls.h"
#include "resource.h"

const int kNumPrograms = 4;

enum EParams
{
  kDelayL = 0,
  kDelayR,
  kBlendL,
  kBlendR,
  kFeedforwardLL,
  kFeedforwardLR,
  kFeedforwardRL,
  kFeedforwardRR,
  kFeedbackLL,
  kFeedbackLR,
  kFeedbackRL,
  kFeedbackRR,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kDelayLX = 27,
  kDelayLY = 40,
  kDelayRX = 27,
  kDelayRY = 160,
  kBlendLX = 130,
  kBlendLY = 40,
  kBlendRX = 130,
  kBlendRY = 160,
  kFeedforwardLLX = 233,
  kFeedforwardLLY = 40,
  kFeedforwardLRX = 336,
  kFeedforwardLRY = 40,
  kFeedforwardRLX = 233,
  kFeedforwardRLY = 160,
  kFeedforwardRRX = 336,
  kFeedforwardRRY = 160,
  kFeedbackLLX = 439,
  kFeedbackLLY = 40,
  kFeedbackLRX = 542,
  kFeedbackLRY = 40,
  kFeedbackRLX = 439,
  kFeedbackRLY = 160,
  kFeedbackRRX = 542,
  kFeedbackRRY = 160,

  kKnobFrames = 20,
  kKnobFrames1 = 19
};

ATKStereoUniversalDelay::ATKStereoUniversalDelay(IPlugInstanceInfo instanceInfo)
  :IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), inFilterL(nullptr, 1, 0, false), inFilterR(nullptr, 1, 0, false),
    outFilterL(nullptr, 1, 0, false), outFilterR(nullptr, 1, 0, false), delayFilter(96000)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDelayL)->InitDouble("Delay L", 1, 0.1, 500.0, 0.1, "ms");
  GetParam(kDelayL)->SetShape(2.);
  GetParam(kDelayR)->InitDouble("Delay R", 1, 0.1, 500.0, 0.1, "ms");
  GetParam(kDelayR)->SetShape(2.);
  GetParam(kBlendL)->InitDouble("Blend L", 100, 0., 100.0, 0.01, "%");
  GetParam(kBlendL)->SetShape(1.);
  GetParam(kBlendR)->InitDouble("Blend R", 100, 0., 100.0, 0.01, "%");
  GetParam(kBlendR)->SetShape(1.);
  GetParam(kFeedforwardLL)->InitDouble("Feedforward L to L", 50., -100, 100, 0.01, "%");
  GetParam(kFeedforwardLL)->SetShape(1.);
  GetParam(kFeedforwardLR)->InitDouble("Feedforward L to R", 50., -100, 100, 0.01, "%");
  GetParam(kFeedforwardLR)->SetShape(1.);
  GetParam(kFeedforwardRL)->InitDouble("Feedforward R to L", 50., -100, 100, 0.01, "%");
  GetParam(kFeedforwardRL)->SetShape(1.);
  GetParam(kFeedforwardRR)->InitDouble("Feedforward R to R", 50., -100, 100, 0.01, "%");
  GetParam(kFeedforwardRR)->SetShape(1.);
  GetParam(kFeedbackLL)->InitDouble("Feedback L to L", 0., -90., 90., 0.01, "%");
  GetParam(kFeedbackLL)->SetShape(1.);
  GetParam(kFeedbackLR)->InitDouble("Feedback L to R", 0., -90., 90., 0.01, "%");
  GetParam(kFeedbackLR)->SetShape(1.);
  GetParam(kFeedbackRL)->InitDouble("Feedback R to L", 0., -90., 90., 0.01, "%");
  GetParam(kFeedbackRL)->SetShape(1.);
  GetParam(kFeedbackRR)->InitDouble("Feedback R to R", 0., -90., 90., 0.01, "%");
  GetParam(kFeedbackRR)->SetShape(1.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(UNIVERSALDELAY_ID, UNIVERSALDELAY_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  IBitmap knob1 = pGraphics->LoadIBitmap(KNOB1_ID, KNOB1_FN, kKnobFrames1);
  IColor color = IColor(255, 255, 255, 255);
  IText text = IText(10, &color, nullptr, IText::kStyleBold);

  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kDelayLX, kDelayLY, kDelayLX + 78, kDelayLY + 78 + 21), kDelayL, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControlText(this, IRECT(kDelayRX, kDelayRY, kDelayRX + 78, kDelayRY + 78 + 21), kDelayR, &knob, &text, "ms"));
  pGraphics->AttachControl(new IKnobMultiControl(this, kBlendLX, kBlendLY, kBlendL, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kBlendRX, kBlendRY, kBlendR, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardLLX, kFeedforwardLLY, kFeedforwardLL, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardLRX, kFeedforwardLRY, kFeedforwardLR, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardRLX, kFeedforwardRLY, kFeedforwardRL, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedforwardRRX, kFeedforwardRRY, kFeedforwardRR, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackLLX, kFeedbackLLY, kFeedbackLL, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackLRX, kFeedbackLRY, kFeedbackLR, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackRLX, kFeedbackRLY, kFeedbackRL, &knob1));
  pGraphics->AttachControl(new IKnobMultiControl(this, kFeedbackRRX, kFeedbackRRY, kFeedbackRR, &knob1));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakePreset("FIR comb filter", 1., 1., 100., 100., 50., 50., 50., 50., 0., 0., 0., 0.);
  MakePreset("IIR comb filter", 1., 1., 100., 100., 50., 50., 50., 50., 10., 10., 10., 10.);
  MakePreset("All pass", 1., 1., 100., 100., 100., 100., 100., 100., -10., -10., -10., -10.);
  MakePreset("Delay", 1., 1., 100., 100., 0., 0., 0., 0., 0., 0., 0., 0.);

  delayFilter.set_input_port(0, &inFilterL, 0);
  delayFilter.set_input_port(1, &inFilterR, 0);
  outFilterL.set_input_port(0, &delayFilter, 0);
  outFilterR.set_input_port(0, &delayFilter, 1);
  endPoint.add_filter(&outFilterL);
  endPoint.add_filter(&outFilterR);
  
  Reset();
}

ATKStereoUniversalDelay::~ATKStereoUniversalDelay() {}

void ATKStereoUniversalDelay::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  inFilterL.set_pointer(inputs[0], nFrames);
  inFilterR.set_pointer(inputs[1], nFrames);
  outFilterL.set_pointer(outputs[0], nFrames);
  outFilterR.set_pointer(outputs[1], nFrames);
  endPoint.process(nFrames);
}

void ATKStereoUniversalDelay::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  if(sampling_rate != inFilterL.get_output_sampling_rate())
  {
    inFilterL.set_input_sampling_rate(sampling_rate);
    inFilterL.set_output_sampling_rate(sampling_rate);
    inFilterR.set_input_sampling_rate(sampling_rate);
    inFilterR.set_output_sampling_rate(sampling_rate);
    delayFilter.set_input_sampling_rate(sampling_rate);
    delayFilter.set_output_sampling_rate(sampling_rate);
    outFilterL.set_input_sampling_rate(sampling_rate);
    outFilterL.set_output_sampling_rate(sampling_rate);
    outFilterR.set_input_sampling_rate(sampling_rate);
    outFilterR.set_output_sampling_rate(sampling_rate);
    endPoint.set_input_sampling_rate(sampling_rate);
    endPoint.set_output_sampling_rate(sampling_rate);
  }
  delayFilter.full_setup();
}

void ATKStereoUniversalDelay::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDelayL:
      delayFilter.set_delay_ch1(GetParam(kDelayL)->Value() / 1000. * GetSampleRate());
      break;
    case kDelayR:
      delayFilter.set_delay_ch1(GetParam(kDelayR)->Value() / 1000. * GetSampleRate());
      break;
    case kBlendL:
      delayFilter.set_blend_ch1((GetParam(kBlendL)->Value()) / 100.);
      break;
    case kBlendR:
      delayFilter.set_blend_ch1((GetParam(kBlendR)->Value()) / 100.);
      break;
    case kFeedforwardLL:
      delayFilter.set_feedforward_ch1_ch1((GetParam(kFeedforwardLL)->Value()) / 100.);
      break;
    case kFeedforwardLR:
      delayFilter.set_feedforward_ch1_ch1((GetParam(kFeedforwardLR)->Value()) / 100.);
      break;
    case kFeedforwardRL:
      delayFilter.set_feedforward_ch1_ch1((GetParam(kFeedforwardRL)->Value()) / 100.);
      break;
    case kFeedforwardRR:
      delayFilter.set_feedforward_ch1_ch1((GetParam(kFeedforwardRR)->Value()) / 100.);
      break;
    case kFeedbackLL:
      delayFilter.set_feedback_ch1_ch1((GetParam(kFeedbackLL)->Value()) / 100.);
      break;
    case kFeedbackLR:
      delayFilter.set_feedback_ch1_ch1((GetParam(kFeedbackLR)->Value()) / 100.);
      break;
    case kFeedbackRL:
      delayFilter.set_feedback_ch1_ch1((GetParam(kFeedbackRL)->Value()) / 100.);
      break;
    case kFeedbackRR:
      delayFilter.set_feedback_ch1_ch1((GetParam(kFeedbackRR)->Value()) / 100.);
      break;

    default:
      break;
  }
}
