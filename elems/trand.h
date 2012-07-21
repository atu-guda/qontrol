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

static const char* const trand_list_type = 
     "flat(-sigma,sigma)\n"       // 0
     "gaussian(sigma)\n"          // 1 
     "gaussian_tail(a,sigma)\n"   // 2
     "exponential(sigma)\n"       // 3
     "laplace(a)\n"               // 4
     "exppow(a,b)"                // 5
;

static const char* const trand_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

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
   /** return ptr to static class_info */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;      
   /** reimplemented from TMiso to delete generator */
   virtual int postRun( int good );
   /** reimplemented from TMiso to init generator and variables */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( double t );
 protected:
   /** reimplemented from TMiso to create generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** type of distribution */
   PRM_LIST1( type, efNRC, "Type", "Distribution type", "", trand_list_type );
   /** time of const output value, if <=0 -- chacge every tick */
   PRM_DOUBLE1( tau, efNRC, "\\tau", "time of const output value, if <=0 -- change every tick ", "" );
   /** amplitude of output */
   PRM_DOUBLE1( ampl, 0, "Amplitude", "Amplitude scale of output", "" );
   /** zero value of output */
   PRM_DOUBLE1( zval, 0, "Base value", "Zero value of output ", "" );
   /** coefficients of distribution */
   PRM_DOUBLE1( sigma, 0, "\\sigma", "\\sigma value of distrinution", "sep=col" );
   PRM_DOUBLE1( a, 0, "a", "Parameter a", "" );
   PRM_DOUBLE1( b, 0, "b", "Parameter b", "" );
   PRM_DOUBLE1( c, 0, "c", "Parameter c", "" );
   /** seed value for generator */
   PRM_INT1( seed, efNRC, "Seed", "Seed value for generator", "sep=col" );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST1( seedType, efNRC, "Seed type", "0 - every run 1- 1d loop .. 3-by model ", "",
       trand_list_seedType );
   /** flag: add base seed to element seed */
   PRM_SWITCH1( addBaseSeed, efNRC, "Add base", "Add base seed to element seed ", "" );
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
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


