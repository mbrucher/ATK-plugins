#ifndef __ATKSideChainExpander__
#define __ATKSideChainExpander__

#include "IPlug_include_in_plug_hdr.h"
#include "controls.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>

#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainColoredExpanderFilter.h>
#include <ATK/Dynamic/PowerFilter.h>

#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/DryWetFilter.h>
#include <ATK/Tools/MSFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKSideChainExpander : public IPlug
{
public:
  ATKSideChainExpander(IPlugInstanceInfo instanceInfo);
  ~ATKSideChainExpander();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inLFilter;
  ATK::InPointerFilter<double> inRFilter;
  ATK::InPointerFilter<double> inSideChainLFilter;
  ATK::InPointerFilter<double> inSideChainRFilter;

  ATK::MiddleSideFilter<double> middlesidesplitFilter;
  ATK::MiddleSideFilter<double> sidechainmiddlesidesplitFilter;
  ATK::VolumeFilter<double> volumesplitFilter;

  ATK::PowerFilter<double> powerFilter1;
  ATK::PowerFilter<double> powerFilter2;
  ATK::SumFilter<double> sumFilter; // in case we link both channels

  ATK::AttackReleaseFilter<double> attackReleaseFilter1;
  ATK::AttackReleaseFilter<double> attackReleaseFilter2;
  ATK::GainFilter<ATK::GainColoredExpanderFilter<double>> gainColoredExpanderFilter1;
  ATK::GainFilter<ATK::GainColoredExpanderFilter<double>> gainColoredExpanderFilter2;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> makeupFilter1;
  ATK::VolumeFilter<double> makeupFilter2;

  ATK::MiddleSideFilter<double> middlesidemergeFilter;
  ATK::VolumeFilter<double> volumemergeFilter;

  ATK::DryWetFilter<double> drywetFilter;

  ATK::OutPointerFilter<double> outLFilter;
  ATK::OutPointerFilter<double> outRFilter;

  ATK::PipelineGlobalSinkFilter endpoint;

  IKnobMultiControlText* power2;
  IKnobMultiControlText* attack2;
  IKnobMultiControlText* release2;
  IKnobMultiControlText* threshold2;
  IKnobMultiControlText* ratio2;
  IKnobMultiControl* softness2;
  IKnobMultiControl* color2;
  IKnobMultiControl* quality2;
  IKnobMultiControlText* makeup2;

  bool sidechain;
};

#endif
