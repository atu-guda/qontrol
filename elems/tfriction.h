#pragma once
/***************************************************************************
                          tfriction.h  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2003-2019 by atu
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

/** Simulate of mass under external force and friction
  *@author atu
  */

class TFriction : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFriction);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init state */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( mass,      0, "&mass",    "Body mass", "min=1e-30\ndef=1" );
   PRM_PARAMD( f_mx,      0, "&f_{mx}",  "Max dry friction force (if constant) f_mx", "min=0\ndef=0.4" );
   PRM_PARAMD( kf_mx,     0, "&k_{Ff}",  "Start force addition coefficient", "sep=col" );
   PRM_PARAMD( kfv,       0, "k_{f&v}",  "Viscous friction coeff", "def=0.01");
   PRM_SWITCH( useMf, efNRC, "use in_f_mx", "Use in_f_mx as maximum friction force", "sep=col"  );

   PRM_DOUBLE( v,   efInner, "v", "current speed", "" );
   PRM_DOUBLE( a,   efInner, "a", "a", "" );
   PRM_DOUBLE( Ff,  efInner, "Ff", "current friction force", "" );
   PRM_INT( bodyState, efInner, "State", "State: 0=Sleep, 1=Run", ""  );

   PRM_INPUT( in_u,    0,  "&in_u",   "Main (source) input",  "sep=block" );
   PRM_INPUT( in_f_mx, 0, "in_{fmx}", "Maximum for input (if enabled)", "sep=col" );

   PRM_DOUBLE( x2, efInner, "x^2", "Squared output", "" );

   /** old values and misc */
   double v_old = 0, x_old = 0;

   Q_CLASSINFO( "nameHintBase",  "fric_" );
   DCL_DEFAULT_STATIC;
};



