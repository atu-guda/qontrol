#pragma once
/***************************************************************************
                          tlorenz.h  -  description
                             -------------------
    begin                : Fri Mar 09 2012
    copyright            : (C) 2012-2016 by atu
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

/**This element simulates Loretz system
  * @author atu
  */

class TLorenz : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLorenz);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int miso_startLoop( long acnx, long acny ) override;
   void calcAux() noexcept;

   /** main system parameters */
   PRM_PARAMD( sigma,   0, "\\sigma", "\\sigma parameter", "def=10.0" );
   PRM_PARAMD( r,       0, "&r",      "r parameter",       "def=28.0" );
   PRM_PARAMD( b,       0, "&b",      "b parameter",       "def=2.6666667" );
   /** start x, y, x values */
   PRM_PARAMD( x_0, efNRC, "x_0", "Initial value of x", "sep=col\ndef=3.05" );
   PRM_PARAMD( y_0, efNRC, "y_0", "Initial value of y", "def=1.58" );
   PRM_PARAMD( z_0, efNRC, "z_0", "Initial value of z", "def=16.62" );
   /** state variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );
   /** its derivates */
   PRM_DOUBLE( vx, efInner, "vx", "dx/dt", "" );
   PRM_DOUBLE( vy, efInner, "vy", "dy/dt", ""  );
   PRM_DOUBLE( vz, efInner, "vz", "dz/dt", ""  );
   PRM_DOUBLE( v,  efInner, "v",  "dr/dt", ""  );
   /** misc usefull output values */
   PRM_DOUBLE( x2, efInner, "x2", "x^2", "" );
   PRM_DOUBLE( y2, efInner, "y2", "y^2", ""  );
   PRM_DOUBLE( z2, efInner, "z2", "z^2", ""  );
   PRM_DOUBLE( xy, efInner, "xy", "x*y", "" );
   PRM_DOUBLE( xz, efInner, "xz", "x*z", "" );
   PRM_DOUBLE( yz, efInner, "yz", "y*z", "" );

   PRM_INPUT( in_x, 0, "&x+", "X additional input",  "sep=block" );
   PRM_INPUT( in_y, 0, "y+",  "Y additional input",  "sep=col" );
   PRM_INPUT( in_z, 0, "z+",  "Z additional input",  "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "lor_" );
   DCL_DEFAULT_STATIC;
};

