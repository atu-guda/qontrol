#pragma once
/***************************************************************************
  tdyn3d.h - TDyn3D class declaration
                             -------------------
    begin                : 2016.04.12
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


#include "tmiso.h"

/** This element intragrates 3 inputs
  * @author atu
  */

class TDyn3D : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TDyn3D);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( c_x,     0, "c_{&x}", "Factor for v_x", "def=1.0" );
   PRM_PARAMD( c_y,     0, "c_{&y}", "Factor for v_y", "def=1.0" );
   PRM_PARAMD( c_z,     0, "c_{&z}", "Factor for v_z", "def=1.0" );
   /** start x, y, x values */
   PRM_PARAMD( x_0, efNRC, "x_0", "Initial value of x", "sep=col" );
   PRM_PARAMD( y_0, efNRC, "y_0", "Initial value of y", "" );
   PRM_PARAMD( z_0, efNRC, "z_0", "Initial value of z", "" );
   /** state/output variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );
   /** net speed */
   PRM_DOUBLE( v, efInner, "v",  "Scalar speed", ""  );

   PRM_INPUT( v_x, 0, "&v_x", "X speed input",  "sep=block" );
   PRM_INPUT( v_y, 0, "v_y",  "Y speed input",  "sep=col" );
   PRM_INPUT( v_z, 0, "v_z",  "Z speed input",  "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "d3_" );
   DCL_DEFAULT_STATIC;
};

