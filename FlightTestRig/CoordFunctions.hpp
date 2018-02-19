#pragma once

//
// Coordinate Helper Functions
//
// Purpose ... work on matrices and vectors, linear interpolation, etc
//
// (c) Andrew Stokes (ADSWNJ) 2012-2017
//
// All rights reserved
//


// Four coordinate systems are used...
// "llr" means longitude [rad], latitude [rad], radius [m] ... defining our 3D point realtive to the planet core as a function of long and lat angles and a radius from core
// "lla" means longitude [rad], latitude [rad], altitude [m] ... defining our 3D point realtive to the planet core as a function of long and lat angles and a local altitude at that point (or mean alt if no elevation data)
// "llad" means longitude [deg], latitude [deg], altitude [m] ... a display convenience in degrees (as all internal work is in radians)
// "ecef" means ECEF Right Handed Cartesian coordinates (X-axis points to 0-lon 0-lat, Y-axis points to 90E-lon, 0-lat, Z-axis points to North Pole 90N)
//
// Three relative vector systems are used...
// "rpos" means relative position to the planet, in global ecliptic frame coordinates (as delivered by orbiter's GetRelativePos() function)
// "ahd" means azimuth [rad], horizontal range [m], down (height) [m] from a refeence point (e.g. a base). Eg. ahd of {PI, 50000, 1000} is azimuth 180 deg, 50km ground track, 1km down
// "ned" means north [m], east [m], down [m] from a reference point (e.g. a base), on a local flat projection plane (useful within say 100 km of landing). 
//
// Transformation path:  RPOS <> ECEF <> LLR <> LLA -> LLAD 
// Relative transformation path:   ECEF+ECEF -> NED <> ARR -> ARRD, and ECEF+NED -> ECEF
//


#include "orbitersdk.h"
#include <limits>

#ifndef _COORD_FUNCTIONS_H
#define _COORD_FUNCTIONS_H
enum cf_type {LLAD = 0, LLA = 1, LLR = 2, GPOS = 10, RPOS = 11, ECEF = 20, NED = 30, AHD = 31, AHDD = 32};

class CoordFunctions {
public:
  CoordFunctions(VESSEL *vin);
  VECTOR3 cnv(const cf_type etype, const cf_type stype, ...);

private:
  VECTOR3 gpos_to_rpos(const VECTOR3 &gpos);
  VECTOR3 rpos_to_ecef(const VECTOR3 &rpos);
  VECTOR3 ecef_to_rpos(const VECTOR3 &ecef);
  VECTOR3 rpos_to_gpos(const VECTOR3 &rpos);
  VECTOR3 ecef_to_llr(const VECTOR3 &ecef);
  VECTOR3 llr_to_ecef(const VECTOR3 &llr);
  VECTOR3 llr_to_lla(const VECTOR3 &llr);
  VECTOR3 lla_to_llr(const VECTOR3 &lla);
  VECTOR3 ned_to_ecef(const VECTOR3 &ecef_s, const VECTOR3 &ned);
  VECTOR3 ecef_to_ned(const VECTOR3 &ecef_s, const VECTOR3 &ecef_e);
  VECTOR3 ned_to_ahd(const VECTOR3 &ned);
  VECTOR3 ahd_to_ned(const VECTOR3 &ahd);
  VECTOR3 lla_to_llad(const VECTOR3 &lla);
  VECTOR3 ahd_to_ahdd(const VECTOR3 &ahd);
  VECTOR3 llad_to_lla(const VECTOR3 &llad);
  VECTOR3 ahdd_to_ahd(const VECTOR3 &ahdd);
  MATRIX3 ecef_to_mat(const VECTOR3 &ecef);
  double to_rad(const double deg);
  double to_deg(const double rad, const bool plusonly = false);
  VESSEL *v;
};
#endif // __COORD_FUNCTIONS_H
