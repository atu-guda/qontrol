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
   /** class name - for check & human purpose */
   virtual const char* getClassName(void) const;
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
   int type, use_noise, use_u_ch, use_f_ch;
   /** coefficients on input and shift */
   double uu, omega, cc, phi_shift, noise;
   /** chaos coeffs */
   double u_ch_v0, u_ch_vm, u_ch_t0, u_ch_tm,
          f_ch_v0, f_ch_vm, f_ch_t0, f_ch_tm;
   /** triggers */
   int was_pulse;
   /** chaos current values */
   double u_ch_vs, u_ch_ve, u_ch_ts, u_ch_te, u_ch_k,
          f_ch_vs, f_ch_ve, f_ch_ts, f_ch_te, f_ch_k;
   /** chaos random generators params */
   int seed_u, seed_p, sseed_u, sseed_p, bseed_u, bseed_p,
       seedType_u, seedType_p, eff_seedType_u, eff_seedType_p, 
       addBaseSeed_u, addBaseSeed_p;
   /** generator structures from GSL */
   gsl_rng *rng_u, *rng_p;
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
};

#endif
