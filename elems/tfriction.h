/***************************************************************************
                          tfriction.h  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2003-2015 by atu
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

#ifndef TFRICTION_H
#define TFRICTION_H

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
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny ) override;

   PRM_DOUBLE( mass, 0, "Mass", "Body mass", "min=1e-30\ndef=1" );
   PRM_DOUBLE( f_mx, 0, "Max Ff",  "Max dry friction force (if constant) f_mx", "min=0\ndef=0.4" );
   PRM_DOUBLE( kf_mx, 0, "k_Ff", "Start force addition coefficient", "sep=col" );
   PRM_DOUBLE( kfv, 0, "k_fv", "Viscous friction coeff", "def=0.01");
   PRM_SWITCH( useMf, efNRC, "use in_f_mx", "Use in_f_mx as maximum friction force", "sep=col"  );
   /** Current speed */
   PRM_DOUBLE( v, efInner, "v", "current speed", "" );
   /** Current friction force */
   PRM_DOUBLE( Ff, efInner, "Ff", "current friction force", "" );
   /** state of mass: 0 -- sleep, 1 -- moving */
   PRM_INT( bodyState, efInner, "State", "State: 0=Sleep, 1=Run", ""  );

   PRM_INPUT( in_u, 0,  "input", "Main (source) input",  "sep=block" );
   PRM_INPUT( in_f_mx, 0, "input f_mx", "Maximum for input (if enabled)", "sep=col" );

   /** old values and misc */
   double v_old = 0, x_old = 0;
   DCL_DEFAULT_STATIC;
};

#endif


