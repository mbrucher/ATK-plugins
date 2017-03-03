#include "ATKSD1.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;
const int OVERSAMPLING = 8;

enum EParams
{
  kDrive = 0,
  kTone,
  kLevel,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kLevelX = 30,
  kLevelY = 20,
  kToneX = 150,
  kToneY = 70,
  kDriveX = 250,
  kDriveY = 20,
  kKnobFrames = 131
};

ATKSD1::ATKSD1(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDrive(0.), mTone(50), mLevel(100),
    inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDrive)->InitDouble("Drive", 0., 0., 100.0, 0.01, "%");
  GetParam(kDrive)->SetShape(2.);
  GetParam(kTone)->InitDouble("Tone", 0, -100., 100.0, 0.01, "%");
  GetParam(kTone)->SetShape(1.);
  GetParam(kLevel)->InitDouble("Level", 100., 0., 100.0, 0.01, "%");
  GetParam(kLevel)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(SD1_ID, SD1_FN);

  auto bigknob = pGraphics->LoadIBitmap(BIGKNOB_ID, BIGKNOB_FN, kKnobFrames);
  auto smallknob = pGraphics->LoadIBitmap(SMALLKNOB_ID, SMALLKNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDriveX, kDriveY, kDrive, &bigknob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kToneX, kToneY, kTone, &smallknob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kLevelX, kLevelY, kLevel, &bigknob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  //SetLatency(3);

  oversamplingFilter.set_input_port(0, &inFilter, 0);
  overdriveFilter.set_input_port(0, &oversamplingFilter, 0);
  lowpassFilter.set_input_port(0, &overdriveFilter, 0);
  decimationFilter.set_input_port(0, &lowpassFilter, 0);
  toneFilter.set_input_port(0, &decimationFilter, 0);
  highpassFilter.set_input_port(0, &toneFilter, 0);
  volumeFilter.set_input_port(0, &highpassFilter, 0);
  outFilter.set_input_port(0, &volumeFilter, 0);
  
  lowpassFilter.set_cut_frequency(20000);
  lowpassFilter.set_order(6);
  highpassFilter.select(2);
  highpassFilter.set_cut_frequency(20);
  highpassFilter.set_attenuation(1);

  Reset();
}

ATKSD1::~ATKSD1() {}

void ATKSD1::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKSD1::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  inFilter.set_input_sampling_rate(sampling_rate);
  inFilter.set_output_sampling_rate(sampling_rate);
  oversamplingFilter.set_input_sampling_rate(sampling_rate);
  oversamplingFilter.set_output_sampling_rate(sampling_rate * OVERSAMPLING);
  lowpassFilter.set_input_sampling_rate(sampling_rate * OVERSAMPLING);
  lowpassFilter.set_output_sampling_rate(sampling_rate * OVERSAMPLING);
  overdriveFilter.set_input_sampling_rate(sampling_rate * OVERSAMPLING);
  overdriveFilter.set_output_sampling_rate(sampling_rate * OVERSAMPLING);
  decimationFilter.set_input_sampling_rate(sampling_rate * OVERSAMPLING);
  decimationFilter.set_output_sampling_rate(sampling_rate);
  toneFilter.set_input_sampling_rate(sampling_rate);
  toneFilter.set_output_sampling_rate(sampling_rate);
  highpassFilter.set_input_sampling_rate(sampling_rate);
  highpassFilter.set_output_sampling_rate(sampling_rate);
  volumeFilter.set_input_sampling_rate(sampling_rate);
  volumeFilter.set_output_sampling_rate(sampling_rate);
  outFilter.set_input_sampling_rate(sampling_rate);
  outFilter.set_output_sampling_rate(sampling_rate);
  overdriveFilter.set_drive(GetParam(kDrive)->Value() / 100.);
}

void ATKSD1::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDrive:
      overdriveFilter.set_drive(GetParam(kDrive)->Value() / 100.);
      break;
    case kTone:
      toneFilter.set_tone((GetParam(kTone)->Value() + 100) / 200.);
      break;
    case kLevel:
      volumeFilter.set_volume(GetParam(kLevel)->Value() / 150.);
      break;

    default:
      break;
  }
}
