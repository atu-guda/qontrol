/***************************************************************************
                          trand.h  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2014 by atu
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


/** random signal generator
*@author atu
*/

class TRand : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRand);
   DCL_CREATE;
   DCL_STD_INF;

   enum DistrType {
     dt_flat = 0, dt_gauss, dt_gausstail, dt_exp, dt_laplace, dt_exppow, dt_beta,
     dt_cauchy, dt_chisq, dt_erlang, dt_fdist, dt_gamma, dt_levy, dt_logistic,
     dt_lognorm, dt_pareto, dt_rayleigh, dt_weibull
   };
   Q_ENUMS(DistrType);
   Q_CLASSINFO( "enum_DistrType_0" , "flat(-sigma,sigma)"     ); // dt_flat
   Q_CLASSINFO( "enum_DistrType_1" , "gaussian(sigma)"        ); // dt_gauss
   Q_CLASSINFO( "enum_DistrType_2" , "gaussian_tail(a,sigma)" ); // dt_gausstail
   Q_CLASSINFO( "enum_DistrType_3" , "exponential(sigma)"     ); // dt_exp
   Q_CLASSINFO( "enum_DistrType_4" , "laplace(a)"             ); // dt_laplace
   Q_CLASSINFO( "enum_DistrType_5" , "exppow(a,b)"            ); // dt_exppow
   Q_CLASSINFO( "enum_DistrType_6" , "beta(a,b)"              ); // dt_beta
   Q_CLASSINFO( "enum_DistrType_7" , "cauchy(a)"              ); // dt_cauchy
   Q_CLASSINFO( "enum_DistrType_8" , "chisq(a)"               ); // dt_chisq
   Q_CLASSINFO( "enum_DistrType_9" , "erlang(a,b)"            ); // dt_erlang
   Q_CLASSINFO( "enum_DistrType_10", "fdist(a,b)"             ); // dt_fdist
   Q_CLASSINFO( "enum_DistrType_11", "gamma(a,b)"             ); // dt_gamma
   Q_CLASSINFO( "enum_DistrType_12", "levy(a,b)"              ); // dt_levy
   Q_CLASSINFO( "enum_DistrType_13", "logistic(a)"            ); // dt_logistic
   Q_CLASSINFO( "enum_DistrType_14", "lognormal(a,sigma)"     ); // dt_lognorm
   Q_CLASSINFO( "enum_DistrType_15", "pareto(a,b)"            ); // dt_pareto
   Q_CLASSINFO( "enum_DistrType_16", "rayleigh(sigma)"        ); // dt_rayleigh
   Q_CLASSINFO( "enum_DistrType_17", "weibull(a,b)"           ); // dt_weibull

   enum SeedType {
     everyRun = 0, startLoop, start2DLoop, asModel
   };
   Q_ENUMS(SeedType);
   Q_CLASSINFO( "enum_SeedType_0", "Every Run" );               // everyRun
   Q_CLASSINFO( "enum_SeedType_1", "On start on 1D loop" );     // startLoop
   Q_CLASSINFO( "enum_SeedType_2", "On start on 2D loop" );     // start2DLoop
   Q_CLASSINFO( "enum_SeedType_3", "As defined in Model" );     // asModel
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to init generator and variables */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of distribution */
   PRM_LIST( type, efNRC, "Type", "Distribution type", "enum=DistrType", "REMOVE_ME" );
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
   PRM_LIST( seedType, efNRC, "Seed type", "When get new seed", "def=3\nenum=SeedType", "REMOVE_ME" );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNRC, "Add base", "Add base seed to element seed ", "def=1" );

   PRM_INPUT( in_t, 0, "input t", "Input t if not used automaticaly",  "sep=block" );

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


