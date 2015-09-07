#ifndef __ATKColoredExpander__
#define __ATKColoredExpander__

#include "IPlug_include_in_plug_hdr.h"
#include "controls.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>

#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainMaxColoredExpanderFilter.h>
#include <ATK/Dynamic/PowerFilter.h>

#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/DryWetFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKColoredExpander : public IPlug
{
public:
  ATKColoredExpander(IPlugInstanceInfo instanceInfo);
  ~ATKColoredExpander();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::PowerFilter<double> powerFilter;
  ATK::AttackReleaseFilter<double> attackReleaseFilter;
  ATK::GainMaxColoredExpanderFilter<double> gainExpanderFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> volumeFilter;
  ATK::DryWetFilter<double> drywetFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
