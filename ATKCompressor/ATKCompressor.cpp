#include "ATKCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kThreshold = 0,
  kSlope,
  kSoftness,
  kAttack,
  kRelease,
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

ATKCompressor::ATKCompressor(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDrive(0.), mTone(50), mLevel(100),
    inFilter(NULL, 1, 0, false), outFilter(NULL, 1, 0, false)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
/*  GetParam(kDrive)->InitDouble("Drive", 0., 0., 100.0, 0.01, "%");
  GetParam(kDrive)->SetShape(2.);
  GetParam(kTone)->InitDouble("Tone", 0, -100., 100.0, 0.01, "%");
  GetParam(kTone)->SetShape(1.);
  GetParam(kLevel)->InitDouble("Level", 100., 0., 100.0, 0.01, "%");
  GetParam(kLevel)->SetShape(2.);
*/
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachBackground(SD1_ID, SD1_FN);

  IBitmap bigknob = pGraphics->LoadIBitmap(BIGKNOB_ID, BIGKNOB_FN, kKnobFrames);
  IBitmap smallknob = pGraphics->LoadIBitmap(SMALLKNOB_ID, SMALLKNOB_FN, kKnobFrames);

//  pGraphics->AttachControl(new IKnobMultiControl(this, kDriveX, kDriveY, kDrive, &bigknob));
//  pGraphics->AttachControl(new IKnobMultiControl(this, kToneX, kToneY, kTone, &smallknob));
//  pGraphics->AttachControl(new IKnobMultiControl(this, kLevelX, kLevelY, kLevel, &bigknob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
  
  powerFilter.set_input_port(0, &inFilter, 0);
  attackReleaseFilter.set_input_port(0, &powerFilter, 0);
  gainCompressorFilter.set_input_port(0, &attackReleaseFilter, 0);
  applyGainFilter.set_input_port(0, &gainCompressorFilter, 0);
  applyGainFilter.set_input_port(1, &inFilter, 0);
  outFilter.set_input_port(0, &applyGainFilter, 0);
  
  Reset();
}

ATKCompressor::~ATKCompressor() {}

void ATKCompressor::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  inFilter.set_pointer(inputs[0], nFrames);
  outFilter.set_pointer(outputs[0], nFrames);
  outFilter.process(nFrames);
}

void ATKCompressor::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  int sampling_rate = GetSampleRate();
  
  inFilter.set_input_sampling_rate(sampling_rate);
  inFilter.set_output_sampling_rate(sampling_rate);
  powerFilter.set_input_sampling_rate(sampling_rate);
  attackReleaseFilter.set_input_sampling_rate(sampling_rate);
  gainCompressorFilter.set_input_sampling_rate(sampling_rate);
  applyGainFilter.set_input_sampling_rate(sampling_rate);
  outFilter.set_input_sampling_rate(sampling_rate);

  powerFilter.set_memory(1e-3 * GetSampleRate());
  attackReleaseFilter.set_attack(GetParam(kAttack)->Value() * 1e-3 * GetSampleRate()); // in ms
  attackReleaseFilter.set_release(GetParam(kRelease)->Value() * 1e-3 * GetSampleRate()); // in ms
}

void ATKCompressor::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      gainCompressorFilter.set_threshold(GetParam(kThreshold)->Value());
      break;
    case kSlope:
      gainCompressorFilter.set_slope(GetParam(kSlope)->Value()); // Should be an int?
      break;
    case kSoftness:
      gainCompressorFilter.set_softness(GetParam(kSoftness)->Value());
      break;
    case kAttack:
      attackReleaseFilter.set_attack(GetParam(kAttack)->Value() * 1e-3 * GetSampleRate()); // in ms
      break;
    case kRelease:
      attackReleaseFilter.set_release(GetParam(kRelease)->Value() * 1e-3 * GetSampleRate()); // in ms
      break;

    default:
      break;
  }
}
