/***************************************************************************
                          tlorenz.h  -  description
                             -------------------
    begin                : Fri Mar 09 2012
    copyright            : (C) 2012-1024 by atu
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

#ifndef TLORENZ_H
#define TLORENZ_H

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
   virtual double f( double t ) override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** main system parameters */
   PRM_DOUBLE( sigma, 0, "\\sigma", "\\sigma parameter", "def=10.0" );
   PRM_DOUBLE( r, 0, "r", "r parameter", "def=28.0" );
   PRM_DOUBLE( b, 0, "b", "b parameter", "def=2.667" );
   /** start x, y, x values */
   PRM_DOUBLE( x_0, efNRC, "x_0", "Initial value of x", "sep=col\ndef=3.05" );
   PRM_DOUBLE( y_0, efNRC, "y_0", "Initial value of y", "def=1.58" );
   PRM_DOUBLE( z_0, efNRC, "z_0", "Initial value of z", "def=16.62" );
   /** state variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );
   /** its derivates */
   PRM_DOUBLE( vx, efInner, "vx", "dx/dt", "" );
   PRM_DOUBLE( vy, efInner, "vy", "dy/dt", ""  );
   PRM_DOUBLE( vz, efInner, "vz", "dz/dt", ""  );
   PRM_DOUBLE( v,  efInner, "v",  "dr/dt", ""  );

   PRM_INPUT( in_x, 0, "x+", "X additional input",  "sep=block" );
   PRM_INPUT( in_y, 0, "y+", "Y additional input", "sep=col" );
   PRM_INPUT( in_z, 0, "z+", "Z additional input",  "sep=col" );

   /** values to store some model vars */
   double tt;

   Q_CLASSINFO( "nameHintBase",  "lor_" );
   DCL_DEFAULT_STATIC;
};

#endif
