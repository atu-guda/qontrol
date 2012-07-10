/***************************************************************************
                          tcorralalysis.h  -  description
                             -------------------
    begin                : Fri Sep 8 2000
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

#ifndef TCORRALALYSIS_H
#define TCORRALALYSIS_H

#include <tmiso.h>

#define CLASS_ID_TCorrAnalysis 2000

/**correlation analisator
  *@author atu
  */

class TCorrAnalysis : public TMiso  {
  Q_OBJECT
 public:
   /** constructor */
   explicit TCorrAnalysis( TDataSet* aparent );
   /** empty destructor */
   virtual ~TCorrAnalysis();
   /** creator */
   static TDataSet* create( TDataSet *apar );
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
   PRM_LIST1( type, efNoRunChange, "Type", "Collection type", "", "All\nt0<t<t1\nu2>0\narrays" );
   PRM_SWITCH1( useCalc,  efNoRunChange, "Calc on u[3]", "Use u3>0 as signal to calc", "" );
   PRM_SWITCH1( useReset, efNoRunChange, "Reset on u[3]", "Use u3<0 as signal to reset", "" );
   PRM_SWITCH1( useFill,  efNoRunChange, "Fill arrays", "Fill output array", "" );
   /** time start / stop values */
   PRM_DOUBLE1( t0, efNoRunChange, "t0", "Time to start on given type", "min=0" );
   PRM_DOUBLE1( t1, efNoRunChange, "t1", "Time to stop on given type", "min=0" );
   /** Names of input arrays if type=UseArrays */
   PRM_STRING1( x_in, efNoRunChange, "in x[]", "name of input x array", "" );
   PRM_STRING1( y_in, efNoRunChange, "in y[]", "name of input y array", "" );
   /** indexes of output values to be stored to model */
   PRM_INT1( out_a, efNoRunChange, "a idx", "Index to store 'a' value", "sep=col" );
   PRM_INT1( out_b, efNoRunChange, "b idx", "Index to store 'b' value", "" );
   PRM_INT1( out_corr, efNoRunChange, "corr idx", "Index to store 'corr' value", "" );
   PRM_INT1( out_ok, efNoRunChange, "ok idx", "Index to store 'ok' value", "" );
   /** names of output arrays */
   PRM_STRING1( x_oname, efNoRunChange, "out x name", "name of output x array", "sep=col" );
   PRM_STRING1( y_oname, efNoRunChange, "out y name", "name of output y array", "" );
   PRM_STRING1( c_oname, efNoRunChange, "compare name", "name of array to compare with", "" );
   /** main output source */
   PRM_LIST1( mainOutput, efNoRunChange, "Main outout", "What value will be main output", "",
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
   PRM_INT1( ii, efInner, "ii", "current loop number", "" );
   PRM_INT1( n, efInner, "n", "number of collected data", "" );
   PRM_INT1( ok, efInner, "ok", "ok state", "" );
   PRM_INT1( nc, efInner, "nc", "nc?", "" );
   /** collectors, out values */
   PRM_DOUBLE1( s_x, efInner, "s_x", "sum(x)", "" );
   PRM_DOUBLE1( s_x2, efInner, "s_x2", "sum(x^2)", "" );
   PRM_DOUBLE1( s_y, efInner, "s_y", "sum(y)", "" );
   PRM_DOUBLE1( s_y2, efInner, "s_y2", "sum(x^2)", "" );
   PRM_DOUBLE1( s_xy, efInner, "s_xy", "sum(x*y)", "" );
   PRM_DOUBLE1( a, efInner, "a", "a coefficient", "" );
   PRM_DOUBLE1( b, efInner, "b", "b coefficient", "" );
   PRM_DOUBLE1( corr, efInner, "corr", "coefficient of correlation", "" );
   PRM_DOUBLE1( cov, efInner, "cov", "coefficient of covariation", "" );
   PRM_DOUBLE1( dis_x, efInner, "dis_x", "x dispersion", "" );
   PRM_DOUBLE1( dis_y, efInner, "dis_y", "y dispersion", "" );
   PRM_DOUBLE1( sigma_x, efInner, "sigma_x", "sigma(x)", "" );
   PRM_DOUBLE1( sigma_y, efInner, "sigma_y", "sigma(y)", "" );
   PRM_DOUBLE1( ave_x, efInner, "aver_x", "aver(x)", "" );
   PRM_DOUBLE1( ave_y, efInner, "aver_y", "aver(y)", "" );
   PRM_DOUBLE1( ave_x2, efInner, "aver_x2", "aver(x^2)", "" );
   PRM_DOUBLE1( ave_y2, efInner, "aver_y2", "aver(y^2)", "" );
   PRM_DOUBLE1( cmp_ms, efInner, "cmp_ms", "comparison mean square", "" );
   PRM_DOUBLE1( cmp_max, efInner, "cmp_max", "max error", "" );
   PRM_DOUBLE1( cmp_min, efInner, "cmp_min", "min error", "" );
   PRM_DOUBLE1( cmp_ampl, efInner, "cmp_ampl", "error amplitude", "" );
   PRM_DOUBLE1( cmp_tmin, efInner, "cmp_tmin", "time(min error)", "" );
   PRM_DOUBLE1( cmp_tmax, efInner, "cmp_tmax", "time(max error)", "" );
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
   /** autoregister */
   static int registered;
   static int reg();
};



#endif

