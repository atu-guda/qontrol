#pragma once
/***************************************************************************
   tmatrix34.h - declaration of TMatrix34 class
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

/** This element applies matrix 3x4 trasform to 3D vector
  * @author atu
  */

class TMatrix34 : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TMatrix34);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   // virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( cx_x, 0, "c&x_x",   "a00", "def=1.0" );
   PRM_PARAMD( cy_x, 0, "c&y_x",   "a10", "def=0.0" );
   PRM_PARAMD( cz_x, 0, "c&z_x",   "a20", "def=0.0" );

   PRM_PARAMD( cx_y, 0, "cx_y",    "a01", "def=0.0\nsep=col" );
   PRM_PARAMD( cy_y, 0, "cy_y",    "a11", "def=1.0" );
   PRM_PARAMD( cz_y, 0, "cz_y",    "a21", "def=0.0" );

   PRM_PARAMD( cx_z, 0, "cx_z",    "a02", "def=0.0\nsep=col" );
   PRM_PARAMD( cy_z, 0, "cy_z",    "a12", "def=0.0" );
   PRM_PARAMD( cz_z, 0, "cz_z",    "a22", "def=1.0" );

   PRM_PARAMD( cx_0, 0, "cx_0",    "a03", "def=0.0\nsep=col" );
   PRM_PARAMD( cy_0, 0, "cy_0",    "a13", "def=0.0" );
   PRM_PARAMD( cz_0, 0, "cz_0",    "a23", "def=0.0" );

   /** state=output variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );

   PRM_INPUT( x_in, 0, "x_{&in}", "X input",  "sep=block" );
   PRM_INPUT( y_in, 0, "y_{in}",  "Y input", "sep=col" );
   PRM_INPUT( z_in, 0, "z_{in}",  "Z input",  "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "mat_" );
   DCL_DEFAULT_STATIC;
};

