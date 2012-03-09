/***************************************************************************
                          tfourier.h  -  description
                             -------------------
    begin                : Sat Sep 9 2000
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

#ifndef TFOURIER_H
#define TFOURIER_H

#include <tmiso.h>


#define CLASS_ID_TFourier 2010 

/**performs Fourier analysis
  *@author atu
  */

class TFourier : public TMiso  {
 public:
   /** constructor */
   explicit TFourier( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFourier();
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
   /** reimplemented from TMiso to alloc buffers */
   virtual int preRun( int run_tp, int an, int anx, int any, double adt );
   /** reimplemented from TMiso to free buffers */
   virtual int postRun( int good );
   /** reimplemented from TMiso to init summators  */
   virtual int startLoop( int acnx, int acny );
   /** reimplemented from TMiso to complete analysis and fill out arrays */
   virtual int endLoop(void);
   /** main computation function */
   virtual double f( const double *u, double t );
 protected:
   void initVars(void);
 protected:
   /** period parameters  */
   double tp, t0, t1, omega;
   /** output values */
   double a0, a1, b1, ampl1, phi, ampl, qpow, qpow1, s_x2;
   /** N of garm, flag for optim., integer representation of period values */
   int ng, useOptim, useFill, nper, np, nn, n_st, n_en, 
       ii, out_a, out_b, out_am, out_om;
   /** arrays for summators */
   double *aa, *bb, *am;
   /** name of array to and from fill */
   char x_oname[MAX_NAMELEN], y_oname[MAX_NAMELEN], 
        a_oname[MAX_NAMELEN], b_oname[MAX_NAMELEN], 
	am_oname[MAX_NAMELEN], om_oname[MAX_NAMELEN];
   /** data descriptors -- with base class elements */ 
   static TDataInfo tfourier_d_i[50];
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** icon */
   static const char* icon[];
};

#endif


