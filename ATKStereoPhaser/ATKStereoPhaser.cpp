#include "ATKStereoPhaser.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kModulation = 0,
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

ATKStereoPhaser::ATKStereoPhaser(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), inFilter(nullptr, 1, 0, false), applyGainFilter(2), out1Filter(nullptr, 1, 0, false), out2Filter(nullptr, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kModulation)->InitDouble("Modulation", 1, 0.1, 100.0, 0.1, "Hz");
  GetParam(kModulation)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(STEREOPHASER_ID, STEREOPHASER_FN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDelayX, kDelayY, kModulation, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);

  allpass1Filter.set_input_port(0, &inFilter, 0);
  allpass2Filter.set_input_port(0, &inFilter, 0);
  applyGainFilter.set_input_port(0, &allpass1Filter, 0);
  applyGainFilter.set_input_port(1, &sinusFilter, 1);
  applyGainFilter.set_input_port(2, &allpass2Filter, 0);
  applyGainFilter.set_input_port(3, &sinusFilter, 0);
  volumeFilter.set_input_port(0, &applyGainFilter, 1);
  sum1Filter.set_input_port(0, &applyGainFilter, 0);
  sum1Filter.set_input_port(1, &volumeFilter, 0);
  sum2Filter.set_input_port(0, &applyGainFilter, 0);
  sum2Filter.set_input_port(1, &applyGainFilter, 1);
  out1Filter.set_input_port(0, &sum1Filter, 0);
  out2Filter.set_input_port(0, &sum2Filter, 0);
  sinkFilter.add_filter(&out1Filter);
  sinkFilter.add_filter(&out2Filter);

  volumeFilter.set_volume(-1);
  std::vector<double> coeffs;
  coeffs.push_back(1);
  coeffs.push_back(0);
  coeffs.push_back(1.3313);
  coeffs.push_back(0);
  coeffs.push_back(0.3855);
  coeffs.push_back(0);
  allpass1Filter.set_coefficients_in(coeffs);
  coeffs.clear();
  coeffs.push_back(-0.3855);
  coeffs.push_back(0);
  coeffs.push_back(-1.3313);
  coeffs.push_back(0);
  allpass1Filter.set_coefficients_out(coeffs);
  coeffs.clear();

  coeffs.push_back(1);
  coeffs.push_back(0);
  coeffs.push_back(0.8335);
  coeffs.push_back(0);
  coeffs.push_back(0.0947);
  coeffs.push_back(0);
  allpass2Filter.set_coefficients_in(coeffs);
  coeffs.clear();
  coeffs.push_back(0);
  coeffs.push_back(-0.0947);
  coeffs.push_back(0);
  coeffs.push_back(-0.8335);
  coeffs.push_back(0);
  allpass2Filter.set_coefficients_out(coeffs);

  Reset();
}

ATKStereoPhaser::~ATKStereoPhaser() {}

void ATKStereoPhaser::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  inFilter.set_pointer(inputs[0], nFrames);
  out1Filter.set_pointer(outputs[0], nFrames);
  out2Filter.set_pointer(outputs[1], nFrames);
  sinkFilter.process(nFrames);
}

void ATKStereoPhaser::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  if (sampling_rate != sinkFilter.get_input_sampling_rate())
  {
    inFilter.set_input_sampling_rate(sampling_rate);
    inFilter.set_output_sampling_rate(sampling_rate);
    allpass1Filter.set_input_sampling_rate(sampling_rate);
    allpass1Filter.set_output_sampling_rate(sampling_rate);
    allpass2Filter.set_input_sampling_rate(sampling_rate);
    allpass2Filter.set_output_sampling_rate(sampling_rate);
    sinusFilter.set_output_sampling_rate(sampling_rate);
    applyGainFilter.set_input_sampling_rate(sampling_rate);
    applyGainFilter.set_output_sampling_rate(sampling_rate);
    volumeFilter.set_input_sampling_rate(sampling_rate);
    volumeFilter.set_output_sampling_rate(sampling_rate);
    sum1Filter.set_input_sampling_rate(sampling_rate);
    sum1Filter.set_output_sampling_rate(sampling_rate);
    sum2Filter.set_input_sampling_rate(sampling_rate);
    sum2Filter.set_output_sampling_rate(sampling_rate);
    out1Filter.set_input_sampling_rate(sampling_rate);
    out1Filter.set_output_sampling_rate(sampling_rate);
    out2Filter.set_input_sampling_rate(sampling_rate);
    out2Filter.set_output_sampling_rate(sampling_rate);
    sinkFilter.set_input_sampling_rate(sampling_rate);
    sinkFilter.set_output_sampling_rate(sampling_rate);
  }
  sinusFilter.full_setup();
}

void ATKStereoPhaser::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
  case kModulation:
    sinusFilter.set_frequency(GetParam(kModulation)->Value());
    break;

  default:
    break;
  }
}
