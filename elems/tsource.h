/***************************************************************************
                          tsource.h  -  description
                             -------------------
    begin                : Thu Aug 24 2000
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

#ifndef TSOURCE_H
#define TSOURCE_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "tmiso.h"

#define CLASS_ID_TSource 1002

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
   /** constructor */
   explicit TSource( TDataSet* aparent );
   /** empty destructor */
   virtual ~TSource();
   /** creator */
   static TDataSet* create( TDataSet* apar );
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** main computation function */
   virtual double f( double t );
   /** reimplemented to delete rnds */
   virtual int postRun( int good );
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** will be called before any action -- good place for allocs */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** type of input and misc flags */
   PRM_LIST1( type, efNRC, "Type", "Source type", "", tsource_list_type );
   /** Amplitude */
   PRM_DOUBLE1( uu, 0, "Amplitude", "Amplitude of signal (or pulse max)", "" );
   /** Frequency */
   PRM_DOUBLE1( omega, 0, "Frequency", "Frequency of source (or pulse width)", "" );
   /** constant shift */
   PRM_DOUBLE1( cc, 0, "C", "Constant base", "" );

   // ----------- U chaos ----------------------
   PRM_SWITCH1( use_u_ch, efNRC, "use U chaos", "Use amplitude perturbations", "sep=col" );
   PRM_DOUBLE1(  u_ch_v0, efNRC, "vU min"     , "Minimum value of amplitule factor", "" );
   PRM_DOUBLE1(  u_ch_vm, efNRC, "vU max"     , "Maximum value of amplitule factor", "" );
   PRM_DOUBLE1(  u_ch_t0, efNRC, "tU min"     , "Minimum time of change", "min=1e-20\nmax=1e20" );
   PRM_DOUBLE1(  u_ch_tm, efNRC, "tU max"     , "Maximum time of change", "min=1e-20\nmax=1e20" );
   PRM_INT1(      seed_u, efNRC, "U seed"     , "Random Generator seed", "" );
   PRM_LIST1( seedType_u, efNRC, "U seed type", "When to seed", "", tsource_list_seedType );
   PRM_SWITCH1( addBaseSeed_u, efNRC, "U add base seed", "Add seed from base(model)", "");
   
   // ----------- Phi chaos ----------------------
   PRM_SWITCH1( use_f_ch,efNRC, "use Phi chaos", "Use phase perturbations", "sep=col" );
   PRM_DOUBLE1( f_ch_v0, efNRC, "vF min"       , "Minimum value of phase factor", "" );
   PRM_DOUBLE1( f_ch_vm, efNRC, "vF max"       , "Maximum value of phase factor", "" );
   PRM_DOUBLE1( f_ch_t0, efNRC, "tF min"       , "Minimum time of change", "min=1e-20\nmax=1e20" );
   PRM_DOUBLE1( f_ch_tm, efNRC, "rF max"       , "Maximum time of change", "min=1e-20\nmax=1e20" );
   PRM_INT1( seed_p, efNRC,     "F seed"       , "Random Generator seed", "" );
   PRM_LIST1( seedType_p, efNRC,"F seed type"  , "When to seed", "", tsource_list_seedType);
   PRM_SWITCH1( addBaseSeed_p, efNRC, "F add base to seed", "Add seed from base(model)", "" );
   
   /** chaos current values */
   double u_ch_vs, u_ch_ve, u_ch_ts, u_ch_te, u_ch_k,
          f_ch_vs, f_ch_ve, f_ch_ts, f_ch_te, f_ch_k;
   int sseed_u, sseed_p, bseed_u, bseed_p,
       eff_seedType_u, eff_seedType_p;
   
   
   /** generator structures from GSL */
   gsl_rng *rng_u, *rng_p;
   /** triggers */
   int was_pulse;
   /** values to store some model vars */	  
   double tt;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif
