/***************************************************************************
                          tcorralalysis.h  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by atu
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

#ifndef TCORRALALYSIS_H
#define TCORRALALYSIS_H

#include <tmiso.h>

#define CLASS_ID_TCorrAnalysis 2000

/**correlation analisator
  *@author atu
  */

class TCorrAnalysis : public TMiso  {
 public:
   /** constructor */
   explicit TCorrAnalysis( TDataSet* aparent );
   /** empty destructor */
   virtual ~TCorrAnalysis();
   /** creator */
   static TDataSet* create( TDataSet *apar );
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
   /** reimplemented from TMiso to set counters */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to reset summators */
   virtual int startLoop( int acnx, int acny );
   /** reimplemented from TMiso to fill out arrays */
   virtual int endLoop(void);
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   /* ============= function members ================== */
   /** resets collocted and computed data */
   void reset_data();
   /** calculate all output values */
   int calc();
   /** puts calculated values to model common vars array */
   void putModelVars();
   /** retrives data from model arrays */
   int getDataFromArrays();

   /* ============= data members ================== */
   /** type of data collection, flags */
   int type, useCalc, useReset, useFill;
   /** time start / stop values */
   double t0, t1;
   /** Names of input arrays if type=UseArrays */
   char x_in[MAX_NAMELEN], y_in[MAX_NAMELEN];
   /** indexes of output values to be stored to model */
   int out_a, out_b, out_corr, out_ok;
   /** names of output arrays */
   char x_oname[MAX_NAMELEN], y_oname[MAX_NAMELEN], c_oname[MAX_NAMELEN];
   /** main output source */
   int mainOutput;
   /** current loop # and number of collected data,  ok state - calculated */
   int ii, n, ok, nc;
   /** collectors, out values */
   double s_x, s_x2, s_y, s_y2, s_xy, a, b, corr, cov, 
          dis_x, dis_y, sigma_x, sigma_y, ave_x, ave_y, ave_x2, ave_y2,
	  cmp_ms, cmp_max, cmp_min, cmp_ampl, cmp_tmin, cmp_tmax;
   /** pointerrs for output source */
   const double *out_source[20];
   /** data descriptors -- with base class elements */ 
   static TDataInfo tcorranalysis_d_i[64];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};



#endif

