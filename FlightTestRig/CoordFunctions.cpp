
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

// Four coordinate systems are used...
// "rpos" relative position to object, in global frame (ecliptic equinox J2000.0 frame)
// "llr" means longitude, latitude, radius ... defining our 3D point relative to the planet core as a function of lon and lat angles and a radius from core
// "lla" means longitude, latitude, altitude ... defining our 3D point relative to the planet core as a function of lon and lat angles and the local altitude from the elevation functions
// "ecef" means ECEF Cartesian coordinates from the core (X-axis is core to 0-lat 0-lon, Y-axis is core to 0-lat 90E-lon, Z-axis is North Pole 90N)
//
// Conversion track is RPOS to ECEF to LLR to LLA and back.
//
// Two vector systems are used...
// "arr" means azimuth, and range, radius ... i.e. this bearing for this distance, at this radius from the core of the planet
// "ned" means north, east, down ... i.e. on a flattened out map, this distance north, this distance east, this distance down (simplified spherical model, good for say 200km x 200km)
//
// Conversion track is ARR to NED and back.
// Target to ARR and back is: src LLR + tgt LLR to ARR, and src LLR + ARR to target LLR
// 

CoordFunctions::CoordFunctions(VESSEL *vin)
{
  v = vin;
}


/*
* \brief General coordinate conversion from any to any of these types: llad, lla, llr, rpos, ecef, ned, ahd, ahdd
* \param from_type  [enum]
* \param to_type    [enum]
* \param start      [VECTOR3] starting coordinate
* \opt-param ref    [VECTOR3] reference coordinate for conversions between <= ecef and >= ned
* \return coordinate in requersted format
*/
VECTOR3 CoordFunctions::cnv(const cf_type etype, const cf_type stype, const VECTOR3 &start, const VECTOR3 &ref) {

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
    case RPOS:
        val = rpos_to_ecef(val);      cur = ECEF; break;
    case ECEF:
      if (etype == RPOS) {
        val = ecef_to_rpos(val);      cur = RPOS; break;
      } else if (etype < cur) {
        val = ecef_to_llr(val);       cur = LLR;  break;
      } else {
        val = ecef_to_ned(ref, val);  cur = NED;  break;
      }
    case NED:
      if (etype < cur) {
        val = ned_to_ecef(ref, val);  cur = ECEF; break;
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
  return val;
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


