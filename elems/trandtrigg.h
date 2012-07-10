/***************************************************************************
                          trandtrigg.h  -  description
                             -------------------
    begin                : Wed Aug 30 2000
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

#ifndef TRANDTRIGG_H
#define TRANDTRIGG_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <tmiso.h>

#define CLASS_ID_TRandTrigg 1018

/**random - deterministic trigger
  *@author atu
  */

class TRandTrigg : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TRandTrigg( TDataSet* aparent );
   /** empty destructor */
   virtual ~TRandTrigg();
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
   /** reimplemented from TMiso to drop random generator */
   virtual int postRun( int good );
   /** reimplemented from TMiso to set start values */
   virtual int startLoop( int acnx, int acny );
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /** reimplemented from TMiso to init random generator */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
   /** Probability of spontatious flip  */
   PRM_DOUBLE( prob, 0 ); 
   /** Forced every */
   PRM_INT( nforce, 0 /* ??? */ );
   /* flags */
   PRM_SWITCH( useLevel, efNoRunChange );
   PRM_SWITCH( useForce, efNoRunChange );
   PRM_SWITCH( useLock, efNoRunChange );
   PRM_SWITCH( useZero, efNoRunChange );
   /** seed value for generator */
   PRM_INT( seed, efNoRunChange );
   /** when seed generator: 0 - every run 1- 1d loop .. 3-by model */
   PRM_LIST( seedType, efNoRunChange );
   /** flag: add base seed to element seed */
   PRM_SWITCH( addBaseSeed, efNoRunChange );
   int ns, currOut;
   /** old value */
   double u_old; 
   
   /** data descriptors -- with base class elements */ 
   static TDataInfo trandtrigg_d_i[22];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
   /** start value of seed */
   int sseed;
   /** effective value of seedType */
   int eff_seedType;
   /** copy valuse of base seed */
   int bseed;
   /** generator structure from GSL */
   gsl_rng *rng;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


