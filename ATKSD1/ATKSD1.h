#ifndef __ATKSD1__
#define __ATKSD1__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Tools/OversamplingFilter.h>
#include <ATK/Tools/DecimationFilter.h>
#include <ATK/Tools/VolumeFilter.h>
#include <ATK/EQ/ButterworthFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/SD1ToneFilter.h>
#include <ATK/EQ/FirstOrderFilter.h>
#include <ATK/Distortion/SD1OverdriveFilter.h>

class ATKSD1 : public IPlug
{
public:
  ATKSD1(IPlugInstanceInfo instanceInfo);
  ~ATKSD1();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mDrive;
  double mTone;
  double mLevel;

  ATK::InPointerFilter<double> inFilter;
  ATK::OversamplingFilter<double, ATK::Oversampling6points5order_8<double> > oversamplingFilter;
  ATK::SD1OverdriveFilter<double> overdriveFilter;
  ATK::IIRFilter<ATK::ButterworthLowPassCoefficients<double> > lowpassFilter;
  ATK::DecimationFilter<double> decimationFilter;
  ATK::IIRFilter<ATK::SD1ToneCoefficients<double> > toneFilter;
  ATK::FirstOrderFilter<double> highpassFilter;
  ATK::VolumeFilter<double> volumeFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
