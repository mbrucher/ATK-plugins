#ifndef __ATKLimiter__
#define __ATKLimiter__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainLimiterFilter.h>
#include <ATK/Dynamic/PowerFilter.h>
#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKLimiter : public IPlug
{
public:
  ATKLimiter(IPlugInstanceInfo instanceInfo);
  ~ATKLimiter();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::PowerFilter<double> powerFilter;
  ATK::AttackReleaseFilter<double> attackReleaseFilter;
  ATK::GainLimiterFilter<double> gainLimiterFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> volumeFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
