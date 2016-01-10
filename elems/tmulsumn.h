/***************************************************************************
        tmulsumn.cpp  - N-input multiplicator/summator + COG
                             -------------------
    begin                : 2015.11.15
    copyright            : (C) 2015-2016 by atu
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

#ifndef TMULSUMN_H
#define TMULSUMN_H

#include "tmiso.h"

/** N-input multiplicator/summator, + COG and other values
  *@author atu
  */

class TMulsumN : public TMiso  {
  Q_OBJECT
  struct ChInSim2 {
    InputSimple *in_p = nullptr, *in_f = nullptr;
  };
 public:
   DCL_CTOR(TMulsumN);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) noexcept override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;

   PRM_DOUBLE( spf,   efInner, "spf",  "Summ of p_i * f_i", "" );
   PRM_DOUBLE( sp,    efInner, "sp",   "Summ of p_i", "" );
   PRM_DOUBLE( sf,    efInner, "sf",   "Summ of f_i", "" );
   PRM_DOUBLE( pge,   efInner, "pge",  "Computed extremum (p)", "" );
   PRM_DOUBLE( spfl,  efInner, "spfl", "Local summ of p_i * f_i", "" );
   PRM_DOUBLE( spl,   efInner, "spl",  "Local summ of p_i", "" );
   PRM_DOUBLE( sfl,   efInner, "sfl",  "Local summ of f_i", "" );
   PRM_DOUBLE( ple,   efInner, "ple",  "Local computed extremum (p)", "" );
   PRM_DOUBLE( pee,   efInner, "pee",  "quadratic extremum approximation (p)", "" );
   PRM_DOUBLE( fee,   efInner, "fee",  "quadratic extremum approximation (f)", "" );
   PRM_DOUBLE( ne,    efInner, "ne",   "Index of point near extremum", "" );
   PRM_DOUBLE( pe,    efInner, "pe",   "Coorditate of point near extremum (p)", "" );
   PRM_DOUBLE( fe,    efInner, "fe",   "Value at point near extremum (f)", "" );
   PRM_DOUBLE( np,    efInner, "np",   "Number of detected points", "" );

   QVector<ChInSim2> pf_ins;

   Q_CLASSINFO( "nameHintBase",  "msum_" );
   DCL_DEFAULT_STATIC;
};

#endif
