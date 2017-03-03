#ifndef __ATKStereoLimiter__
#define __ATKStereoLimiter__

#include "IPlug_include_in_plug_hdr.h"
#include "controls.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>

#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainLimiterFilter.h>
#include <ATK/Dynamic/PowerFilter.h>

#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/DryWetFilter.h>
#include <ATK/Tools/MaxFilter.h>

class ATKStereoLimiter : public IPlug
{
public:
  ATKStereoLimiter(IPlugInstanceInfo instanceInfo);
  ~ATKStereoLimiter();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inLFilter;
  ATK::InPointerFilter<double> inRFilter;

  ATK::PowerFilter<double> powerFilter1;
  ATK::PowerFilter<double> powerFilter2;
  ATK::MaxFilter<double> maxFilter;

  ATK::AttackReleaseFilter<double> attackReleaseFilter;
  ATK::GainFilter<ATK::GainLimiterFilter<double>> gainLimiterFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;

  ATK::DryWetFilter<double> drywetFilter;

  ATK::OutPointerFilter<double> outLFilter;
  ATK::OutPointerFilter<double> outRFilter;

  ATK::PipelineGlobalSinkFilter endpoint;
};

#endif
