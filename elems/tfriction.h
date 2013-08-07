/***************************************************************************
                          tfriction.h  -  description
                             -------------------
    begin                : Jul 18 2003
    copyright            : (C) 2003-2013 by atu
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
   /** constructor */
   explicit TFriction( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFriction();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
 protected:
   /** main computation function */
   virtual double f( double t );
   /** reimplemented from TMiso to init state */
   virtual int do_startLoop( int acnx, int acny );
   
   /** Parameter: mody mass */
   PRM_DOUBLE1( mass, 0, "Mass", "Body mass", "min=1e-30" );
   /** Parameter: max dry friction force (if constant)  */
   PRM_DOUBLE1( f_mx, 0, "Max Ff",  "Max dry friction force (if constant) f_mx", "min=0" );
   /** Parameter: start force addition coeff */
   PRM_DOUBLE1( kf_mx, 0, "k_Ff", "Start force addition coefficient", "sep=col" );
   /** Parameter: viscous friction coeff */
   PRM_DOUBLE1( kfv, 0, "k_fv", "Viscous friction coeff", "");
   /** Switch: use u[1] as max dry friction force */
   PRM_SWITCH1( useMf, efNRC, "u[1] is f_mx", "Use u[1] as maximum friction force", "sep=col"  );
   /** Current speed */
   PRM_DOUBLE1( v, efInner, "v", "current speed", "" );
   /** Current friction force */
   PRM_DOUBLE1( Ff, efInner, "Ff", "current friction force", "" );
   /** state of mass: 0 -- sleep, 1 -- moving */
   PRM_INT1( bodyState, efInner, "State", "State: 0=Sleep, 1=Run", ""  );
   /** old values and misc */
   double v_old, x_old;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


