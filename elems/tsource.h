/***************************************************************************
                          tsource.h  -  description
                             -------------------
    begin                : Thu Aug 24 2000
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

#ifndef TSOURCE_H
#define TSOURCE_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "tmiso.h"

static const char* const tsource_list_type = 
             "U*sin(o*t)\n"           // 0
             "U*sign(sin(o*t))\n"     // 1
             "U*t/T*sin(o*t)\n"       // 2
             "U*t/T*sign(sin(o*t))\n" // 3
             "Dirac(t-tau)\n"         // 4
             "U*Thetta(t-tau)\n"      // 5
             "U*t/T\n"                // 6
             "saw(t,tau) /|/|/| \n"   // 7
             "saw2(t.tau) /\\/\\ \n"  // 8
             "Chaos Wave(Phi)\n"      // 9
             "U*triangle(o*t)\n"      // 10
             "Phase"                 // 11
;

static const char* const tsource_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

/**This element provide different types
  * of input sources
  * @author atu
  */

class TSource : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSource);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TSource() override;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented to delete rnds */
   virtual int do_postRun( int good ) override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of input and misc flags */
   PRM_LIST( type, efNRC, "Type", "Source type", "", tsource_list_type );
   /** Amplitude */
   PRM_DOUBLE( uu, 0, "Amplitude", "Amplitude of signal (or pulse max)", "def=1" );
   /** Frequency */
   PRM_DOUBLE( omega, 0, "Frequency", "Frequency of source (or pulse width)", "def=0.7" );
   /** constant shift */
   PRM_DOUBLE( cc, 0, "C", "Constant base", "" );

   // ----------- U chaos ----------------------
   PRM_SWITCH( use_u_ch, efNRC, "use U chaos", "Use amplitude perturbations", "sep=col" );
   PRM_DOUBLE(  u_ch_v0, efNRC, "vU min"     , "Minimum value of amplitule factor", "def=0.5" );
   PRM_DOUBLE(  u_ch_vm, efNRC, "vU max"     , "Maximum value of amplitule factor", "def=1.5" );
   PRM_DOUBLE(  u_ch_t0, efNRC, "tU min"     , "Minimum time of change", "def=2\nmin=1e-20\nmax=1e20" );
   PRM_DOUBLE(  u_ch_tm, efNRC, "tU max"     , "Maximum time of change", "def=10\nmin=1e-20\nmax=1e20" );
   PRM_INT(      seed_u, efNRC, "U seed"     , "Random Generator seed", "def=RND" );
   PRM_LIST( seedType_u, efNRC, "U seed type", "When to seed", "def=3", tsource_list_seedType );
   PRM_SWITCH( addBaseSeed_u, efNRC, "U add base seed", "Add seed from base(model)", "def=1");
   
   // ----------- Phi chaos ----------------------
   PRM_SWITCH( use_f_ch,efNRC, "use Phi chaos", "Use phase perturbations", "sep=col" );
   PRM_DOUBLE( f_ch_v0, efNRC, "vF min"       , "Minimum value of phase factor", "def=-0.2" );
   PRM_DOUBLE( f_ch_vm, efNRC, "vF max"       , "Maximum value of phase factor", "def=0.2" );
   PRM_DOUBLE( f_ch_t0, efNRC, "tF min"       , "Minimum time of change", "def=2\nmin=1e-20\nmax=1e20" );
   PRM_DOUBLE( f_ch_tm, efNRC, "rF max"       , "Maximum time of change", "def=10\nmin=1e-20\nmax=1e20" );
   PRM_INT( seed_p, efNRC,     "F seed"       , "Random Generator seed", "def=RND" );
   PRM_LIST( seedType_p, efNRC,"F seed type"  , "When to seed", "def=3", tsource_list_seedType);
   PRM_SWITCH( addBaseSeed_p, efNRC, "F add base to seed", "Add seed from base(model)", "def=1" );
   
   /** chaos current values */
   double u_ch_vs = 1, u_ch_ve = 1, u_ch_ts = 1, u_ch_te = 1, u_ch_k = 1,
          f_ch_vs = 1, f_ch_ve = 1, f_ch_ts = 1, f_ch_te = 1, f_ch_k = 1;
   int sseed_u, sseed_p, bseed_u, bseed_p,
       eff_seedType_u, eff_seedType_p;
   
   
   /** generator structures from GSL */
   gsl_rng *rng_u = nullptr, *rng_p = nullptr;
   /** triggers */
   int was_pulse = 0;
   /** values to store some model vars */	  
   double tt;
   
   DCL_DEFAULT_STATIC;
};

#endif
