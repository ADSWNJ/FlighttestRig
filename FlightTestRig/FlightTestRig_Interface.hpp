// ==============================================================
//
//	FlightTestRig (Orbiter Interface)
//	============================
//
//	Copyright (C) 2016-2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
//

#ifndef SCRAM_ATT_INTERFACE
#define SCRAM_ATT_INTERFACE

#include "windows.h"
#include "orbitersdk.h"
#include "FlightTestRig.hpp"


class FlightTestRig_Interface : public oapi::Module {
public:
  FlightTestRig_Interface(HINSTANCE hDLL);
  ~FlightTestRig_Interface();
  void clbkSimulationStart(RenderMode mode);
  void clbkSimulationEnd();
  void clbkPreStep (double simt, double simdt, double mjd);
  //void clbkPostStep (double simt, double simdt, double mjd);
  void clbkDeleteVessel (OBJHANDLE hVessel);
  static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);

};
#endif // SCRAM_ATT_INTERFACE