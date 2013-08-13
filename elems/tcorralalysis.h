/***************************************************************************
                          tcorralalysis.h  -  description
                             -------------------
    begin                : Fri Sep 8 2000
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

#ifndef TCORRALALYSIS_H
#define TCORRALALYSIS_H

#include <tmiso.h>

/**correlation analisator
  *@author atu
  */

class TCorrAnalysis : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TCorrAnalysis);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to reset summators */
   virtual int do_startLoop( int acnx, int acny ) override;
   /** reimplemented from TMiso to fill out arrays */
   virtual int do_endLoop() override;
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
   PRM_LIST( type, efNoRunChange, "Type", "Collection type", "", "All\nt0<t<t1\nu2>0\narrays" );
   PRM_SWITCH( useCalc,  efNoRunChange, "Calc on u[3]", "Use u3>0 as signal to calc", "" );
   PRM_SWITCH( useReset, efNoRunChange, "Reset on u[3]", "Use u3<0 as signal to reset", "" );
   PRM_SWITCH( useFill,  efNoRunChange, "Fill arrays", "Fill output array", "" );
   /** time start / stop values */
   PRM_DOUBLE( t0, efNoRunChange, "t0", "Time to start on given type", "min=0" );
   PRM_DOUBLE( t1, efNoRunChange, "t1", "Time to stop on given type", "min=0" );
   /** Names of input arrays if type=UseArrays */
   PRM_STRING( x_in, efNoRunChange, "in x[]", "name of input x array", "" );
   PRM_STRING( y_in, efNoRunChange, "in y[]", "name of input y array", "" );
   /** indexes of output values to be stored to model */
   PRM_INT( out_a, efNoRunChange, "a idx", "Index to store 'a' value", "sep=col" );
   PRM_INT( out_b, efNoRunChange, "b idx", "Index to store 'b' value", "" );
   PRM_INT( out_corr, efNoRunChange, "corr idx", "Index to store 'corr' value", "" );
   PRM_INT( out_ok, efNoRunChange, "ok idx", "Index to store 'ok' value", "" );
   /** names of output arrays */
   PRM_STRING( x_oname, efNoRunChange, "out x name", "name of output x array", "sep=col" );
   PRM_STRING( y_oname, efNoRunChange, "out y name", "name of output y array", "" );
   PRM_STRING( c_oname, efNoRunChange, "compare name", "name of array to compare with", "" );
   /** main output source */
   PRM_LIST( mainOutput, efNoRunChange, "Main outout", "What value will be main output", "",
       "summ(x)\n"      //  0
       "summ(x^2)\n"    //  1
       "summ(y)\n"      //  2
       "summ(y^2)\n"    //  3 
       "summ(xy)\n"     //  4
       "a\n"            //  5
       "b\n"            //  6
       "corr\n"         //  7
       "cov\n"          //  8
       "disp(x)\n"      //  9
       "dips(y)\n"      // 10
       "sigma(x)\n"     // 11
       "sigma(y)\n"     // 12
       "aver(x)\n"      // 13
       "aver(y)\n"      // 14
       "aver(x^2)\n"    // 15
       "aver(y^2)"      // 16
   );
   /** current loop # and number of collected data,  ok state - calculated */
   PRM_INT( ii, efInner, "ii", "current loop number", "" );
   PRM_INT( n, efInner, "n", "number of collected data", "" );
   PRM_INT( ok, efInner, "ok", "ok state", "" );
   PRM_INT( nc, efInner, "nc", "nc?", "" );
   /** collectors, out values */
   PRM_DOUBLE( s_x, efInner, "s_x", "sum(x)", "" );
   PRM_DOUBLE( s_x2, efInner, "s_x2", "sum(x^2)", "" );
   PRM_DOUBLE( s_y, efInner, "s_y", "sum(y)", "" );
   PRM_DOUBLE( s_y2, efInner, "s_y2", "sum(x^2)", "" );
   PRM_DOUBLE( s_xy, efInner, "s_xy", "sum(x*y)", "" );
   PRM_DOUBLE( a, efInner, "a", "a coefficient", "" );
   PRM_DOUBLE( b, efInner, "b", "b coefficient", "" );
   PRM_DOUBLE( corr, efInner, "corr", "coefficient of correlation", "" );
   PRM_DOUBLE( cov, efInner, "cov", "coefficient of covariation", "" );
   PRM_DOUBLE( dis_x, efInner, "dis_x", "x dispersion", "" );
   PRM_DOUBLE( dis_y, efInner, "dis_y", "y dispersion", "" );
   PRM_DOUBLE( sigma_x, efInner, "sigma_x", "sigma(x)", "" );
   PRM_DOUBLE( sigma_y, efInner, "sigma_y", "sigma(y)", "" );
   PRM_DOUBLE( ave_x, efInner, "aver_x", "aver(x)", "" );
   PRM_DOUBLE( ave_y, efInner, "aver_y", "aver(y)", "" );
   PRM_DOUBLE( ave_x2, efInner, "aver_x2", "aver(x^2)", "" );
   PRM_DOUBLE( ave_y2, efInner, "aver_y2", "aver(y^2)", "" );
   PRM_DOUBLE( cmp_ms, efInner, "cmp_ms", "comparison mean square", "" );
   PRM_DOUBLE( cmp_max, efInner, "cmp_max", "max error", "" );
   PRM_DOUBLE( cmp_min, efInner, "cmp_min", "min error", "" );
   PRM_DOUBLE( cmp_ampl, efInner, "cmp_ampl", "error amplitude", "" );
   PRM_DOUBLE( cmp_tmin, efInner, "cmp_tmin", "time(min error)", "" );
   PRM_DOUBLE( cmp_tmax, efInner, "cmp_tmax", "time(max error)", "" );
   /** pointerrs for output source */
   const double *out_source[20];
   DCL_DEFAULT_STATIC;
};



#endif

