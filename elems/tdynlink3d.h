#pragma once
/***************************************************************************
  tdynlink3d.h.h - TDynLink3D class declaration
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

/** This element intragrates 3 inputs and dependencies on x, y, z
  * @author atu
  */

class TDynLink3D : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TDynLink3D);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   // X
   PRM_PARAMD(  x_0, efNRC, "x_0", "Initial value of x", "" );
   PRM_PARAMD( cx_0,  0, "cx_0", "Steady component to x", "sep=col" );
   PRM_PARAMD( cx_x,  0, "cx_x", "Factor from x to x", "sep=block" );
   PRM_PARAMD( cx_y,  0, "cx_y", "Factor from y to x", "sep=col" );
   PRM_PARAMD( cx_z,  0, "cx_z", "Factor from z to x", "sep=col" );
   PRM_PARAMD( cx_x2, 0, "cx_{x2}", "Factor from x^2 to x", "sep=block" );
   PRM_PARAMD( cx_y2, 0, "cx_{y2}", "Factor from y^2 to x", "sep=col" );
   PRM_PARAMD( cx_z2, 0, "cx_{z2}", "Factor from z^2 to x", "sep=col" );
   PRM_PARAMD( cx_xy, 0, "cx_{xy}", "Factor from xy to  x", "sep=block" );
   PRM_PARAMD( cx_yz, 0, "cx_{yz}", "Factor from yz to  x", "sep=col" );
   PRM_PARAMD( cx_xz, 0, "cx_{xz}", "Factor from xz to  x", "sep=col" );
   //
   PRM_INPUT( v_xa, 0, "v_{xa}", "X speed addition",  "sep=block" );

   // Y
   PRM_PARAMD(  y_0, efNRC, "y_0", "Initial value of y", "sep=tab\ntabname=Y" );
   PRM_PARAMD( cy_0,  0, "cy_0", "Steady component to y", "sep=col" );
   PRM_PARAMD( cy_x,  0, "cy_x", "Factor from x to y", "sep=block" );
   PRM_PARAMD( cy_y,  0, "cy_y", "Factor from y to y", "sep=col" );
   PRM_PARAMD( cy_z,  0, "cy_z", "Factor from z to y", "sep=col" );
   PRM_PARAMD( cy_x2, 0, "cy_{x2}", "Factor from x^2 to y", "sep=block" );
   PRM_PARAMD( cy_y2, 0, "cy_{y2}", "Factor from y^2 to y", "sep=col" );
   PRM_PARAMD( cy_z2, 0, "cy_{z2}", "Factor from z^2 to y", "sep=col" );
   PRM_PARAMD( cy_xy, 0, "cy_{xy}", "Factor from xy to  y", "sep=block" );
   PRM_PARAMD( cy_yz, 0, "cy_{yz}", "Factor from yz to  y", "sep=col" );
   PRM_PARAMD( cy_xz, 0, "cy_{xz}", "Factor from xz to  y", "sep=col" );
   //
   PRM_INPUT( v_ya, 0, "v_{ya}", "Y speed addition",  "sep=block" );

   // Z
   PRM_PARAMD(  z_0, efNRC, "z_0", "Initial value of z", "sep=tab\ntabname=Z" );
   PRM_PARAMD( cz_0,  0, "cz_0", "Steady component to z", "sep=col" );
   PRM_PARAMD( cz_x,  0, "cz_x", "Factor from x to z", "sep=block" );
   PRM_PARAMD( cz_y,  0, "cz_y", "Factor from y to z", "sep=col" );
   PRM_PARAMD( cz_z,  0, "cz_z", "Factor from z to z", "sep=col" );
   PRM_PARAMD( cz_x2, 0, "cz_{x2}", "Factor from x^2 to z", "sep=block" );
   PRM_PARAMD( cz_y2, 0, "cz_{y2}", "Factor from y^2 to z", "sep=col" );
   PRM_PARAMD( cz_z2, 0, "cz_{z2}", "Factor from z^2 to z", "sep=col" );
   PRM_PARAMD( cz_xy, 0, "cz_{xy}", "Factor from xy to  z", "sep=block" );
   PRM_PARAMD( cz_yz, 0, "cz_{yz}", "Factor from yz to  z", "sep=col" );
   PRM_PARAMD( cz_xz, 0, "cz_{xz}", "Factor from xz to  z", "sep=col" );
   //
   PRM_INPUT( v_za, 0, "v_{za}", "Z speed addition",  "sep=block" );

   /** state/output variables */
   PRM_DOUBLE( x, efInner, "x", "x current value", "" );
   PRM_DOUBLE( y, efInner, "y", "y current value", ""  );
   PRM_DOUBLE( z, efInner, "z", "z current value", ""  );
   /** speeds */
   PRM_DOUBLE( v_x,  efInner, "v_x",  "X speed", ""  );
   PRM_DOUBLE( v_y,  efInner, "v_y",  "Y speed", ""  );
   PRM_DOUBLE( v_z,  efInner, "v_z",  "Z speed", ""  );
   PRM_DOUBLE(   v,  efInner, "v",    "Scalar speed", ""  );
   /** misc values */
   PRM_DOUBLE(  x2,  efInner, "x2",    "x^2", ""  );
   PRM_DOUBLE(  y2,  efInner, "y2",    "y^2", ""  );
   PRM_DOUBLE(  z2,  efInner, "z2",    "z^2", ""  );
   PRM_DOUBLE(  xy,  efInner, "xy",    "xy",  ""  );
   PRM_DOUBLE(  yz,  efInner, "yz",    "yz",  ""  );
   PRM_DOUBLE(  xz,  efInner, "xz",    "xz",  ""  );

   Q_CLASSINFO( "nameHintBase",  "d3l_" );
   DCL_DEFAULT_STATIC;
};

