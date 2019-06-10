#pragma once
/*
 * =====================================================================================
 *
 *       Filename:  tiir7.h
 *
 *    Description:  IIR filter with 7 stored values declarations
 *
 *        Created:  2019-06-10 22:53:09
 *        License:  GPLv2+
 *         Author:  Anton Guda (atu), atu@nmetau.edu.ua
 *
 * =====================================================================================
 */

#include "tmiso.h"

class TIIR7 : public TMiso  {
  Q_OBJECT
 public:
   static constexpr unsigned n_val = 7;
   DCL_CTOR(TIIR7);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( a,  0, "&a", "Common factor", "sep=col\ndef=1" );
   PRM_PARAMD( b,  0, "&b", "Shift", "sep=col" );


   PRM_DOUBLE( u_min, 0, "u_{min}", "Input value lower limit", "def=-1e300\nsep=block" );
   PRM_DOUBLE( u_max, 0, "u_{max}", "Input value upper limit", "def=1e300" );
   PRM_DOUBLE( x_min, 0, "x_{min}", "Input value lower limit", "def=-1e300\nsep=col" );
   PRM_DOUBLE( x_max, 0, "x_{max}", "Input value upper limit", "def=1e300" );

   PRM_INPUT(      u, 0,   "&u",                "Input",  "sep=block" );
   PRM_LOGICIN( hold, 0, "hold",  "Hold current output",  "sep=col" );


   PRM_DOUBLE_ARR( ka,    0, "ka", "Coeffs with x (out)", "N=7\nsep=tab\ntabname=Arrays" );
   PRM_DOUBLE_ARR( kb,    0, "kb", "Coeffs with u  (in)", "N=7\ndefs=0.7 0.3 0\nsep=col" );
   PRM_DOUBLE_ARR( uo,  efInner, "uo", "Input  values (0=now)",  "N=7" );
   PRM_DOUBLE_ARR( xo,  efInner, "xo", "Output values (0=now)",  "N=7" );

   bool on_start = true;

   Q_CLASSINFO( "nameHintBase",  "iir_" );
   DCL_DEFAULT_STATIC;
};



