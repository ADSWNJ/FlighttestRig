// ==============================================================
//
//	FlightTestRig (MFD Button Management)
//	=====================================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================

#include "MFDButtonPage.hpp"
#include "FlightTestRig_Buttons.hpp"
#include "FlightTestRig.hpp"


FlightTestRig_Buttons::FlightTestRig_Buttons() 
{
    static const MFDBUTTONMENU mnu0[] =
    {
      {"Position", 0, 'P'},
      {"Hold Pos", 0, 'H'},
      {"Enter Rot", 0, 'R'}
    };
    RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));
    RegisterFunction("POS", OAPI_KEY_P, &FlightTestRig::Button_POS);
    RegisterFunction("HLD", OAPI_KEY_P, &FlightTestRig::Button_HLD);
    RegisterFunction("ROT", OAPI_KEY_P, &FlightTestRig::Button_ROT);

    // Page 2, etc...
    //static const MFDBUTTONMENU mnu1[] =
    //{
    //  { "Mode Select", 0, 'M' }
    //};
    //RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));
    //RegisterFunction("MOD", OAPI_KEY_M, &FlightTestRig::Button_MOD);

    return;
}

bool FlightTestRig_Buttons::SearchForKeysInOtherPages() const
{
    return false;
}