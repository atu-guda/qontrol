#pragma once
/***************************************************************************
                          trand.h  -  description
                             -------------------
    begin                : Thu Dec 18 2003
    copyright            : (C) 2003-2016 by atu
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
     dt_lognorm, dt_pareto, dt_rayleigh, dt_weibull, dt_none
   };
   Q_ENUM(DistrType);
   Q_CLASSINFO( "enum_DistrType_0" , "flat(-\\sigma,\\sigma)"   ); // dt_flat
   Q_CLASSINFO( "enum_DistrType_1" , "gaussian(\\sigma)"        ); // dt_gauss
   Q_CLASSINFO( "enum_DistrType_2" , "gaussian_tail(a,\\sigma)" ); // dt_gausstail
   Q_CLASSINFO( "enum_DistrType_3" , "exponential(\\sigma)"     ); // dt_exp
   Q_CLASSINFO( "enum_DistrType_4" , "laplace(a)"               ); // dt_laplace
   Q_CLASSINFO( "enum_DistrType_5" , "exppow(a,b)"              ); // dt_exppow
   Q_CLASSINFO( "enum_DistrType_6" , "beta(a,b)"                ); // dt_beta
   Q_CLASSINFO( "enum_DistrType_7" , "cauchy(a)"                ); // dt_cauchy
   Q_CLASSINFO( "enum_DistrType_8" , "chisq(a)"                 ); // dt_chisq
   Q_CLASSINFO( "enum_DistrType_9" , "erlang(a,b)"              ); // dt_erlang
   Q_CLASSINFO( "enum_DistrType_10", "fdist(a,b)"               ); // dt_fdist
   Q_CLASSINFO( "enum_DistrType_11", "gamma(a,b)"               ); // dt_gamma
   Q_CLASSINFO( "enum_DistrType_12", "levy(a,b)"                ); // dt_levy
   Q_CLASSINFO( "enum_DistrType_13", "logistic(a)"              ); // dt_logistic
   Q_CLASSINFO( "enum_DistrType_14", "lognormal(a,\\sigma)"     ); // dt_lognorm
   Q_CLASSINFO( "enum_DistrType_15", "pareto(a,b)"              ); // dt_pareto
   Q_CLASSINFO( "enum_DistrType_16", "rayleigh(\\sigma)"        ); // dt_rayleigh
   Q_CLASSINFO( "enum_DistrType_17", "weibull(a,b)"             ); // dt_weibull
   Q_CLASSINFO( "enum_DistrType_18", "none"                     ); // dt_none

   enum SeedType {
     everyRun = 0, startLoop, start2DLoop, asModel
   };
   Q_ENUM(SeedType);
   Q_CLASSINFO( "enum_SeedType_0", "Every Run" );               // everyRun
   Q_CLASSINFO( "enum_SeedType_1", "On start on 1D loop" );     // startLoop
   Q_CLASSINFO( "enum_SeedType_2", "On start on 2D loop" );     // start2DLoop
   Q_CLASSINFO( "enum_SeedType_3", "As defined in Model" );     // asModel
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   /** reimplemented from TMiso to init generator */
   virtual int do_preRun() override;
   /** reimplemented from TMiso to init generator and variables */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_LIST(  type, efNRC, "&Type", "Distribution type", "enum=DistrType" );
   PRM_PARAMD( tau,     0, "\\tau", "time of const output value, if <=0 -- change every tick ", "def=0.05" );
   PRM_PARAMD( ampl,    0, "A&mpl", "Amplitude scale of output", "def=1" );
   PRM_PARAMD( zval,    0, "&Zero", "Base (zero) value of output ", "" );
   PRM_SWITCH( inputTime, efNRC, " input Time", "input is time, not[0]", "def=1" );
   /** coefficients of distribution */
   PRM_PARAMD( sigma, 0, "\\sigma", "\\sigma value of distribution", "sep=col\ndef=1" );
   PRM_PARAMD(     a, 0, "&a", "Parameter a", "def=1" );
   PRM_PARAMD(     b, 0, "&b", "Parameter b", "def=1" );
   PRM_PARAMD(     c, 0, "&c", "Parameter c", "def=1" );
   /** seed value for generator */
   PRM_INT( seed, efNRC, "Seed", "Seed value for generator", "sep=col\ndef=RND" );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNRC, "Seed type", "When get new seed", "def=3\nenum=SeedType" );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNRC, "Add base", "Add base seed to element seed ", "def=1" );
   PRM_SWITCH( addHash,     efNRC, "Add hash", "Add hash value of full name ", "def=1" );

   PRM_SWITCH( useDiscr,  efNRC, "Discretization", "Use discretization noise ", "def=0\nsep=block" );
   PRM_PARAMD(      d_h,      0, "d_{h}",    "Discretization step", "def=0.1\nmin=1e-12\nsep=col" );
   PRM_PARAMD(   d_base,      0, "d_{base}", "Discretization base", "def=0\nsep=col" );

   PRM_INPUT( in_t, 0, "&in_t", "Input t if not used automatically",  "sep=block" );
   PRM_INPUT( in_v, 0, "&in_v", "Additional input",  "sep=col" );

   PRM_DOUBLE(  x2, efInner, "x2", "x^2", "" );

   /** time (may not be time) after previous value change */
   double sp_time = DMAX;
   /** old input (usually time) to calc difference */
   double old_in = 0;
   /** stored const output value */
   double old_val = 0;
   /** start value of seed */
   int sseed = 1;
   /** effective value of seedType */
   int eff_seedType = 0;
   /** copy values of base seed */
   int bseed = 1;
   int hseed = 0; // seed from full name hash value. Required for subschemes.
   /** generator structure from GSL */
   RandGenerator rng;

   DCL_DEFAULT_STATIC;
   Q_CLASSINFO( "nameHintBase",  "rnd_" );
};



