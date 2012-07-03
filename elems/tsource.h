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
   /** class id */
   virtual int getClassId(void) const ;
   /** return ptr to static class_info, nust be implemented in each class */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** return icon */
   virtual const char** getIcon(void) const;
   /** main computation function */
   virtual double f( const double *u, double t );
   /** will be called before any action -- good place for allocs */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented to delete rnds */
   virtual int postRun( int good );
   /** called before each inner param loop. Unused param - -1 */
   virtual int startLoop( int acnx, int acny );
 protected:
   /** type of input and misc flags */
   PRM_LIST( type, efNoRunChange );
   /** Amplitude */
   PRM_DOUBLE( uu, 0 );
   /** Frequency */
   PRM_DOUBLE( omega, 0 );
   /** constant shift */
   PRM_DOUBLE( cc, 0 );

   // ----------- U chaos ----------------------
   PRM_SWITCH( use_u_ch, efNoRunChange );
   PRM_DOUBLE(  u_ch_v0, efNoRunChange );
   PRM_DOUBLE(  u_ch_vm, efNoRunChange );
   PRM_DOUBLE(  u_ch_t0, efNoRunChange );
   PRM_DOUBLE(  u_ch_tm, efNoRunChange );
   PRM_INT(      seed_u, efNoRunChange );
   PRM_LIST( seedType_u, efNoRunChange );
   PRM_SWITCH( addBaseSeed_u, efNoRunChange );
   
   // ----------- Phi chaos ----------------------
   PRM_SWITCH( use_f_ch,efNoRunChange );
   PRM_DOUBLE( f_ch_v0, efNoRunChange );
   PRM_DOUBLE( f_ch_vm, efNoRunChange );
   PRM_DOUBLE( f_ch_t0, efNoRunChange );
   PRM_DOUBLE( f_ch_tm, efNoRunChange );
   PRM_INT( seed_p, efNoRunChange );
   PRM_LIST( seedType_p, efNoRunChange );
   PRM_SWITCH( addBaseSeed_p, efNoRunChange );
   
   // some unused params
   PRM_DOUBLE( phi_shift, efInner );
   PRM_SWITCH( use_noise, efInner ); // unused
   PRM_DOUBLE( noise, efInner ); // unused

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
   /** data descriptors -- with base class elements */
   static TDataInfo tsource_d_i[48];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   /** autoregister */
   static int registered;
   static int reg();
};

#endif
