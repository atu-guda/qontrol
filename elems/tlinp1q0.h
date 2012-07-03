/***************************************************************************
                          tlinp1q0.h  -  description
                             -------------------
    begin                : Fri Sep 1 2000
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

#ifndef TLINP1Q0_H
#define TLINP1Q0_H

#include <tmiso.h>

#define CLASS_ID_TLinP1Q0 1050

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
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to set fx_elnu */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to init state */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** coefficient near f(x) */
   PRM_DOUBLE( a, 0 );
   /** coefficient near u(t)  */
   PRM_DOUBLE( ku, 0 );
   /** name of element to compute f(x) */
   PRM_STRING( fx, efNoRunChange );
   /** old value of x  */
   double x_old;
   /** element # */
   int elnu_fx;
   /** data descriptors -- with base class elements */ 
   static TDataInfo tlinp1q0_d_i[17];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   /** autoregister */
   static int registered;
   static int reg();
};


#endif


