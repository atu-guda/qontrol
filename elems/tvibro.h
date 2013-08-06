/***************************************************************************
                          tvibro.h  -  description
                             -------------------
    begin                : Mon Aug 28 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TVIBRO_H
#define TVIBRO_H

#include <tmiso.h>

/**can simulate vibrational element
  *@author atu
  */

class TVibro : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TVibro( TDataSet* aparent );
   /** empty destructor */
   virtual ~TVibro();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** return ptr to static class_info, nust be implemented in each class */
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
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   
   /** damping */
   PRM_DOUBLE1( c0, 0, "c_0", "Damping coeficient", "" );
   /** natural frequency */
   PRM_DOUBLE1( Omega, 0, "\\Omega", "Natural frequency if c_0=0 and f(x)=x", "" );
   /** Initial velosity dx/dt(0) */
   PRM_DOUBLE1( v0, efNoRunChange, "v_0", "v(0)", "sep=col" );
   /** Use u[1] as return force element output */
   PRM_SWITCH1( use_u1, efNoRunChange, "u[1] is RF", 
        "Use u[1] as return force element output", "" );
   /** Curent velosity dx/dt */
   PRM_DOUBLE1( v, efInner, "v", "v(t)", "" );
   /** flag for start(1,2,0)  */
   int isStart;
   /** old values and misc */
   double u_old, f_old, x_old, x_old2, tdt2;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


