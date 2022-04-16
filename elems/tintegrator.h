#pragma once
/***************************************************************************
                          tintegrator.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2020 by atu
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


#include <tmiso.h>


/**realistic integrator with reset, discharge and floating base
  *@author atu
  */

class TIntegrator : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TIntegrator);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( ki,   0, QSL("&k_i"),  QSL("Factor before integral"), QSL("def=1") );
   PRM_PARAMD( vmin, 0, QSL("Min"),   QSL("Limit min value"), QSL("def=-10000") );
   PRM_PARAMD( vmax, 0, QSL("Max"),   QSL("Limit max value"), QSL("def=10000") );

   PRM_DOUBLE( v,     efInner, QSL("v"), QSL("Current value"), "" );
   PRM_DOUBLE( t_rst, efInner, QSL("t_{rst}"), QSL("time from last reset"), "" );
   PRM_DOUBLE( v_a,   efInner, QSL("v_a"), QSL("Average value"), "" );
   /** flags */
   PRM_SWITCH( useAver,  efNRC, QSL("output &Average"), QSL("Output average value"), QSL("sep=col") );
   PRM_SWITCH( useMin,   efNRC, QSL("limit Min value"), QSL("limit output to minimum value"), "" );
   PRM_SWITCH( useMax,   efNRC, QSL("limit Max value"), QSL("limit output to maximum value"), "" );
   PRM_SWITCH( useSqIn,  efNRC, QSL("x^{&2} on input"), QSL("Calculate square on input"), QSL("sep=col") );
   PRM_SWITCH( useSqrOut,efNRC, QSL("\u221As&qrt(output)"), QSL("Calculate square root on output"), "" );
   PRM_SWITCH( invKi,    efNRC, QSL("Inverse k_i"), QSL("Use 1/ki instead of ki"), "" );

   PRM_INPUT(     in_u, 0, QSL("&in_u"),   QSL("Main input"),     QSL("sep=block") );
   PRM_INPUT(    in_u1, 0, QSL("in_{u1}"), QSL("Negative input"), QSL("sep=col")  );
   PRM_INPUT(    in_u2, 0, QSL("in_{u2}"), QSL("Third+ input"),   QSL("sep=col") );

   PRM_LOGICIN( in_rst, 0, QSL("rst"),     QSL("Reset signal"), QSL("sep=block") );
   PRM_INPUT(    v_rst, 0, QSL("v_{rst}"), QSL("Value of v on reset"), QSL("sep=col") );

   PRM_DOUBLE( x2,   efInner, QSL("x^2"),   QSL("Squared output"), "" );
   PRM_DOUBLE( in,   efInner, QSL("in"),    QSL("Net input"), "" );
   PRM_DOUBLE( in_k, efInner, QSL("in_k"),  QSL("Scaled input"), "" );

   double v_old = 0;
   double last_rst = 0;
   bool useAver_c = false; // cached switch values
   bool useMin_c = false;
   bool useMax_c = false;
   bool useSqIn_c = false;
   bool useSqrOut_c = false;
   bool invKi_c = false;

   Q_CLASSINFO( "nameHintBase",  "ig_" );
   DCL_DEFAULT_STATIC;
};




