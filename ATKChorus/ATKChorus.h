#ifndef __ATKChorus__
#define __ATKChorus__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Delay/UniversalVariableDelayLineFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/SecondOrderFilter.h>
#include <ATK/Tools/OffsetVolumeFilter.h>
#include <ATK/Tools/WhiteNoiseGeneratorFilter.h>

class ATKChorus : public IPlug
{
public:
  ATKChorus(IPlugInstanceInfo instanceInfo);
  ~ATKChorus();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::WhiteNoiseGeneratorFilter<double> noiseGenerator;
  ATK::IIRFilter<ATK::LowPassCoefficients<double> > lowPass;
  ATK::OffsetVolumeFilter<double> offsetFilter;
  ATK::UniversalVariableDelayLineFilter<double> delayFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
