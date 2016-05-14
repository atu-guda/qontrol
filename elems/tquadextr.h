#pragma once
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
   virtual double f() noexcept override;

   PRM_SWITCH( emulC,  efNRC, "Emulate C", "Make virtual linear central point", "" );
   PRM_SWITCH( limitX, efNRC, "Limi&t X",  "Limit X to", "def=1\nsep=col" );
   PRM_PARAMD( lim_s,  0,     "lim_{&s}",  "Limit Scale", "def=0.99" );
   PRM_SWITCH( limitG, efNRC, "Limit as &given", "Limit X by given values", "sep=col" );
   PRM_PARAMD( x_min,  0,     "x_{&min}", "Given minimal x", "sep=col\ndef=-1" );
   PRM_PARAMD( x_max,  0,     "x_{max}", "Given miximal x", "def=1" );
   // Outputs
   PRM_DOUBLE( a_1,   efInner, "a_1", "Coefficient at x", "" );
   PRM_DOUBLE( a_2,   efInner, "a_2", "Coefficient at x^2", "" );
   PRM_DOUBLE( x_cn,  efInner, "x_{cn}", "x of calculated extremum point", "" ); // main
   PRM_DOUBLE( x_cnt, efInner, "x_{cn}", "Relative x of calculated extremum point", "" );
   PRM_DOUBLE( y_cn,  efInner, "y_{cn}", "y of calculated extremum point", "" );
   PRM_DOUBLE( dy,    efInner, "dy",     "y_cn - y_c", "" );
   PRM_DOUBLE( dy_dx, efInner, "dy/dx",  "dy/(x_cn-x_c)", "" );
   PRM_DOUBLE( dy_sx, efInner, "dy_sx",  "dy*sign(x_cnt)", "" );
   PRM_DOUBLE( f_c,   efInner, "f_c",    "a-la Coulomb force", "" );
   // aux
   PRM_DOUBLE( x_lt,  efInner, "x_{lt}", "Relative x left point = x_l-x_c", "" );
   PRM_DOUBLE( x_rt,  efInner, "x_{rt}", "Relative x right point = x_r-x_c", "" );
   PRM_DOUBLE( y_lt,  efInner, "y_{lt}", "Relative y left point = y_l-y_c", "" );
   PRM_DOUBLE( y_rt,  efInner, "y_{rt}", "Relative y right point = y_r-y_c", "" );

   PRM_INPUT( x_l,   0, "x_{&l}", "Left point argument",  "sep=block" );
   PRM_INPUT( y_l,   0, "&y_{l}", "Left point value",  "" );
   PRM_INPUT( x_c,   0, "x_{&c}", "Central point argument",  "sep=col" );
   PRM_INPUT( y_c,   0, "y_{c}",  "Central point value",  "" );
   PRM_INPUT( x_r,   0, "x_{&r}", "Right point argument",  "sep=col" );
   PRM_INPUT( y_r,   0, "y_{r}",  "Right point value",  "" );

   Q_CLASSINFO( "nameHintBase",  "qextr_" );
   DCL_DEFAULT_STATIC;
};



