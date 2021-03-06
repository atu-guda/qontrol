#pragma once
/***************************************************************************
  terrmeasure.h -- error measure element declaration
                             -------------------
    begin                : 2016.05.23
    copyright            : (C) 2016-2016 by atu
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

class TErrMeasure : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TErrMeasure);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;
   void reset_vals();

   PRM_PARAMD( p,   0, "&p",  "Power value", "min=1\ndef=2" );

   PRM_DOUBLE( t_rst,  efInner, "t_{rst}", "time from last reset", "" );
   PRM_DOUBLE( er,     efInner, "er",  "Current error value", "" );
   PRM_DOUBLE( er1,    efInner, "er1", "Current error absolute value", "" );
   PRM_DOUBLE( er2,    efInner, "er2", "Current error squared value", "" );
   PRM_DOUBLE( er2_0,  efInner, "er2_0", "Compare current error squared value", "" );
   PRM_DOUBLE( erp,    efInner, "erp", "Current error absolute value in the p power", "" );
   PRM_DOUBLE( mi1,    efInner, "mi1", "Integral of er1", "" );
   PRM_DOUBLE( mi2,    efInner, "mi2", "sqrt (integral of er2)", "" );
   PRM_DOUBLE( mi2_0,  efInner, "mi2_0", "sqrt (integral of er2_0)", "" );
   PRM_DOUBLE( mip,    efInner, "mip", "(integral of erp)^{1/p} ", "" );
   PRM_DOUBLE( m1,     efInner, "m1",  "mi1/t_{rst}", "" );
   PRM_DOUBLE( m1sq,   efInner, "m1sq","sqrt(mi1)", "" );
   PRM_DOUBLE( m12,    efInner, "m12", "m1^2", "" );
   PRM_DOUBLE( m2,     efInner, "m2",  "mi2/t_{rst}", "" );
   PRM_DOUBLE( m2_0,   efInner, "m2_0", "mi2_0/t_{rst}", "" );
   PRM_DOUBLE( m22,    efInner, "m22", "m2^2", "" );
   PRM_DOUBLE( mp,     efInner, "mp",  "mip/t_{rst}", "" );
   PRM_DOUBLE( mmax,   efInner, "mmax","max(er1)", "" );
   PRM_DOUBLE( r1,     efInner, "r1",  "relative m1", "" );
   PRM_DOUBLE( r1sq,   efInner, "r1sq","relative m1sq", "" );
   PRM_DOUBLE( r2,     efInner, "r2",  "relative m2", "" );
   PRM_DOUBLE( r22,    efInner, "r22", "relative m22", "" );
   PRM_DOUBLE( rp,     efInner, "rp",  "relative mp", "" );
   PRM_DOUBLE( rmax,   efInner, "rmax","relative mmax", "" );

   PRM_INPUT(  in_x,    0, "in_{&x}", "First input",  "sep=block" );
   PRM_INPUT(  in_y,    0, "in_{&y}", "Second input", "" );
   PRM_INPUT(  in_y0,   0, "in_{y0}", "Input to compare with", "" );
   PRM_LOGICIN( rst,    0, "&rst",    "Reset signal", "sep=col" );
   PRM_LOGICIN( enable, 0, "&enable", "Enable signal", "def=1" );

   double s2, s2_0, sp; //* summs for integrals;


   Q_CLASSINFO( "nameHintBase",  "err_" );
   DCL_DEFAULT_STATIC;
};




