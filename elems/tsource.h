/***************************************************************************
                          tsource.h  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000-2015 by atu
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

#include "tmiso.h"
#include "rand_gen.h"

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
   enum SourceType {
     so_sin = 0, so_sign = 1, so_sin_raise = 2, so_sign_raise = 3, so_dirac = 4,
     so_theta = 5, so_raise = 6, so_saw = 7, so_saw2 = 8, so_chaos_wave = 9,
     so_triangle = 10, so_phase = 11
   };
   Q_ENUMS(SourceType);
   Q_CLASSINFO( "enum_SourceType_0",  "U*sin(omega*t+phi)" );           // so_sin
   Q_CLASSINFO( "enum_SourceType_1",  "U*sign(sin(omega*t+phi))" );     // so_sign
   Q_CLASSINFO( "enum_SourceType_2",  "U*t/T*sin(omega*t+phi)" );       // so_sin_raise
   Q_CLASSINFO( "enum_SourceType_3",  "U*t/T*sign(sin(omega*t+phi))" ); // so_sign_raise
   Q_CLASSINFO( "enum_SourceType_4",  "Dirac(t-tau)" );                 // so_dirac
   Q_CLASSINFO( "enum_SourceType_5",  "U*Thetta(t-tau)" );              // so_theta
   Q_CLASSINFO( "enum_SourceType_6",  "U*t/T" );                        // so_raise
   Q_CLASSINFO( "enum_SourceType_7",  "U*saw(t,tau) /|/|/| " );         // so_saw
   Q_CLASSINFO( "enum_SourceType_8",  "U*saw2(t,tau) /\\/\\ " );        // so_saw2
   Q_CLASSINFO( "enum_SourceType_9",  "U*chaos Wave(Phi)" );            // so_chaos_wave
   Q_CLASSINFO( "enum_SourceType_10", "U*triangle(omega*t+phi)" );      // so_triangle
   Q_CLASSINFO( "enum_SourceType_11", "Phase" );                        // so_phase


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
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** type of input and misc flags */
   PRM_LIST( type, efNRC, "Type", "Source type", "enum=SourceType" ); //TODO
   /** Amplitude */
   PRM_DOUBLE( uu, 0, "U", "Amplitude of signal (or pulse max)", "def=1" );
   /** Frequency */
   PRM_DOUBLE( omega, 0, "omega", "Frequency of source (or pulse width)", "def=0.7" );
   /** constant shift */
   PRM_DOUBLE( cc, 0, "C", "Constant base", "" );
   /** phase shift */
   PRM_DOUBLE( phi, 0, "phi", "Phase shift", "def=0" );

   // ----------- U chaos ----------------------
   PRM_SWITCH( use_u_ch, efNRC, "use U chaos", "Use amplitude perturbations", "sep=col" );
   PRM_DOUBLE(  u_ch_v0, efNRC, "vU min"     , "Minimum value of amplitule factor", "def=0.5" );
   PRM_DOUBLE(  u_ch_vm, efNRC, "vU max"     , "Maximum value of amplitule factor", "def=1.5" );
   PRM_DOUBLE(  u_ch_t0, efNRC, "tU min"     , "Minimum time of change", "def=2\nmin=1e-20\nmax=1e20" );
   PRM_DOUBLE(  u_ch_tm, efNRC, "tU max"     , "Maximum time of change", "def=10\nmin=1e-20\nmax=1e20" );
   PRM_INT(      seed_u, efNRC, "U seed"     , "Random Generator seed", "def=RND" );
   PRM_LIST( seedType_u, efNRC, "U seed type", "When to seed", "def=3\nenum=SeedType" );
   PRM_SWITCH( addBaseSeed_u, efNRC, "U add base seed", "Add seed from base(model)", "def=1");

   // ----------- Phi chaos ----------------------
   PRM_SWITCH( use_f_ch,efNRC, "use Phi chaos", "Use phase perturbations", "sep=col" );
   PRM_DOUBLE( f_ch_v0, efNRC, "vF min"       , "Minimum value of phase factor", "def=-0.2" );
   PRM_DOUBLE( f_ch_vm, efNRC, "vF max"       , "Maximum value of phase factor", "def=0.2" );
   PRM_DOUBLE( f_ch_t0, efNRC, "tF min"       , "Minimum time of change", "def=2\nmin=1e-20\nmax=1e20" );
   PRM_DOUBLE( f_ch_tm, efNRC, "rF max"       , "Maximum time of change", "def=10\nmin=1e-20\nmax=1e20" );
   PRM_INT( seed_p, efNRC,     "F seed"       , "Random Generator seed", "def=RND" );
   PRM_LIST( seedType_p, efNRC,"F seed type"  , "When to seed", "def=3\nenum=SeedType" );
   PRM_SWITCH( addBaseSeed_p, efNRC, "F add base to seed", "Add seed from base(model)", "def=1" );

   // NO inputs

   /** chaos current values */
   double u_ch_vs = 1, u_ch_ve = 1, u_ch_ts = 1, u_ch_te = 1, u_ch_k = 1,
          f_ch_vs = 1, f_ch_ve = 1, f_ch_ts = 1, f_ch_te = 1, f_ch_k = 1;
   int sseed_u, sseed_p, bseed_u, bseed_p,
       eff_seedType_u, eff_seedType_p;


   /** generator structures */
   RandGenerator rng_u, rng_p;
   /** triggers */
   int was_pulse = 0;
   /** values to store some model vars */
   double tt;

   DCL_DEFAULT_STATIC;
};

#endif
