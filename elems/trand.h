/***************************************************************************
                          trand.h  -  description
                             -------------------
    begin                : Thu Dec 18 2003
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

#ifndef TRAND_H
#define TRAND_H

#include <tmiso.h>
#include "rand_gen.h"

static const char* const trand_list_type = 
     "flat(-sigma,sigma)\n"       // 0
     "gaussian(sigma)\n"          // 1 
     "gaussian_tail(a,sigma)\n"   // 2
     "exponential(sigma)\n"       // 3
     "laplace(a)\n"               // 4
     "exppow(a,b)\n"              // 5
     "beta(a,b)\n"                // 6
     "cauchy(a)\n"                // 7
     "chisq(a)\n"                 // 8
     "erlang(a,b)\n"              // 9
     "fdist(a,b)\n"               // 10
     "gamma(a,b)\n"               // 11
     "levy(a,b)\n"                // 12
     "logistic(a)\n"              // 13
     "lognormal(a,sigma)\n"       // 14
     "pareto(a,b)\n"              // 15
     "rayleigh(sigma)\n"          // 16 
     "weibull(a,b)"               // 17
;

static const char* const trand_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

/** random signal generator
  *@author atu
  */

class TRand : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRand);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to init generator and variables */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** type of distribution */
   PRM_LIST( type, efNRC, "Type", "Distribution type", "", trand_list_type );
   /** time of const output value, if <=0 -- change every tick */
   PRM_DOUBLE( tau, efNRC, "\\tau", "time of const output value, if <=0 -- change every tick ", "def=0.05" );
   /** amplitude of output */
   PRM_DOUBLE( ampl, 0, "Ampl", "Amplitude scale of output", "def=1" );
   /** zero value of output */
   PRM_DOUBLE( zval, 0, "Base", "Base (zero) value of output ", "" );
   PRM_SWITCH( inputTime, efNRC, " input Time", "input is time, not[0]", "def=1" );
   /** coefficients of distribution */
   PRM_DOUBLE( sigma, 0, "\\sigma", "\\sigma value of distrinution", "sep=col\ndef=1" );
   PRM_DOUBLE( a, 0, "a", "Parameter a", "def=1" );
   PRM_DOUBLE( b, 0, "b", "Parameter b", "def=1" );
   PRM_DOUBLE( c, 0, "c", "Parameter c", "def=1" );
   /** seed value for generator */
   PRM_INT( seed, efNRC, "Seed", "Seed value for generator", "sep=col\ndef=RND" );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNRC, "Seed type", "0 - every run 1- 1d loop .. 3-by model ", "def=3",
       trand_list_seedType );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNRC, "Add base", "Add base seed to element seed ", "def=1" );
   
   /** time (may not be time) after previos value change */
   double sp_time = DMAX;
   /** old input (usualy time) to calc difference */
   double old_in = 0;
   /** keeped const output value */
   double old_val = 0;
   /** start value of seed */
   int sseed;
   /** effective value of seedType */
   int eff_seedType;
   /** copy valuse of base seed */
   int bseed;
   /** generator structure from GSL */
   RandGenerator rng;
   
   DCL_DEFAULT_STATIC;
};

#endif


