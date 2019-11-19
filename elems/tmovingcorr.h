#pragma once
/***************************************************************************
     tmovingcorr.h - Moving correlation analysis declaration
                             -------------------
    begin                : 2016.08.02
    copyright            : (C) 2016-2019 by atu
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

#include <array>

#include <circbuf.h>
#include <tmiso.h>

/** Moving correlation analyser
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
   /** deallocates buf */
   virtual int do_postRun( int good ) override;
   /** reimplemented from TMiso to reset arrays */
   virtual int miso_startLoop( long acnx, long acny ) override;
   /* ============= function members ================== */
   /** resets collected and computed data except a, b */
   void reset_data();
   /** calculate all output values */
   int calc();

   /* ============= data members ================== */
   PRM_PARAMD(      tw,   efNRC, QSL("tw")      , QSL("Window time")                , QSL("def=1\nmin=1e-6") );
   PRM_DOUBLE(     slp,   efNRC, QSL("Sleep")   , QSL("Part of tw to mute outputs") , QSL("def=0.5\nmin=0") );
   PRM_SWITCH(  diff_x,   efNRC, QSL("diff x")  , QSL("Calc dx/dt")                 , QSL("sep=col") );
   PRM_SWITCH(  diff_y,   efNRC, QSL("diff y")  , QSL("Calc dy/dt")                 , QES );
   PRM_INT(         ok, efInner, QSL("ok")      , QSL("ok state")                   , QES );
   /** collectors, out values */
   PRM_DOUBLE(     s_x, efInner, QSL("s_x")     , QSL("sum(x)")                     , QES );
   PRM_DOUBLE(    s_x2, efInner, QSL("s_x2")    , QSL("sum(x^2)")                   , QES );
   PRM_DOUBLE(     s_y, efInner, QSL("s_y")     , QSL("sum(y)")                     , QES );
   PRM_DOUBLE(    s_y2, efInner, QSL("s_y2")    , QSL("sum(x^2)")                   , QES );
   PRM_DOUBLE(    s_xy, efInner, QSL("s_xy")    , QSL("sum(x*y)")                   , QES );
   PRM_DOUBLE(       a, efInner, QSL("a")       , QSL("a coefficient")              , QES );
   PRM_DOUBLE(       b, efInner, QSL("b")       , QSL("b coefficient")              , QES );
   PRM_DOUBLE(    corr, efInner, QSL("corr")    , QSL("coefficient of correlation") , QES );
   PRM_DOUBLE(     cov, efInner, QSL("cov")     , QSL("coefficient of covariation") , QES );
   PRM_DOUBLE(   dis_x, efInner, QSL("dis_x")   , QSL("x dispersion")               , QES );
   PRM_DOUBLE(   dis_y, efInner, QSL("dis_y")   , QSL("y dispersion")               , QES );
   PRM_DOUBLE( sigma_x, efInner, QSL("sigma_x") , QSL("sigma(x)")                   , QES );
   PRM_DOUBLE( sigma_y, efInner, QSL("sigma_y") , QSL("sigma(y)")                   , QES );
   PRM_DOUBLE(   ave_x, efInner, QSL("aver_x")  , QSL("aver(x)")                    , QES );
   PRM_DOUBLE(   ave_y, efInner, QSL("aver_y")  , QSL("aver(y)")                    , QES );
   PRM_DOUBLE(  ave_x2, efInner, QSL("aver_x2") , QSL("aver(x^2)")                  , QES );
   PRM_DOUBLE(  ave_y2, efInner, QSL("aver_y2") , QSL("aver(y^2)")                  , QES );
   PRM_DOUBLE(   n_sam, efInner, QSL("n_sam")   , QSL("Number of samples")          , QES );

   PRM_INPUT(     in_x,       0, QSL("in_{&x}") , QSL("First input")                , QSL("sep=block") );
   PRM_INPUT(     in_y,       0, QSL("in_{&y}") , QSL("Second input")               , QES );
   PRM_LOGICIN( in_ena,       0, QSL("en")      , QSL("Signal to enable")           , QSL("sep=col\ndef=1") );
   PRM_LOGICIN( in_rst,       0, QSL("rst")     , QSL("Signal to reset")            , QES );

   // ring arrays
   TCircBuf             a_x,  a_x2,  a_y,  a_y2 , a_xy;
   std::array<TCircBuf*, 5> bufs { &a_x, &a_x2, &a_y, &a_y2, &a_xy }; // is std::reference_wrapper better?

   double x_old {0.0}, y_old {0.0}, slpt {0.0};
   bool on_start { true };

   Q_CLASSINFO( "nameHintBase",  "mcorr_" );
   DCL_DEFAULT_STATIC;
};




