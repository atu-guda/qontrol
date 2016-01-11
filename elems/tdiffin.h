/***************************************************************************
                          tdiffin.h - diff between input signals
                             -------------------
    begin                : 2015.02.25
    copyright            : (C) 2015-2016 by atu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TDIFFIN_H
#define TDIFFIN_H

#include <tmiso.h>



/** differentiator of input signals: dy/dx, d^2y/dx^2
  *@author atu
  */

class TDiffIn : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TDiffIn);
   DCL_CREATE;
   DCL_STD_INF;

 protected:
   /** main computation function */
   virtual double f() noexcept override;

   PRM_SWITCH( emulC, efNRC, "Emulate C", "Make virtual linear cenral point", "" );
   // Outputs
   PRM_DOUBLE( d_rc, efInner, "d_rc", "Derivation between R and C", "" );
   PRM_DOUBLE( d_rl, efInner, "d_rl", "Derivation between R and L", "" ); // main
   PRM_DOUBLE( d_cl, efInner, "d_rl", "Derivation between C and L", "" );
   PRM_DOUBLE( d2,   efInner, "d2",   "d^2 y/dx^2", "" );
   // aux
   PRM_DOUBLE( dlt_x_rc, efInner, "dlt_x_rc", "x_r-x_c", "" );
   PRM_DOUBLE( dlt_x_rl, efInner, "dlt_x_rl", "x_r-x_l", "" );
   PRM_DOUBLE( dlt_x_cl, efInner, "dlt_x_cl", "x_c-x_l", "" );
   PRM_DOUBLE( dlt_y_rc, efInner, "dlt_y_rc", "y_r-y_c", "" );
   PRM_DOUBLE( dlt_y_rl, efInner, "dlt_y_rl", "y_r-y_l", "" );
   PRM_DOUBLE( dlt_y_cl, efInner, "dlt_y_cl", "y_c-y_l", "" );

   PRM_INPUT( x_r,   0, "x_r", "Coordinate on right point",  "sep=block" );
   PRM_INPUT( y_r,   0, "y_r", "Value on right point",  "" );
   PRM_INPUT( x_c,   0, "x_c", "Coordinate on central point",  "sep=col" );
   PRM_INPUT( y_c,   0, "y_c", "Value on central point",  "" );
   PRM_INPUT( x_l,   0, "x_l", "Coordinate on left point",  "sep=col" );
   PRM_INPUT( y_l,   0, "y_l", "Value on left point",  "" );

   Q_CLASSINFO( "nameHintBase",  "dif_" );
   DCL_DEFAULT_STATIC;
};

#endif


