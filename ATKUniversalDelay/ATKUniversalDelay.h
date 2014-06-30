#ifndef __ATKUniversalDelay__
#define __ATKUniversalDelay__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Delay/UniversalFixedDelayLineFilter.h>

class ATKUniversalDelay : public IPlug
{
public:
  ATKUniversalDelay(IPlugInstanceInfo instanceInfo);
  ~ATKUniversalDelay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mDrive;
  double mTone;
  double mLevel;

  ATK::InPointerFilter<double> inFilter;
  ATK::UniversalFixedDelayLineFilter<double> delayFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
