/***************************************************************************
                          trandtrigg.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

#ifndef TRANDTRIGG_H
#define TRANDTRIGG_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <tmiso.h>

static const char* const trandtrigg_list_seedType = 
     "Every run\n"          // 0
     "Start of 1d-loop\n"   // 1 
     "Start of 2d-loop\n"   // 2
     "As model"             // 3
;

/**random - deterministic trigger
  *@author atu
  */

class TRandTrigg : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRandTrigg);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TRandTrigg() override;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to init random generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to drop random generator */
   virtual int do_postRun( int good ) override;
   /** reimplemented from TMiso to set start values */
   virtual int do_startLoop( int acnx, int acny ) override;
   
   /** Probability of spontatious flip  */
   PRM_DOUBLE( prob, 0, "Probability", "Probability of spontatious flip",""); 
   /** Forced every */
   PRM_INT( nforce, 0, "Forced every N", "Forced every N changes", "" );
   /* flags */
   PRM_SWITCH( useLevel, efNRC, "Use Level to trigg", 
       "Use Level to trigger, not +- pulse", "sep=col" );
   PRM_SWITCH( useForce, efNRC, "u[1] - force", "Use u[1] to force flip", "" );
   PRM_SWITCH( useLock,  efNRC, "u[2] - lock", "Use u[2] to lock", "" );
   PRM_SWITCH( useZero,  efNRC, "0 = neg. output", 
       "Use 0 as negative output, not -1", "");
   /** seed value for generator */
   PRM_INT( seed, efNRC, "Seed", "Random generator seed", "sep=col" );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNRC, "Seed at", "Specifies, when to seed", "", 
       trandtrigg_list_seedType );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNRC, "add Base to Seed", 
       "add Base (model) value to Seed", "" );

   int ns, currOut;
   /** old value */
   double u_old; 
   
   /** start value of seed */
   int sseed;
   /** effective value of seedType */
   int eff_seedType;
   /** copy valuse of base seed */
   int bseed;
   /** generator structure from GSL */
   gsl_rng *rng;
   
   DCL_DEFAULT_STATIC;
};

#endif


