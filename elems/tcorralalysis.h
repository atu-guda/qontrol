/***************************************************************************
                          tcorralalysis.h  -  description
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000-2015 by atu
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
   enum CallType { call_all = 0, call_time, call_u2 };
   Q_ENUMS(CallType);
   Q_CLASSINFO( "enum_CallType_0", "All" );     // call_all
   Q_CLASSINFO( "enum_CallType_1", "t0<t<t1" ); // call_time
   Q_CLASSINFO( "enum_CallType_2", "in_add>0" );// call_u2
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   /** reimplemented from TMiso to reset summators */
   virtual int do_startLoop( int acnx, int acny ) override;
   /* ============= function members ================== */
   /** resets collocted and computed data */
   void reset_data();
   /** calculate all output values */
   int calc();
   /** retrives data from model arrays */
   int getDataFromArrays();

   /* ============= data members ================== */
   /** type of data collection, flags */
   PRM_LIST( type, efNoRunChange, "Type", "Collection type", "enum=CallType" );
   PRM_SWITCH( useCalc,  efNoRunChange, "use Calc", "Use in_calc>0 as signal to calc", "" );
   PRM_SWITCH( useReset, efNoRunChange, "use Reset", "Use u_rst>0 as signal to reset", "" );
   /** time start / stop values */
   PRM_DOUBLE( t0, efNoRunChange, "t0", "Time to start on given type", "min=0\nsep=col" );
   PRM_DOUBLE( t1, efNoRunChange, "t1", "Time to stop on given type", "min=0\ndef=10000" );
   /** current loop # and number of collected data,  ok state - calculated */
   PRM_INT( ii, efInner, "ii", "current loop number", "" );
   PRM_INT( n, efInner, "n", "number of collected data", "" );
   PRM_INT( ok, efInner, "ok", "ok state", "" );
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

   PRM_INPUT( in_x, 0, "x input", "First input",  "sep=block" );
   PRM_INPUT( in_y, 0, "y input", "Second input", "" );
   PRM_INPUT( in_add, 0, "add input", "Signal to add",  "sep=col" );
   PRM_INPUT( in_rst, 0, "rst signal", "Reset signal", "" );
   PRM_INPUT( in_calc, 0, "calc signal", "Calc signal", "sep=col" );

   DCL_DEFAULT_STATIC;
};



#endif

