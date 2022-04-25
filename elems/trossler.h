#pragma once
/***************************************************************************
                          trossler.h  -  description
                             -------------------
    begin                : Thu May 03 2012
    copyright            : (C) 2012-2022 by atu
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

/**This element simulates Rossler system
  * @author atu
  */

class TRossler : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRossler);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int miso_startLoop( long acnx, long acny ) override;
   void calcAux() noexcept;
   virtual void preCalc() override;
   void pre_f() noexcept;

   /** main system parameters */
   PRM_PARAMD(   a,     0, "&a", "Parameter a", "def=0.25" );
   PRM_PARAMD(   b,     0, "&b", "Parameter b", "def=1" );
   PRM_PARAMD(   c,     0, "&c", "Parameter c", "def=5.58" );
   /** start x, y, x values */
   PRM_PARAMD( x_0, efNRC, "x_0", "Initial x value", "def=0.1\nsep=col" );
   PRM_PARAMD( y_0, efNRC, "y_0", "Initial y value", "" );
   PRM_PARAMD( z_0, efNRC, "z_0", "Initial z value", "" );
   /** variable of state */
   PRM_DOUBLE( x, efInner, "x", "x value", "" );
   PRM_DOUBLE( y, efInner, "y", "y value", "" );
   PRM_DOUBLE( z, efInner, "z", "z value", "" );
   //* derivates
   PRM_DOUBLE( vx, efInner, "vx", "dx/dt", "" );
   PRM_DOUBLE( vy, efInner, "vy", "dy/dt", "" );
   PRM_DOUBLE( vz, efInner, "vz", "dz/dt", "" );
   PRM_DOUBLE( v,  efInner, "v",  "dr/dt", ""  );
   /** misc usefull output values */
   PRM_DOUBLE( x2, efInner, "x2", "x^2", "" );
   PRM_DOUBLE( y2, efInner, "y2", "y^2", ""  );
   PRM_DOUBLE( z2, efInner, "z2", "z^2", ""  );
   PRM_DOUBLE( xy, efInner, "xy", "x*y", "" );
   PRM_DOUBLE( xz, efInner, "xz", "x*z", "" );
   PRM_DOUBLE( yz, efInner, "yz", "y*z", "" );

   PRM_INPUT( in_x, 0, "x+", "Addition to x",  "sep=block" );
   PRM_INPUT( in_y, 0, "y+", "Addition to y",  "sep=col" );
   PRM_INPUT( in_z, 0, "z+", "Addition to z",  "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "ross_" );
   DCL_DEFAULT_STATIC;
};

