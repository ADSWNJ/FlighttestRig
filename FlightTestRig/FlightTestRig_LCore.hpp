// ==============================================================
//
//	FlightTestRig (Local Core Header)
//	============================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================

#include "windows.h"
#include "orbitersdk.h"
#include "FlightTestRig_Buttons.hpp"
#include "MFDPersist.hpp"
#include "FlightTestRig_GCore.hpp"
#include "FlightTestRig_VCore.hpp"
#include <list>
#include <string>

using namespace std;

#ifndef _FLIGHTTESTRIG_LCORE_H
#define _FLIGHTTESTRIG_LCORE_H

//+++++
// Local Persistence core. One of these is instantiated per Vessel AND MFD panel location. Local defaults for that combination.
//+++++

class FlightTestRig_LCore {
  public:
    // Local references ... instantiation, references for vesseland mfd position, and links to the appropriate VC, MC and GC
    FlightTestRig_LCore(VESSEL *vin, UINT mfdin, FlightTestRig_GCore* gcin);
    VESSEL *v;
    UINT m;
    FlightTestRig_GCore* GC;
    FlightTestRig_VCore* VC;

    // Add local vessel+panel data here

    FlightTestRig_Buttons B;
    bool showMessage{ false };
    bool okMessagePage{ true };
    string Message;
    int mode{ 0 };
    int PrvNxtMode;

    oapi::Sketchpad *skp; // points to local sketchpad for this MFD and vessel
    int skpLoB;           // Lowest precision for skp eng numnber formatting
    char skpBuf[128];     // Formatting buffer for MFD updates
    char skpFmtBuf[128];  // Formatting buffer for MFD updates
    int skpColPix;        // X-offset pixel (top left origin)
    int skpLinePix;       // Y offsel pixel (top left origin)

};


#endif // _FLIGHTTESTRIG_CORE_CLASSES