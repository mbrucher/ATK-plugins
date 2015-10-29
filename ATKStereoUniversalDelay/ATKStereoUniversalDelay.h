#ifndef __ATKStereoUniversalDelay__
#define __ATKStereoUniversalDelay__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>
#include <ATK/Delay/StereoUniversalFixedDelayLineFilter.h>

class ATKStereoUniversalDelay : public IPlug
{
public:
  ATKStereoUniversalDelay(IPlugInstanceInfo instanceInfo);
  ~ATKStereoUniversalDelay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilterL;
  ATK::InPointerFilter<double> inFilterR;
  ATK::StereoUniversalFixedDelayLineFilter<double> delayFilter;
  ATK::OutPointerFilter<double> outFilterL;
  ATK::OutPointerFilter<double> outFilterR;
  ATK::PipelineGlobalSinkFilter endPoint;
};

#endif
