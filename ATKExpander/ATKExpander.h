#ifndef __ATKExpander__
#define __ATKExpander__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainExpanderFilter.h>
#include <ATK/Dynamic/PowerFilter.h>
#include <ATK/Tools/ApplyGainFilter.h>

class ATKExpander : public IPlug
{
public:
  ATKExpander(IPlugInstanceInfo instanceInfo);
  ~ATKExpander();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::PowerFilter<double> powerFilter;
  ATK::AttackReleaseFilter<double> attackReleaseFilter;
  ATK::GainExpanderFilter<double> gainExpanderFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
