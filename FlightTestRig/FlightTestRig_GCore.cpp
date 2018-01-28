// ==============================================================
//
//	FlightTestRig (Core Persistence)
//	================================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================

#include "FlightTestRig_GCore.hpp"
#include "FlightTestRig_VCore.hpp"

FlightTestRig_GCore::FlightTestRig_GCore() {
  return;
}

FlightTestRig_GCore::~FlightTestRig_GCore() {
  return;
}


void FlightTestRig_GCore::corePreStep(double simT,double simDT,double mjd) {
  if (coreSimT == 0) {
    coreSimT = simT;
    return;
  }
  if (coreSimT == simT) return;

  if (P.firstVC() == NULL) return; // No vessels interested in FlightTestRig yet

  coreSimDT = simT - coreSimT;
  coreSimT = simT;
  //sprintf(oapiDebugString(),"GCORE PRESTEP: %15.15f", coreSimDT);

  // Once per update - call vessel corePreSteps
  for (FlightTestRig_VCore* VC = (FlightTestRig_VCore*) P.firstVC(); VC != NULL; VC = (FlightTestRig_VCore*) P.nextVC()) {
    VC->corePreStep(coreSimT, coreSimDT, mjd);
  }

}
