#pragma once
/***************************************************************************
                          tfunctrans.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2019 by atu
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

 /**trigonometric, exponetial and wavelet functions
  *@author atu
  */

class TFuncTrans : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFuncTrans);
   DCL_CREATE;
   DCL_STD_INF;

   enum FuncType {
     ft_sin = 0, ft_signSin, ft_tanh, ft_atan2, ft_exp, ft_expM2, ft_wave, ft_mhat,
     ft_ln, ft_yExp, ft_yExp2, ft_sin2, ft_cos, ft_cos2, ft_tan, ft_exp1Msin,
     ft_1Mexp1Msin2, ft_expM2d, ft_divMod,
     ft_expMod, ft_sinh, ft_cosh, ft_expm1
   };
   Q_ENUM(FuncType);
   Q_CLASSINFO( "enum_FuncType_0",  "ae*sin(be*y+c)+g"                      ); //  ft_sin
   Q_CLASSINFO( "enum_FuncType_1",  "ae*sign(sin(be*y+c)+d)+g"              ); //  ft_signSin
   Q_CLASSINFO( "enum_FuncType_2",  "ae*tanh(be*y+c)+g"                     ); //  ft_tanh
   Q_CLASSINFO( "enum_FuncType_3",  "ae*atan2(u0*be,u1)+g"                  ); //  ft_atan2
   Q_CLASSINFO( "enum_FuncType_4",  "ae*exp(be*y)+g"                        ); //  ft_exp
   Q_CLASSINFO( "enum_FuncType_5",  "ae*exp(-be*y^2+c)+g"                   ); //  ft_expM2
   Q_CLASSINFO( "enum_FuncType_6",  "ae*Wave(be*y+c)+g"                     ); //  ft_wave
   Q_CLASSINFO( "enum_FuncType_7",  "ae*Mhat(be*y+c)+g"                     ); //  ft_mhat
   Q_CLASSINFO( "enum_FuncType_8",  "ae*ln(be*y+c)+g"                       ); //  ft_ln
   Q_CLASSINFO( "enum_FuncType_9",  "ae*y*exp(-(be*y+c))+g"                 ); //  ft_yExp
   Q_CLASSINFO( "enum_FuncType_10", "ae*y*exp(-be*y^2+c)+g"                 ); //  ft_yExp2
   Q_CLASSINFO( "enum_FuncType_11", "ae*sin^2(be*y+c)+g"                    ); //  ft_sin2
   Q_CLASSINFO( "enum_FuncType_12", "ae*cos(be*y+c)+g"                      ); //  ft_cos
   Q_CLASSINFO( "enum_FuncType_13", "ae*cos^2(be*y+c)+g"                    ); //  ft_cos2
   Q_CLASSINFO( "enum_FuncType_14", "ae*tan(b*y+c)+g"                       ); //  ft_tan
   Q_CLASSINFO( "enum_FuncType_15", "ae*exp(-be*y*y)*(1-c*sin^2(dy))+g"     ); //  ft_exp1Msin
   Q_CLASSINFO( "enum_FuncType_16", "ae*(1-exp(-b*y*y))*(1-c*sin^2(dy))+g"  ); //  ft_1Mexp1Msin2
   Q_CLASSINFO( "enum_FuncType_17", "ae*exp(-(y/b)^2+c)+g"                  ); //  ft_expM2d
   Q_CLASSINFO( "enum_FuncType_18", "ae/((|y|/be+1+c)+g"                    ); //  ft_divMod
   Q_CLASSINFO( "enum_FuncType_19", "ae*exp(-|y|/be+c)+g"                   ); //  ft_expMod
   Q_CLASSINFO( "enum_FuncType_20", "ae*sinh(be*y)+g"                       ); //  ft_sinh
   Q_CLASSINFO( "enum_FuncType_21", "ae*cosh(be*y)+g"                       ); //  ft_cosh
   Q_CLASSINFO( "enum_FuncType_22", "ae*(exp(be*y)-1)+g"                    ); //  ft_expm1
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   /** type of function */
   PRM_LIST( type, 0, "&Type", "Function type", "enum=FuncType" );
   /** coefficients and shift */
   PRM_PARAMD( a,  0, "&a", "Parameter a", "sep=col\ndef=1" );
   PRM_PARAMD( b,  0, "&b", "Parameter b", "def=1" );
   PRM_PARAMD( c,  0, "&c", "Parameter c", "" );
   PRM_PARAMD( d,  0, "&d", "Parameter d", "" );
   PRM_PARAMD( e,  0, "&e", "Parameter e", "sep=col" );
   PRM_PARAMD( g,  0, "&g", "Parameter g", "" );
   PRM_PARAMD( ra, 0, "ra", "Reverse a", "def=1" );
   PRM_PARAMD( rb, 0, "rb", "Reverse b", "def=1" );
   PRM_PARAMD( x0, 0, "x0", "Input shift: y = in_0 - in_1 - x0;", "" );

   PRM_DOUBLE( y,  efInner, "y",   "in_0 - in_1 - x0", "" );
   PRM_DOUBLE( x2, efInner, "x^2", "Squared output", "" );
   PRM_DOUBLE( ae, efInner, "a/ra", "effective a", "" );
   PRM_DOUBLE( be, efInner, "b/rb", "effective b", "" );

   PRM_INPUT( in_0, 0, "in_{&0}", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_{&1}", "Second input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "ft_" );
   DCL_DEFAULT_STATIC;
};




