#pragma once
/***************************************************************************
  tbjt.h - Simple BJT transistor model (declarations)
                             -------------------
    begin                : 2016.08.12
    copyright            : (C) 2016-2017 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "tmiso.h"

/** Simple  BJT transistor model (Ebers-Moll)
  *@author atu
  */

class TBjt : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TBjt);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   // Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   /** coefficients on input and shift */
   PRM_SWITCH( isPNP,  efNRC, "PNP",      "Is this transistor PNP type (not working now)", "" );
   PRM_PARAMD( h_FE,       0, "&h_{FE}",  "Forward beta", "def=250\nmin=0.1" );
   PRM_PARAMD( I_s,        0, "I_s",      "Transport saturation current", "def=1.4e-14\nmin=0" );
   PRM_PARAMD( N_f,        0, "N_f",      "Forward emission coefficient", "def=1.3\nmin=0.1" );
   PRM_PARAMD( Temp,       0, "Temp",     "Temperature (K)", "def=300\nsep=col\nmin=1" );
   PRM_PARAMD( V_af,       0, "V_{af}",   "Forward Early voltage", "def=75\nmin=1" );
   PRM_PARAMD( R_ce0,      0, "R_{ce0}",  "Effective C-E resistance", "def=1\nmin=0.01" );
   PRM_SWITCH( useRc,  efNRC, "use R_c",  "Use only R_c in collector part", "sep=col" );
   PRM_PARAMD( V_cc,       0, "V_{cc}",   "V_cc for useRc", "def=5" );
   PRM_PARAMD( R_c,        0, "R_{c}",    "R_c for useRc", "def=100\nmin=0.01" );
   PRM_PARAMD( I_c_max,    0, "I_{c max}","Maximum I_c", "def=100\nmin=0.00001" );

   // * Outputs and misc states
   PRM_DOUBLE( I_c,     efInner, "I_c",  "Collector current = main output", "" );
   PRM_DOUBLE( I_b,     efInner, "I_b", "Base current", "" );
   PRM_DOUBLE( I_e,     efInner, "I_e", "Emitter current", "" );
   PRM_DOUBLE( V_t,     efInner, "V_t", "Thermal voltage = k_b T / q_e", "" );
   PRM_DOUBLE( V_be,    efInner, "V_{be}", "Base-emitter voltage", "" );
   PRM_DOUBLE( V_ce,    efInner, "V_{ce}", "Collector-emitter voltage", "" );
   PRM_DOUBLE( V_cb,    efInner, "V_{cb}", "Collector-base voltage", "" );
   PRM_DOUBLE( V_co,    efInner, "V_{co}", "Output V_c for useRc", "" );

   PRM_INPUT( V_b, 0, "V_{&b}", "Base potential",  "sep=block" );
   PRM_INPUT( V_c, 0, "V_{&c}", "Collector potential",  "sep=col" );
   PRM_INPUT( V_e, 0, "V_{&e}", "Emitter potential",  "sep=col" );

   static const constexpr double I_c_min = 1e-15; // 1fA

   Q_CLASSINFO( "nameHintBase",  "q_" );
   DCL_DEFAULT_STATIC;
};

