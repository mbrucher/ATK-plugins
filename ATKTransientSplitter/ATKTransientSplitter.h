#ifndef __ATKTransientSplitter__
#define __ATKTransientSplitter__

#include "IPlug_include_in_plug_hdr.h"
#include "controls.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>

#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainSwellFilter.h>
#include <ATK/Dynamic/PowerFilter.h>

#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/DryWetFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/OneMinusFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKTransientSplitter : public IPlug
{
public:
  ATKTransientSplitter(IPlugInstanceInfo instanceInfo);
  ~ATKTransientSplitter();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inFilter;
  ATK::PowerFilter<double> powerFilter;
  ATK::AttackReleaseFilter<double> slowAttackReleaseFilter;
  ATK::AttackReleaseFilter<double> fastAttackReleaseFilter;
  ATK::VolumeFilter<double> invertFilter;
  ATK::SumFilter<double> sumFilter;
  ATK::GainFilter<ATK::GainSwellFilter<double>> gainSwellFilter;
  ATK::OneMinusFilter<double> oneMinusFilter;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::OutPointerFilter<double> outTransientFilter;
  ATK::OutPointerFilter<double> outSustainFilter;
  ATK::PipelineGlobalSinkFilter sinkFilter;
};

#endif
