/***************************************************************************
                          trand.h  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2012 by atu
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

#ifndef TRAND_H
#define TRAND_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <tmiso.h>


#define CLASS_ID_TRand 1025

/** random signal generator
  *@author atu
  */

class TRand : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TRand( TDataSet* aparent );
   /** destructor */
   virtual ~TRand();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** class id */
   virtual int getClassId(void) const ;
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
   /** return ptr to static class_info */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to create generator */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to delete generator */
   virtual int postRun( int good );
   /** reimplemented from TMiso to init generator and variables */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** type of distribution */
   PRM_LIST( type, efNoRunChange );
   /** time of const output value, if <=0 -- chacge every tick */
   PRM_DOUBLE( tau, efNoRunChange );
   /** amplitude of output */
   PRM_DOUBLE( ampl, 0 );
   /** zero value of output */
   PRM_DOUBLE( zval, 0 );
   /** coefficients of distribution */
   PRM_DOUBLE( sigma, 0 );
   PRM_DOUBLE( a, 0 );
   PRM_DOUBLE( b, 0 );
   PRM_DOUBLE( c, 0 );
   /** seed value for generator */
   PRM_INT( seed, efNoRunChange );
   /** use same seed every loop -- obsoleted by seedType */
   PRM_SWITCH( useSameSeed, efInner );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNoRunChange );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNoRunChange );
   /** data descriptors -- with base class elements */ 
   static TDataInfo trand_d_i[31];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   // @ iface end 
   /** time (may not be time) after previos value change */
   double sp_time;
   /** old input (usualy time) to calc difference */
   double old_in;
   /** keeped const output value */
   double old_val;
   /** start value of seed */
   int sseed;
   /** effective value of seedType */
   int eff_seedType;
   /** copy valuse of base seed */
   int bseed;
   /** generator structure from GSL */
   gsl_rng *rng;
};

#endif


