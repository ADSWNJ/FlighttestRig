// ==========================================================================
//
//	FlightTestRig (Local (Vessel+MFD Panel) Core Persistence)
//	=========================================================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==========================================================================

#include "FlightTestRig_GCore.hpp"
#include "FlightTestRig_VCore.hpp"
#include "FlightTestRig_LCore.hpp"

FlightTestRig_LCore::FlightTestRig_LCore(VESSEL *vin, UINT mfdin, FlightTestRig_GCore* gcin) {
  GC = gcin;
  v = vin;
  m = mfdin;
  VC = (FlightTestRig_VCore*) GC->P.findVC(v);
  return;
}
