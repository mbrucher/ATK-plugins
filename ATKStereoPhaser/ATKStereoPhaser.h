#ifndef __ATKStereoPhaser__
#define __ATKStereoPhaser__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/PipelineGlobalSinkFilter.h>
#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/CustomIIRFilter.h>
#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/SinusGeneratorFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKStereoPhaser : public IPlug
{
public:
  ATKStereoPhaser(IPlugInstanceInfo instanceInfo);
  ~ATKStereoPhaser();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::IIRFilter<ATK::CustomIIRCoefficients<double> > allpass1Filter;
  ATK::IIRFilter<ATK::CustomIIRCoefficients<double> > allpass2Filter;
  ATK::SinusGeneratorFilter<double> sinusFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> volumeFilter;
  ATK::SumFilter<double> sum1Filter;
  ATK::SumFilter<double> sum2Filter;
  ATK::OutPointerFilter<double> out1Filter;
  ATK::OutPointerFilter<double> out2Filter;
  ATK::PipelineGlobalSinkFilter sinkFilter;
};

#endif
