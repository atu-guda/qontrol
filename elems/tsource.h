#pragma once
/***************************************************************************
                          tsource.h  -  description
                             -------------------
    begin                : Thu Aug 24 2000
    copyright            : (C) 2000-2019 by atu
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
     so_sin = 0, so_sign, so_sin_raise, so_sign_raise, so_dirac,
     so_theta, so_raise, so_saw, so_saw2, so_chaos_wave, // = 9
     so_triangle, so_phase, so_pulse, so_pwm, so_ladder // = 14
   };
   Q_ENUM(SourceType);
   Q_CLASSINFO( "enum_SourceType_0",  "U*sin(\\omega*t+\\phi)" );           // so_sin
   Q_CLASSINFO( "enum_SourceType_1",  "U*sign(sin(\\omega*t+\\phi))" );     // so_sign
   Q_CLASSINFO( "enum_SourceType_2",  "U*t/T*sin(\\omega*t+\\phi)" );       // so_sin_raise
   Q_CLASSINFO( "enum_SourceType_3",  "U*t/T*sign(sin(\\omega*t+\\phi))" ); // so_sign_raise
   Q_CLASSINFO( "enum_SourceType_4",  "Dirac(t-\\tau)" );                   // so_dirac
   Q_CLASSINFO( "enum_SourceType_5",  "U*Thetta(t-\\tau)" );                // so_theta
   Q_CLASSINFO( "enum_SourceType_6",  "U*t/T" );                            // so_raise
   Q_CLASSINFO( "enum_SourceType_7",  "U*saw(t) /|/|/| " );                 // so_saw
   Q_CLASSINFO( "enum_SourceType_8",  "U*saw2(t) /\\\\/\\\\ " );            // so_saw2
   Q_CLASSINFO( "enum_SourceType_9",  "U*chaos Wave(\\phi)" );              // so_chaos_wave
   Q_CLASSINFO( "enum_SourceType_10", "U*triangle(\\omega*t+\\phi)" );      // so_triangle
   Q_CLASSINFO( "enum_SourceType_11", "Phase" );                            // so_phase
   Q_CLASSINFO( "enum_SourceType_12", "Pulse" );                            // so_pulse
   Q_CLASSINFO( "enum_SourceType_13", "PWM" );                              // so_pwm
   Q_CLASSINFO( "enum_SourceType_14", "ladder" );                           // so_ladder


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
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun() override;
   /** called before each inner param loop. Unused param - -1 */
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_LIST(        type, efNRC, QSL("&Type")   , QSL("Source type")                              , QSL("enum=SourceType") ); //TODO
   PRM_SWITCH( use_rfreq, efNRC, QSL("Natural") , QSL("Use natural frequency instead of \\omega") , QES );
   PRM_PARAMD(        uu,     0, QSL("&U")      , QSL("Amplitude of signal (or pulse max)")       , QSL("def=1") );
   PRM_PARAMD(     omega,     0, QSL("\\omega") , QSL("Frequency of source (or pulse width)")     , QSL("def=0.7") );
   PRM_PARAMD(        cc,     0, QSL("&C")      , QSL("Constant base")                            , QSL("def=0") );
   PRM_PARAMD(       phi,     0, QSL("\\phi")   , QSL("Phase shift")                              , QSL("def=0") );
   PRM_PARAMD(      vmin,     0, QSL("vmin")    , QSL("Minimum value")                            , QSL("def=-1e100\nsep=col") );
   PRM_PARAMD(      vmax,     0, QSL("vmax")    , QSL("Maximum value")                            , QSL("def=1e100") );
   PRM_PARAMD(       dc0,     0, QSL("dc0")     , QSL("Duty start")                               , QSL("def=0") );
   PRM_PARAMD(        dc,     0, QSL("dc")      , QSL("Duty cycle")                               , QSL("def=0.5") );
   PRM_PARAMD(      tmin, efNRC, QSL("tmin")    , QSL("Minimum time")                             , QSL("def=-1e100") );
   PRM_PARAMD(      tmax, efNRC, QSL("tmax")    , QSL("Maximum time")                             , QSL("def=1e100") );

   // ----------- U chaos ----------------------
   PRM_SWITCH( use_u_ch, efNRC, QSL("use U chaos"), QSL("Use amplitude perturbations"), QSL("sep=tab\ntabname=Chaos") );
   PRM_PARAMD(  u_ch_v0, efNRC, QSL("vU min")     , QSL("Minimum value of amplitude factor"), QSL("def=0.5") );
   PRM_PARAMD(  u_ch_vm, efNRC, QSL("vU max")     , QSL("Maximum value of amplitude factor"), QSL("def=1.5") );
   PRM_PARAMD(  u_ch_t0, efNRC, QSL("tU min")     , QSL("Minimum time of change"), QSL("def=2\nmin=1e-20\nmax=1e20") );
   PRM_PARAMD(  u_ch_tm, efNRC, QSL("tU max")     , QSL("Maximum time of change"), QSL("def=10\nmin=1e-20\nmax=1e20") );
   PRM_INT(      seed_u, efNRC, QSL("U seed")     , QSL("Random Generator seed"), QSL("def=RND") );
   PRM_LIST( seedType_u, efNRC, QSL("U seed type"), QSL("When to seed"), QSL("def=3\nenum=SeedType") );
   PRM_SWITCH( addBaseSeed_u, efNRC, QSL("U add base seed"), QSL("Add seed from base(model)"), QSL("def=1"));
   PRM_SWITCH( addHash_u, efNRC, QSL("U add hash"), QSL("Add hash(fullname) to seed"), QSL("def=1"));

   // ----------- Phi chaos ----------------------
   PRM_SWITCH( use_f_ch,efNRC, QSL("use \\phi chaos"), QSL("Use phase perturbations"), QSL("sep=col") );
   PRM_PARAMD( f_ch_v0, efNRC, QSL("vF min")       , QSL("Minimum value of phase factor"), QSL("def=-0.2") );
   PRM_PARAMD( f_ch_vm, efNRC, QSL("vF max")       , QSL("Maximum value of phase factor"), QSL("def=0.2") );
   PRM_PARAMD( f_ch_t0, efNRC, QSL("tF min")       , QSL("Minimum time of change"), QSL("def=2\nmin=1e-20\nmax=1e20") );
   PRM_PARAMD( f_ch_tm, efNRC, QSL("rF max")       , QSL("Maximum time of change"), QSL("def=10\nmin=1e-20\nmax=1e20") );
   PRM_INT( seed_p, efNRC,     QSL("F seed")       , QSL("Random Generator seed"), QSL("def=RND") );
   PRM_LIST( seedType_p, efNRC,QSL("F seed type")  , QSL("When to seed"), QSL("def=3\nenum=SeedType") );
   PRM_SWITCH( addBaseSeed_p, efNRC, QSL("F add base to seed"), QSL("Add seed from base(model)"), QSL("def=1") );
   PRM_SWITCH( addHash_p, efNRC, QSL("F add hash"), QSL("Add hash(fullname) to seed"), QSL("def=1"));

   // inners
   PRM_DOUBLE( pha,   efInner, QSL("pha"),   QSL("current phase: 2\\pi t=1"), QES  );
   PRM_DOUBLE( pha_0, efInner, QSL("pha_0"), QSL("current phase in [0;1) range"), QES  );
   PRM_DOUBLE( u2,    efInner, QSL("u2"),    QSL("u^2"), QES  );
   PRM_DOUBLE( cv,    efInner, QSL("cv"),    QSL("Complement to v"), QES  );
   PRM_DOUBLE( T_0,   efInner, QSL("T_0"),   QSL("Period"), QSL("def=1")  );
   PRM_DOUBLE( T_1,   efInner, QSL("T_1"),   QSL("dc part of period"), QSL("def=1")  );

   // NO inputs

   /** chaos current values */
   double u_ch_vs = 1, u_ch_ve = 1, u_ch_ts = 1, u_ch_te = 1, u_ch_k = 1,
          f_ch_vs = 1, f_ch_ve = 1, f_ch_ts = 1, f_ch_te = 1, f_ch_k = 1;
   int sseed_u = 1, sseed_p = 1, bseed_u =1 , bseed_p = 1,
       hseed_u = 0, hseed_p = 0,
       eff_seedType_u = 0, eff_seedType_p = 0;


   /** generator structures */
   RandGenerator rng_u, rng_p;
   /** triggers */
   int was_pulse = 0;
   /** values to store some model vars */
   double tt = 1;

   Q_CLASSINFO( "nameHintBase",  "so_" );
   DCL_DEFAULT_STATIC;
};

