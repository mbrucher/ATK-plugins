#ifndef __ATKSD1__
#define __ATKSD1__

#include "IPlug_include_in_plug_hdr.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>

class ATKSD1 : public IPlug
{
public:
  ATKSD1(IPlugInstanceInfo instanceInfo);
  ~ATKSD1();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mDrive;
  double mTone;
  double mLevel;
  
  ATK::InPointerFilter<double> inFilter;
  ATK::OutPointerFilter<double> outFilter;
};

#endif
