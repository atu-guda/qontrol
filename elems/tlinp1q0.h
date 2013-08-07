/***************************************************************************
                          tlinp1q0.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TLINP1Q0_H
#define TLINP1Q0_H

#include <tmiso.h>

/** simple linear diff element (aperiodic  1-st order)
  * with ability to handle nonlinear part
  * dx/dt= a * f(ku*u - x) 
  * or w/o nonliear part
  * 1/a * dx/dt + x = ku * u
  * a = 1 / tau
  * linear element:
  * W = ku / ( tau * p ) + 1
  * @author atu
  */

class TLinP1Q0 : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TLinP1Q0( TDataSet *aparent );
   /** empty destructor */
   virtual ~TLinP1Q0();
   /** creator */
   static TDataSet* create( TDataSet *apar );
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
   
   /** coefficient near f(x) */
   PRM_DOUBLE1( a, 0, "a", "a in dx/dt = a*f(ku*u(t)-x)", "" );
   /** coefficient near u(t)  */
   PRM_DOUBLE1( ku, 0, "k_u",  "Amplification", "" );
   /** name of element to compute f(x) */
   PRM_STRING1( fx, efNoRunChange | efNoDial, "old f(x) elem", "unused", "max=128" );
   /** Use u[1] as f(x) */
   PRM_SWITCH1( use_u1, efNoRunChange, "u[1] is f()", 
        "Use u[1] as source of f()", "" );
   /** old value of x  */
   double x_old;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};


#endif


