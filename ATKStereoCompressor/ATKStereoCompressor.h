#ifndef __ATKStereoCompressor__
#define __ATKStereoCompressor__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <ATK/Core/PipelineGlobalSinkFilter.h>

#include <ATK/Dynamic/AttackReleaseFilter.h>
#include <ATK/Dynamic/GainCompressorFilter.h>
#include <ATK/Dynamic/PowerFilter.h>

#include <ATK/Tools/ApplyGainFilter.h>
#include <ATK/Tools/MSFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

class ATKStereoCompressor : public IPlug
{
public:
  ATKStereoCompressor(IPlugInstanceInfo instanceInfo);
  ~ATKStereoCompressor();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  ATK::InPointerFilter<double> inLFilter;
  ATK::InPointerFilter<double> inRFilter;

  ATK::MiddleSideFilter<double> middlesidesplitFilter;
  ATK::VolumeFilter<double> volumesplitFilter;

  ATK::PowerFilter<double> powerFilter1;
  ATK::PowerFilter<double> powerFilter2;
  ATK::SumFilter<double> sumFilter; // in case we link both channels

  ATK::AttackReleaseFilter<double> attackReleaseFilter1;
  ATK::AttackReleaseFilter<double> attackReleaseFilter2;
  ATK::GainCompressorFilter<double> gainCompressorFilter1;
  ATK::GainCompressorFilter<double> gainCompressorFilter2;
  ATK::ApplyGainFilter<double> applyGainFilter;
  ATK::VolumeFilter<double> makeupFilter1;
  ATK::VolumeFilter<double> makeupFilter2;

  ATK::MiddleSideFilter<double> middlesidemergeFilter;
  ATK::VolumeFilter<double> volumemergeFilter;

  ATK::OutPointerFilter<double> outLFilter;
  ATK::OutPointerFilter<double> outRFilter;

  ATK::PipelineGlobalSinkFilter endpoint;

  IKnobMultiControl* attack2;
  IKnobMultiControl* release2;
  IKnobMultiControl* threshold2;
  IKnobMultiControl* ratio2;
  IKnobMultiControl* softness2;
  IKnobMultiControl* makeup2;
};

#endif
