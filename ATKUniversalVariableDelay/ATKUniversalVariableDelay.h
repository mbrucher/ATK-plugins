#ifndef __ATKUniversalVariableDelay__
#define __ATKUniversalVariableDelay__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Delay/UniversalVariableDelayLineFilter.h>
#include <ATK/Tools/CachedSinusGeneratorFilter.h>

class ATKUniversalVariableDelay : public IPlug
{
public:
  ATKUniversalVariableDelay(IPlugInstanceInfo instanceInfo);
  ~ATKUniversalVariableDelay();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::CachedSinusGeneratorFilter<double> sinusGenerator;
  ATK::UniversalVariableDelayLineFilter<double> delayFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
