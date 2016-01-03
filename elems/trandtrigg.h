/***************************************************************************
                          trandtrigg.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
    copyright            : (C) 2000-2016 by atu
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

#include <tmiso.h>
#include "rand_gen.h"

/**random - deterministic trigger
  *@author atu
  */

class TRandTrigg : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TRandTrigg);
   DCL_CREATE;
   DCL_STD_INF;

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
   /** reimplemented from TMiso to init random generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to set start values */
   virtual int do_startLoop( int acnx, int acny ) override;

   /** Probability of spontatious flip  */
   PRM_DOUBLE( prob, 0, "Probability", "Probability of spontatious flip","def=0.5");
   /** Forced every */
   PRM_INT( nforce, 0, "Forced every N", "Forced every N changes", "def=2" );
   /* flags */
   PRM_SWITCH( useLevel, efNRC, "Use Level to trigg",
       "Use Level to trigger, not +- pulse", "sep=col" );
   PRM_SWITCH( useForce, efNRC, "use Force", "Use in_force to force flip", "" );
   PRM_SWITCH( useLock,  efNRC, "use Lock", "Use in_lock to lock", "" );
   PRM_SWITCH( useZero,  efNRC, "0 = neg. output",
       "Use 0 as negative output, not -1", "");
   /** seed value for generator */
   PRM_INT( seed, efNRC, "Seed", "Random generator seed", "sep=col\ndef=RND" );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNRC, "Seed at", "Specifies, when to seed", "def=3\nenum=SeedType" );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNRC, "add Base to Seed",
       "add Base (model) value to Seed", "def=1" );

   PRM_INPUT( in_u, 0, "input ", "Main input",  "sep=block" );
   PRM_INPUT( in_force, 0, "Force in", "Signal : force trigg", "sep=col" );
   PRM_INPUT( in_lock, 0, "Lock in", "Signal: lock",  "sep=col" );


   int ns = 0, currOut = 0;
   /** old value */
   double u_old = 0;

   /** start value of seed */
   int sseed = 1;
   /** effective value of seedType */
   int eff_seedType = 0;
   /** copy valuse of base seed */
   int bseed = 1;
   /** generator */
   RandGenerator rng;

   Q_CLASSINFO( "nameHintBase",  "rtrig_" );
   DCL_DEFAULT_STATIC;
};

#endif


