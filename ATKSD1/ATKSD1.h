#ifndef __ATKSD1__
#define __ATKSD1__

#include "IPlug_include_in_plug_hdr.h"

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
};

#endif
