// ==============================================================
//
//	FlightTestRig (Button Handling Headers)
//	==================================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================



#ifndef _SCRAM_ATT_BUTTON_CLASS
#define _SCRAM_ATT_BUTTON_CLASS
#include "MFDButtonPage.hpp"

class FlightTestRig;

class FlightTestRig_Buttons : public MFDButtonPage<FlightTestRig>
{
  public:
    FlightTestRig_Buttons();
  protected:
    bool SearchForKeysInOtherPages() const;
  private:
};
#endif // _SCRAM_ATT_BUTTON_CLASS

