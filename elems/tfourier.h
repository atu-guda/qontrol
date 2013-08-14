/***************************************************************************
                          tfourier.h  -  description
                             -------------------
    begin                : Sat Sep 9 2000
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

#ifndef TFOURIER_H
#define TFOURIER_H

#include <tmiso.h>


/**performs Fourier analysis
  *@author atu
  */

class TFourier : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFourier);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   void initVars(void);
   /** reimplemented from TMiso to alloc buffers */
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   /** reimplemented from TMiso to init summators  */
   virtual int do_startLoop( int acnx, int acny ) override;
   /** reimplemented from TMiso to complete analysis and fill out arrays */
   virtual int do_endLoop(void) override;
 protected:
   /** period parameters  */
   PRM_DOUBLE( omega, efNoRunChange, "\\omega", "base frequency", "min=1e-3\ndef=1" );
   PRM_DOUBLE( t0, efNoRunChange, "t0", "Start time", "" );
   PRM_DOUBLE( t1, efNoRunChange, "t1", "Stop  time", "def=1e6" );
   PRM_INT( ng, efNoRunChange, "N of Harm", "Number of harmonics to analize", "def=3\nmin=1" );
   PRM_SWITCH( useOptim, efNoRunChange, "Optimize", "Optimize t1 to  better FT", "def=1" );
   PRM_SWITCH( useFill, efNoRunChange, "Fill cmp", "Fill compare array:", "" );
   PRM_INT( np, efNoRunChange, "np", "np", "sep=col\ndef=1\nmin=1" );
   PRM_INT( nn, efNoRunChange, "nn", "nn", "def=1" );
   PRM_INT( out_a, efNoRunChange, "out_a", "start index for a[i]", "min=-1\nmax=1000\ndef=-1" );
   PRM_INT( out_b, efNoRunChange, "out_b", "start index for a[i]", "min=-1\nmax=1000\ndef=-1" );
   PRM_INT( out_am, efNoRunChange, "out_am", "start index for A[i]", "min=-1\nmax=1000\ndef=-1" );
   PRM_INT( out_om, efNoRunChange, "out_om", "start index for omega[i]", "min=-1\nmax=1000\ndef=-1" );
   /** name of array to and from fill */
   PRM_STRING( x_oname, efNoRunChange, "x[] in ", "in x[] array name", "sep=col" );
   PRM_STRING( y_oname, efNoRunChange, "y[] out", "out y[] array name", "" );
   PRM_STRING( a_oname, efNoRunChange, "a[] out", "output array for a[i]", "" );
   PRM_STRING( b_oname, efNoRunChange, "b[] out", "output array for b[i]", "" );
   PRM_STRING( am_oname, efNoRunChange, "A[] out", "output array for A[i]", "" );
   PRM_STRING( om_oname, efNoRunChange, "ome[] out", "output array for omega[i]", "" );
   /** output values */
   PRM_DOUBLE( tp, efInner, "period", "base period for given frequency", "" );
   PRM_DOUBLE( a0, efInner, "a0", "constant part", "" );
   PRM_DOUBLE( a1, efInner, "a1", "part at cos(omega*t)", "" );
   PRM_DOUBLE( b1, efInner, "b1", "part at sin(omega*t)", "" );
   PRM_DOUBLE( ampl1, efInner, "ampl1", "Amplitude of first harmonic", "" );
   PRM_DOUBLE( phi, efInner, "phi", "Phase of first harmonic", "" );
   PRM_DOUBLE( ampl, efInner, "ampl", "Amplitude of all calculated harmonics?", "" );
   PRM_DOUBLE( qpow, efInner, "qpow", "power criterion?", "" );
   PRM_DOUBLE( qpow1, efInner, "qpow1", "power criterion1?", "" );
   PRM_DOUBLE( s_x2, efInner, "s_x2", "Input power", "" );
   int nper, n_st, n_en, ii;
   /** arrays for summators */
   dvector aa, bb, am;
   DCL_DEFAULT_STATIC;
};

#endif


