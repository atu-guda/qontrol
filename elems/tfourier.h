/***************************************************************************
                          tfourier.h  -  description
                             -------------------
    begin                : Sat Sep 9 2000
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

#ifndef TFOURIER_H
#define TFOURIER_H

#include <tmiso.h>


#define CLASS_ID_TFourier 2010 

/**performs Fourier analysis
  *@author atu
  */

class TFourier : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TFourier( TDataSet *aparent );
   /** empty destructor */
   virtual ~TFourier();
   /** creator */
   static TDataSet* create( TDataSet *apar );
   /** class id */
   virtual const TClassInfo* getClassInfo(void) const;
   /** return ptr to static class_info, static version */
   static const TClassInfo* getStaticClassInfo(void)
      { return &class_info; };
   /** returns help string */
   virtual const char* getHelp(void) const;
   /** reimplemented from TMiso to init summators  */
   virtual int startLoop( int acnx, int acny );
   /** reimplemented from TMiso to complete analysis and fill out arrays */
   virtual int endLoop(void);
 protected:
   /** main computation function */
   virtual double f( double t );
   void initVars(void);
   /** reimplemented from TMiso to alloc buffers */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt );
 protected:
   /** period parameters  */
   PRM_DOUBLE1( omega, efNoRunChange, "\\omega", "base frequency", "min=1e-3" );
   PRM_DOUBLE1( t0, efNoRunChange, "t0", "Start time", "" );
   PRM_DOUBLE1( t1, efNoRunChange, "t1", "Stop  time", "" );
   PRM_INT1( ng, efNoRunChange, "N of Harm", "Number of harmonics to analize", "" );
   PRM_SWITCH1( useOptim, efNoRunChange, "Optimize", "Optimize t1 to  better FT", "" );
   PRM_SWITCH1( useFill, efNoRunChange, "Fill cmp", "Fill compare array:", "" );
   PRM_INT1( np, efNoRunChange, "np", "np", "sep=col" );
   PRM_INT1( nn, efNoRunChange, "nn", "nn", "" );
   PRM_INT1( out_a, efNoRunChange, "out_a", "start index for a[i]", "min=0\nmax=1000" );
   PRM_INT1( out_b, efNoRunChange, "out_b", "start index for a[i]", "min=0\nmax=1000" );
   PRM_INT1( out_am, efNoRunChange, "out_am", "start index for A[i]", "min=0\nmax=1000" );
   PRM_INT1( out_om, efNoRunChange, "out_om", "start index for omega[i]", "min=0\nmax=1000" );
   /** name of array to and from fill */
   PRM_STRING1( x_oname, efNoRunChange, "x[] in ", "in x[] array name", "sep=col" );
   PRM_STRING1( y_oname, efNoRunChange, "y[] out", "out y[] array name", "" );
   PRM_STRING1( a_oname, efNoRunChange, "a[] out", "output array for a[i]", "" );
   PRM_STRING1( b_oname, efNoRunChange, "b[] out", "output array for b[i]", "" );
   PRM_STRING1( am_oname, efNoRunChange, "A[] out", "output array for A[i]", "" );
   PRM_STRING1( om_oname, efNoRunChange, "ome[] out", "output array for omega[i]", "" );
   /** output values */
   PRM_DOUBLE1( tp, efInner, "period", "base period for given frequency", "" );
   PRM_DOUBLE1( a0, efInner, "a0", "constant part", "" );
   PRM_DOUBLE1( a1, efInner, "a1", "part at cos(omega*t)", "" );
   PRM_DOUBLE1( b1, efInner, "b1", "part at sin(omega*t)", "" );
   PRM_DOUBLE1( ampl1, efInner, "ampl1", "Amplitude of first harmonic", "" );
   PRM_DOUBLE1( phi, efInner, "phi", "Phase of first harmonic", "" );
   PRM_DOUBLE1( ampl, efInner, "ampl", "Amplitude of all calculated harmonics?", "" );
   PRM_DOUBLE1( qpow, efInner, "qpow", "power criterion?", "" );
   PRM_DOUBLE1( qpow1, efInner, "qpow1", "power criterion1?", "" );
   PRM_DOUBLE1( s_x2, efInner, "s_x2", "Input power", "" );
   int nper, n_st, n_en, ii;
   /** arrays for summators */
   std::vector<double> aa, bb, am;
   /** class decription */
   static TClassInfo class_info;
   /** help str */
   static const char* helpstr;
   /** autoregister */
   static int registered;
   static int reg();
};

#endif


