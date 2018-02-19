
//
// Coordinate Transform Helper Functions
//
// Purpose ... work on matrices and vectors, linear interpolation, etc
//
// (c) Andrew Stokes (ADSWNJ) 2012-2017
//
// All rights reserved
//

//#define ORBITER2010
#define ORBITER2016        // Elevation model

#include "CoordFunctions.hpp"
#include <cstdarg>

// Six coordinate systems are used...
// "llad" means longitude [deg], latitude [deg], altitude [m]... defining our 3D point relative to the planet core as a function of lon and lat angles and the local altitude from the elevation functions
// "lla" means longitude [rad], latitude [rad], altitude [m] ... defining our 3D point relative to the planet core as a function of lon and lat angles and the local altitude from the elevation functions
// "llr" means longitude [rad], latitude [rad], radius [m] ... defining our 3D point relative to the planet core as a function of lon and lat angles and a radius from core
// "gpos" global position, in global frame (ecliptic equinox J2000.0 frame)
// "rpos" relative position to main gravity source, in global frame (ecliptic equinox J2000.0 frame)
// "ecef" means ECEF Cartesian coordinates [m] from the core (X-axis is core to 0-lat 0-lon, Y-axis is core to 0-lat 90E-lon, Z-axis is North Pole 90N)
//
// Three relative vector systems are used...
// "ned" means north, east, down [m] ... i.e. on a flattened out map, this distance north, this distance east, this distance down (simplified spherical model, good for say 200km x 200km)
// "ahd" means azimuth [rad], horizontal-offset [m], down-offset range [m] ... i.e. this bearing for this horizontal distance, and this vertical down disatance
// "ahdd" means azimuth [deg], horizontal-offset [m], down-offset range [m] ... i.e. this bearing for this horizontal distance, and this vertical down disatance
//
// Conversion paths are as follows:
// LLAD <> LLA <> LLR <> ECEF 
// RPOS <> ECEF
// ECEF tgt + ECEF ref -> NED
// NED + ECEF ref -> ECEF tgt
// NED <> AHD <> AHDD
// 
// The cnv function automatically does intermediate transforms to deliver any to any conversions. Just remember to give it a reference v
// 

CoordFunctions::CoordFunctions(VESSEL *vin)
{
  v = vin;
}


/*
* \brief General coordinate conversion from any to any of these types: llad, lla, llr, rpos, ecef, ned, ahd, ahdd
* \param to_type     [enum] Desired target coordinate type
* \param from_type   [enum] Starting coordinate type
* \param start       [VECTOR3] Starting coordinate
* \opt-param reftype [enum] Reference coordinate type for ECEF<>NED conversions. Must be <= ECEF (i.e. not NED, AHD, AHDD)
* \opt-param ref     [VECTOR3] Reference coordinate for conversions between <= ecef and >= ned
* \return coordinate in requested format
*/
VECTOR3 CoordFunctions::cnv(const cf_type etype, const cf_type stype, ...) {

  va_list vl;
  va_start(vl, stype);
  const VECTOR3 start = va_arg(vl, const VECTOR3);

  cf_type cur = stype;
  VECTOR3 val = start;
  while (cur != etype) {
    switch (cur) {
    case LLAD:
        val = llad_to_lla(val);       cur = LLA;  break;
    case LLA:
      if (etype < cur) {
        val = lla_to_llad(val);       cur = LLAD; break;
      } else {
        val = lla_to_llr(val);        cur = LLR;  break;
      }
    case LLR:
      if (etype < cur) {
        val = llr_to_lla(val);        cur = LLA; break;
      } else {
        val = llr_to_ecef(val);       cur = ECEF;  break;
      }
    case GPOS:
        val = gpos_to_rpos(val);      cur = RPOS; break;
    case RPOS:
      if (etype == GPOS) {
        val = rpos_to_gpos(val);      cur = GPOS; break;
      } else {
        val = rpos_to_ecef(val);      cur = ECEF; break;
      }
    case ECEF:
      if (etype == GPOS || etype == RPOS) {
        val = ecef_to_rpos(val);      cur = RPOS; break;
      } else if (etype < cur) {
        val = ecef_to_llr(val);       cur = LLR;  break;
      } else {
        const cf_type ref_type = va_arg(vl, const cf_type);
        if (ref_type > ECEF) throw("Error in cnv: relative coord type not permitted here");
        const VECTOR3 ref = va_arg(vl, const VECTOR3);
        const VECTOR3 ref_ecef = this->cnv(ECEF, ref_type, ref);
        val = ecef_to_ned(ref_ecef, val);  cur = NED;  break;
      }
    case NED:
      if (etype < cur) {
        const cf_type ref_type = va_arg(vl, const cf_type);
        if (ref_type > ECEF) throw("Error in cnv: relative coord type not permitted here");
        const VECTOR3 ref = va_arg(vl, const VECTOR3);
        const VECTOR3 ref_ecef = this->cnv(ECEF, ref_type, ref);
        val = ned_to_ecef(ref_ecef, val);  cur = ECEF; break;
      } else {
        val = ned_to_ahd(val);        cur = AHD;  break;
      }
    case AHD:
      if (etype < cur) {
        val = ahd_to_ned(val);        cur = NED;  break;
      } else {
        val = ahd_to_ahdd(val);       cur = AHDD; break;
      }
    case AHDD:
        val = ahdd_to_ahd(val);       cur = AHD;  break;
    }
  }
  va_end(vl);
  return val;
}

/*
 * \brief Converts GPOS to RPOS
 * \param gpos Global coordinates
 * \return rpos  planet-relative position (global frame)
 */
VECTOR3 CoordFunctions::gpos_to_rpos(const VECTOR3 &gpos) {
  OBJHANDLE pH = v->GetGravityRef();
  VECTOR3 ppos, rpos;
  oapiGetGlobalPos(pH, &ppos);
  rpos = gpos - ppos;
  return rpos;
}

/*
 * \brief Converts RPOS to GPOS
 * \param rpos  planet-relative position (global frame)
 * \return gpos Global coordinates
 */
VECTOR3 CoordFunctions::rpos_to_gpos(const VECTOR3 &rpos) {
  OBJHANDLE pH = v->GetGravityRef();
  VECTOR3 ppos, gpos;
  oapiGetGlobalPos(pH, &ppos);
  gpos = rpos + ppos;
  return gpos;
}

/*
 * \brief Converts RPOS to ECEF
 * \param rpos  planet-relative position (global frame)
 * \return earth-centered earth-fixed coordinates 
 */
VECTOR3 CoordFunctions::rpos_to_ecef(const VECTOR3 &rpos) {
  MATRIX3 mat;
  oapiGetRotationMatrix(v->GetGravityRef(), &mat);
  VECTOR3 ecef_l = tmul(mat, rpos); // left handed ECEF from Orbiter
  VECTOR3 ecef{ ecef_l.x, ecef_l.z, ecef_l.y }; // convert to normal ECEF
  return ecef;
}

/*
 * \brief Converts ECEF to RPOS
 * \param ecef earth-centered earth-fixed coordinates 
 * \return planetary relative position (global frame)
 */
VECTOR3 CoordFunctions::ecef_to_rpos(const VECTOR3 &ecef) { 
  VECTOR3 rpos;
  MATRIX3 mat;
  oapiGetRotationMatrix(v->GetGravityRef(), &mat);
  VECTOR3 ecef_l{ ecef.x, ecef.z, ecef.y };
  rpos = mul(mat, ecef_l);
  return rpos;
}
/*
 * \brief Converts ECEF to LLR
 * \param ecef earth-centered earth-fixed coordinates
 * \return lon [rad], lat [rad], radius [m]
 */
VECTOR3 CoordFunctions::ecef_to_llr(const VECTOR3 &ecef) {
  double len = length(ecef);
  double lat = asin(ecef.z / len);
  double lon = atan2(ecef.y / len, ecef.x / len);
  VECTOR3 llr{ lon, lat, len };
  return llr;
}

/*
 * \brief Converts LLR to ECEF
 * \param llr  lon [rad], lat [rad], radius [m]
 * \return earth-centered earth-fixed coordinates
 */
VECTOR3 CoordFunctions::llr_to_ecef(const VECTOR3 &llr) {
  VECTOR3 ecef = _V(cos(llr.y)*cos(llr.x), cos(llr.y)*sin(llr.x), sin(llr.y))*llr.z;
  return ecef;
}

/*
 * \brief Converts LLR to LLA
 * \param llr  lon [rad], lat [rad], radius [m]
 * \return lon [rad], lat [rad], altitude [m]
 */
VECTOR3 CoordFunctions::llr_to_lla(const VECTOR3 &llr) {
  OBJHANDLE hPlanet = v->GetGravityRef();
  double surf_hgt = oapiGetSize(hPlanet);
  double alt;
#ifdef ORBITER2016
  ELEVHANDLE eh = oapiElevationManager(hPlanet);
  if (eh) {
    surf_hgt += oapiSurfaceElevation(hPlanet, llr.x, llr.y);
  }
#endif
  alt = llr.z - surf_hgt;
  VECTOR3 lla{ llr.x, llr.y, alt };
  return lla;
}

/*
 * \brief Converts LLA to LLR
 * \param lla  lon [rad], lat [rad], altitude [m]
 * \return lon [rad], lat [rad], radius [m]
 */
VECTOR3 CoordFunctions::lla_to_llr(const VECTOR3 &lla) {
  OBJHANDLE hPlanet = v->GetGravityRef();
  double surf_hgt = oapiGetSize(hPlanet);
#ifdef ORBITER2016
  ELEVHANDLE eh = oapiElevationManager(hPlanet);
  if (eh) {
    surf_hgt += oapiSurfaceElevation(hPlanet, lla.x, lla.y);
  }
#endif
  VECTOR3 llr{lla.x, lla.y, lla.z + surf_hgt};
  return llr;
}


/*
 * \brief Converts reference ECEF point to internal NED conversion matrix
 * \param ecef earth-centered earth-fixed coordinates of a reference point (e.g. base, vessel)
 * \return conversion matrix from ECEF to NED
 */
MATRIX3 CoordFunctions::ecef_to_mat(const VECTOR3 &ecef) {
  VECTOR3 northpole = _V(0.0, 0.0, 1.0);                              
  VECTOR3 d = ecef;                                                 
  normalise(d);
  VECTOR3 e = crossp(northpole, d);
  normalise(e);
  VECTOR3 n = crossp(d, e);
  normalise(n);
  MATRIX3 m{ n.x, n.y, n.z, e.x, e.y, e.z, d.x, d.y, d.z };
  return m;
}

/*
 * \brief Converts start ECEF plus NED into end ECEF
 * \param ecef_s  earth-centered earth-fixed coordinates of a reference start point (e.g. base, vessel)
 * \param ned  North [m] East [m] Down [m] offset
 * \return earth-centered earth-fixed coordinates of a end point
 */
VECTOR3 CoordFunctions::ned_to_ecef(const VECTOR3 &ecef_s, const VECTOR3 &ned) {
  MATRIX3 m = ecef_to_mat(ecef_s);
  VECTOR3 ecef_delta = tmul(m, ned);
  return ecef_s + ecef_delta;
}

/*
 * \brief Converts start ECEF and end ECEF into NED
 * \param ecef_s  earth-centered earth-fixed coordinates of a reference start point (e.g. base, vessel)
 * \param ned  North [m] East [m] Down [m] offset
 * \return earth-centered earth-fixed coordinates of a end point
 */
VECTOR3 CoordFunctions::ecef_to_ned(const VECTOR3 &ecef_s, const VECTOR3 &ecef_e) {
  MATRIX3 m = ecef_to_mat(ecef_s);
  VECTOR3 ts = mul(m, ecef_s);
  VECTOR3 te = mul(m, ecef_e);
  VECTOR3 ned{ te.x, te.y, te.z - ts.z };
  return ned;
}

/*
 * \brief Converts NED into AHD
 * \param ned  North [m] East [m] Down [m] offset
 * \return Azimuth [rad] Horizontal [m] Down [m] to target
 */
VECTOR3 CoordFunctions::ned_to_ahd(const VECTOR3 &ned) {
  VECTOR3 arr;
  arr.x = atan2(ned.y, ned.x);
  arr.y = sqrt(ned.x*ned.x + ned.y*ned.y);
  arr.z = ned.z;
  return arr;
}

/*
 * \brief Converts AHD into NED
 * \param ahd  Azimuth [rad] Horizontal [m] Down [m] to target
 * \return North [m] East [m] Down [m] to target
 */
VECTOR3 CoordFunctions::ahd_to_ned(const VECTOR3 &ahd) {
  VECTOR3 ned;
  ned.x = cos(ahd.x)*ahd.y;
  ned.y = sin(ahd.x)*ahd.y;
  ned.z = ahd.z;
  return ned;
}

/*
 * \brief Converts LLA to LLAD
 * \param lla  lon [rad], lat [rad], altitude [m]
 * \return lon [deg], lat [deg], altitude [m]
 */
VECTOR3 CoordFunctions::lla_to_llad(const VECTOR3 &lla) {
  VECTOR3 llad{ to_deg(lla.x), to_deg(lla.y), lla.z };
  return llad;
}

/*
 * \brief Converts AHD into AHDD
 * \param ahd  Azimuth [rad] Horizontal [m] Down [m] to target
 * \return Azimuth [deg] Horizontal [m] Down [m] to target
 */
VECTOR3 CoordFunctions::ahd_to_ahdd(const VECTOR3 &ahd) {
  VECTOR3 ahdd{ to_deg(ahd.x, true), ahd.y, ahd.z };
  return ahdd;
}

/*
 * \brief Converts LLAD to LLA
 * \param lla  lon [deg], lat [deg], altitude [m]
 * \return lon [rad], lat [rad], altitude [m]
 */
VECTOR3 CoordFunctions::llad_to_lla(const VECTOR3 &llad) {
  VECTOR3 lla{ to_rad(llad.x), to_rad(llad.y), llad.z };
  return lla;
}

/*
 * \brief Converts AHDD into AHD
 * \param ahd  Azimuth [deg] Horizontal [m] Down [m] to target
 * \return Azimuth [rad] Horizontal [m] Down [m] to target
 */
VECTOR3 CoordFunctions::ahdd_to_ahd(const VECTOR3 &ahdd) {
  VECTOR3 ahd{ to_rad(ahdd.x), ahdd.y, ahdd.z };
  return ahd;
}

/*
 * \brief Converts degrees to radians
 * \param deg  degree angle (any value)
 * \return radians angle in range >-PI to <=+PI
 */
double CoordFunctions::to_rad(const double d) {
  double a = d;
  while (a > 180.0) a -= 360.0;
  while (a <= -180.0) a += 360.0;
  return a * RAD;
}

/*
 * \brief Converts radians to degrees
 * \param rad  radian angle (any value)
 * \return degrees angle in range >= 0.0 to <360.0
 */
double CoordFunctions::to_deg(const double r, const bool plusonly) {
  double a = r * DEG;
  if (plusonly) {
    while (a >= 360.0) a -= 360.0;
    while (a < 0.0) a += 360.0;
  } else {
    while (a > 180.0) a -= 360.0;
    while (a <= -180.0) a += 360.0;
  }
  return a;
}


