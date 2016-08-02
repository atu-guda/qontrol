#pragma once
/***************************************************************************
     tmovingcorr.h - Moving correlation analisys declaration
                             -------------------
    begin                : 2016.08.02
    copyright            : (C) 2016-2016 by atu
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


#include <tmiso.h>

/** Moving correlation analisator
  *@author atu
  */

class TMovingCorr : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TMovingCorr);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   /** reimplemented from TMiso to reset summators */
   virtual int miso_startLoop( long acnx, long acny ) override;
   /* ============= function members ================== */
   /** resets collected and computed data */
   void reset_data();
   void reset_counters(); // reset only counters: to call at reset()
   /** calculate all output values */
   int calc();

   /* ============= data members ================== */
   PRM_DOUBLE( tw, efNRC, "tm", "Window time", "def=1\nmin=1e-6" );
   // PRM_SWITCH( useReset, efNRC, "use &Reset", "Use in_rst as signal to reset", "" );
   /** current loop # and number of collected data,  ok state - calculated */
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

   PRM_INPUT(     in_x,  0, "in_{&x}", "First input",  "sep=block" );
   PRM_INPUT(     in_y,  0, "in_{&y}", "Second input", "sep=col" );
   PRM_LOGICIN( in_rst,  0, "rst", "Reset signal", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "mcorr_" );
   DCL_DEFAULT_STATIC;
};




