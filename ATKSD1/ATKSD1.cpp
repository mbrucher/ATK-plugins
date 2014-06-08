#include "ATKSD1.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

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

  kDriveX = 55,
  kDriveY = 38,
  kToneX = 125,
  kToneY = 95,
  kLevelX = 193,
  kLevelY = 38,
  kKnobFrames = 60
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

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDriveX, kDriveY, kDrive, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kToneX, kToneY, kTone, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, kLevelX, kLevelY, kLevel, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  //SetLatency(3);
}

ATKSD1::~ATKSD1() {}

void ATKSD1::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* out1 = outputs[0];

  for (int s = 0; s < nFrames; ++s, ++in1, ++out1)
  {
    *out1 = *in1 * mLevel;
  }
}

void ATKSD1::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  //reset setup GetSampleRate()
}

void ATKSD1::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kDrive:
      //mGain = GetParam(kGain)->Value() / 100.;
      break;
    case kTone:
      //mGain = GetParam(kGain)->Value() / 100.;
      break;
    case kLevel:
      //mGain = GetParam(kGain)->Value() / 100.;
      break;

    default:
      break;
  }
}
