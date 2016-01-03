/***************************************************************************
                          tquadexrt.h - find top extrenum in quadratic approx
                           -------------------
    begin                : 2015.02.27
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

#ifndef TQUADEXTR_H
#define TQUADEXTR_H

#include <tmiso.h>



/** Find upper extremum in quadratic approximation by given 3 points
  *@author atu
  */

class TQuadExtr : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TQuadExtr);
   DCL_CREATE;
   DCL_STD_INF;

 protected:
   virtual double f( double t ) override;

   PRM_SWITCH( emulC, efNRC, "Emulate C", "Make virtual linear central point", "" );
   PRM_SWITCH( limitX, efNRC, "Limit X", "Limit X to half-width", "def=1" );
   PRM_DOUBLE( lim_s,  0, "lim_{s}", "Limit Scale", "sep=col\ndef=0.5" );
   PRM_SWITCH( limitG, efNRC, "Limit as given", "Limit X by given values", "sep=col" );
   PRM_DOUBLE( x_min,  0, "x_{min}", "Given minimal x", "sep=col\ndef=-1" );
   PRM_DOUBLE( x_max,  0, "x_{max}", "Given miximal x", "def=1" );
   // Outputs
   PRM_DOUBLE( a_1,   efInner, "a_1", "Coefficient at x", "" );
   PRM_DOUBLE( a_2,   efInner, "a_2", "Coefficient at x^2", "" );
   PRM_DOUBLE( x_cn,  efInner, "x_{cn}", "x of calculated extremum point", "" ); // main
   PRM_DOUBLE( x_cnt, efInner, "x_{cn}", "Relative x of calculated extremum point", "" );
   PRM_DOUBLE( y_cn,  efInner, "y_{cn}", "y of calculated extremum point", "" );
   PRM_DOUBLE( dy,    efInner, "dy",     "y_cn - y_c", "" );
   PRM_DOUBLE( dy_dx, efInner, "dy/dx",  "dy/(x_cn-x_c)", "" );
   PRM_DOUBLE( dy_sx, efInner, "dy_sx",  "dy*sign(x_cnt)", "" );
   // aux
   PRM_DOUBLE( x_lt,     efInner, "x_{lt}", "Relative x left point = x_l-x_c", "" );
   PRM_DOUBLE( x_rt,     efInner, "x_{rt}", "Relative x right point = x_r-x_c", "" );
   PRM_DOUBLE( y_lt,     efInner, "y_{lt}", "Relative y left point = y_l-y_c", "" );
   PRM_DOUBLE( y_rt,     efInner, "y_{rt}", "Relative y right point = y_r-y_c", "" );

   PRM_INPUT( x_r,   0, "x_r", "Coordinate on right point",  "sep=block" );
   PRM_INPUT( y_r,   0, "y_r", "Value on right point",  "" );
   PRM_INPUT( x_c,   0, "x_c", "Coordinate on central point",  "sep=col" );
   PRM_INPUT( y_c,   0, "y_c", "Value on central point",  "" );
   PRM_INPUT( x_l,   0, "x_l", "Coordinate on left point",  "sep=col" );
   PRM_INPUT( y_l,   0, "y_l", "Value on left point",  "" );

   Q_CLASSINFO( "nameHintBase",  "qextr_" );
   DCL_DEFAULT_STATIC;
};

#endif


