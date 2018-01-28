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
      {"AP On/Off", 0, 'A'},
      {"Tgt DP Down", 0, 'D'},
      {"Tgt DP Up", 0, 'U'},
      {"Log On/Off", 0, 'L'},
      { "Reset Err Avg", 0, 'R' },
      { "Show Diags", 0, 'S' },
      { "AP VACC --", 0, '1' },
      { "AP VACC -1", 0, '2' },
      { "AP VACC  0", 0, '3' },
      { "AP VACC +1", 0, '4' },
      { "AP VACC ++", 0, '5' },
      { "AP AUTO", 0, '6' }
    };
    RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));
    RegisterFunction("AP", OAPI_KEY_A, &FlightTestRig::Button_AP);
    RegisterFunction("DP-", OAPI_KEY_D, &FlightTestRig::Button_DN);
    RegisterFunction("DP+", OAPI_KEY_U, &FlightTestRig::Button_UP);
    RegisterFunction("LOG", OAPI_KEY_L, &FlightTestRig::Button_LOG);
    RegisterFunction("REA", OAPI_KEY_R, &FlightTestRig::Button_REA);
    RegisterFunction("DIA", OAPI_KEY_S, &FlightTestRig::Button_DIA);
    RegisterFunction("A--", OAPI_KEY_1, &FlightTestRig::Button_A2D);
    RegisterFunction("A1D", OAPI_KEY_2, &FlightTestRig::Button_A1D);
    RegisterFunction("A0", OAPI_KEY_3, &FlightTestRig::Button_A0);
    RegisterFunction("A1U", OAPI_KEY_4, &FlightTestRig::Button_A1U);
    RegisterFunction("A++", OAPI_KEY_5, &FlightTestRig::Button_A2U);
    RegisterFunction("APA", OAPI_KEY_6, &FlightTestRig::Button_APA);


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