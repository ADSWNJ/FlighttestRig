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
void FlightTestRig::Button_POS() {
  VC->setPos(0, 0, 30000, 15, 0);
  return;
};

