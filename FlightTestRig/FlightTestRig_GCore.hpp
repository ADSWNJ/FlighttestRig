// ==============================================================
//
//	FlightTestRig (Global Core Header)
//	=============================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================

#include <list>
#include <string>
#include "windows.h"
#include "orbitersdk.h"
#include "FlightTestRig_Buttons.hpp"
#include "MFDPersist.hpp"
using namespace std;

#ifndef _FLIGHTTESTRIG_GCORE_H
#define _FLIGHTTESTRIG_GCORE_H


//+++++
// Global Persistence core. One of these is instantiated for the whole orbiter session, on the first launch of this MFD type
//+++++

class FlightTestRig_GCore {
  public:
    void corePreStep(double SimT,double SimDT,double mjd);

    // Global references ... instantiation and a link to the persistence library (running the linked lists)
    FlightTestRig_GCore();
    ~FlightTestRig_GCore();
    MFDPersist P;
    char moduleName[32];

  private:
    double coreSimT{ 0.0 };
    double coreSimDT;
};


#endif // _FLIGHTTESTRIG_GCORE_H