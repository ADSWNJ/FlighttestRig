// ==============================================================
//
//	FlightTestRig (Button Handling Code)
//	===============================
//
//	Copyright (C) 2017	Andrew (ADSWNJ) Stokes
//                   All rights reserved
//
//	See FlightTestRig.cpp
//
// ==============================================================

#include "FlightTestRig.hpp"
#include <math.h>

// ==============================================================
// MFD button hooks to Button Page library
//
char* FlightTestRig::ButtonLabel (int bt)
{
	return LC->B.ButtonLabel(bt);
}

// Return button menus
int FlightTestRig::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	return LC->B.ButtonMenu(menu);
}

// Return clicked button
bool FlightTestRig::ConsumeButton (int bt, int event) {
  return LC->B.ConsumeButton(this, bt, event);
}

// Return pressed keystroke
bool FlightTestRig::ConsumeKeyBuffered (DWORD key) {
  return LC->B.ConsumeKeyBuffered(this, key);
}



// ==============================================================
// MFD Button Handler Callbacks
//

// AP = AutoPilot On/Off
void FlightTestRig::Button_AP() {
  VC->apState = (VC->apState ? 0 : 1);
  VC->warpLock = false;
  if (VC->apState == 1) {
    VC->vAccPrevTgt = VC->vAccAvg;
  }
  return;
};

// DWN = Reduce Dyn Pressure Target
void FlightTestRig::Button_DN() {
  VC->DPTgt -= 1.0;
  return;
};

// UP = Increase Dyn Pressure Target
void FlightTestRig::Button_UP() {
  VC->DPTgt += 1.0;
  return;
};

// LOG = Toggle Log
void FlightTestRig::Button_LOG() {
  if (VC->logState == 2) {
    return;
  }
  if (VC->logState == 0) {
    VC->logOpen();
  } else {
    VC->logClose();
  }
  return;
};

// DIA = Toggle Diags
void FlightTestRig::Button_DIA() {
  VC->showDiags = !VC->showDiags;
  return;
};


// A2D = AP VACC --
void FlightTestRig::Button_A2D() {
  VC->apState = 2;
  VC->vAccTgt -= 1.0;
  VC->warpLock = false;
  return;
};

// A1D = AP VACC = -1
void FlightTestRig::Button_A1D() {
  VC->apState = 2;
  VC->vAccTgt = -1.0;
  VC->warpLock = false;
  return;
};

// A0 = AP VACC = 0
void FlightTestRig::Button_A0() {
  VC->apState = 2;
  VC->vAccTgt = 0.0;
  VC->warpLock = false;
  return;
};

// A1U = AP VACC = +1
void FlightTestRig::Button_A1U() {
  VC->apState = 2;
  VC->vAccTgt = 1.0;
  VC->warpLock = false;
  return;
};

// A1U = AP VACC ++
void FlightTestRig::Button_A2U() {
  VC->apState = 2;
  VC->vAccTgt += 1.0;
  VC->warpLock = false;
  return;
};

// APA = AP Auto-Control
void FlightTestRig::Button_APA() {
  VC->apState = 1;
  VC->vAccTgt = 0.0;
  VC->warpLock = false;
  return;
};

// REA = Reset Error Average
void FlightTestRig::Button_REA() {
  VC->sumAbsErr = 0.0;
  VC->startSumAbsErr = VC->lastSumAbsErr = oapiGetSimTime();
  return;
};


// NULL Button
void FlightTestRig::Button_NUL() {
  return;
};
