/***************************************************************************
                          tfunctrans.h  -  description
                             -------------------
    begin                : Sun Aug 27 2000
    copyright            : (C) 2000-2016 by atu
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

#ifndef TFUNCTRANS_H
#define TFUNCTRANS_H

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
     ft_expMod, ft_sinh, ft_cosh
   };
   Q_ENUMS(FuncType);
   Q_CLASSINFO( "enum_FuncType_0",  "a*sin(b*y)+g"                         ); //  ft_sin
   Q_CLASSINFO( "enum_FuncType_1",  "a*sign(sin(b*y)+c)+g"                 ); //  ft_signSin
   Q_CLASSINFO( "enum_FuncType_2",  "a*tanh(by)+g"                         ); //  ft_tanh
   Q_CLASSINFO( "enum_FuncType_3",  "a*atan2(u0*b,u1(x))+g"                ); //  ft_atan2
   Q_CLASSINFO( "enum_FuncType_4",  "a*exp(b*y)+g"                         ); //  ft_exp
   Q_CLASSINFO( "enum_FuncType_5",  "a*exp(-b*y^2)+g"                      ); //  ft_expM2
   Q_CLASSINFO( "enum_FuncType_6",  "a*Wave(b*y)+g"                        ); //  ft_wave
   Q_CLASSINFO( "enum_FuncType_7",  "a*Mhat(b*y)+g"                        ); //  ft_mhat
   Q_CLASSINFO( "enum_FuncType_8",  "a*ln(b*y)+g"                          ); //  ft_ln
   Q_CLASSINFO( "enum_FuncType_9",  "a*y*exp(-b*y)+g"                      ); //  ft_yExp
   Q_CLASSINFO( "enum_FuncType_10", "a*y*exp(-b*y^2)+g"                    ); //  ft_yExp2
   Q_CLASSINFO( "enum_FuncType_11", "a*sin^2(b*y)+g"                       ); //  ft_sin2
   Q_CLASSINFO( "enum_FuncType_12", "a*cos(b*y)+g"                         ); //  ft_cos
   Q_CLASSINFO( "enum_FuncType_13", "a*cos^2(b*y)+g"                       ); //  ft_cos2
   Q_CLASSINFO( "enum_FuncType_14", "a*tan(b*y)+g"                         ); //  ft_tan
   Q_CLASSINFO( "enum_FuncType_15", "a*exp(-b*y*y)*(1-c*sin^2(dy))+g"      ); //  ft_exp1Msin
   Q_CLASSINFO( "enum_FuncType_16", "a*(1-exp(-b*y*y))*(1-c*sin^2(dy))+g"  ); //  ft_1Mexp1Msin2
   Q_CLASSINFO( "enum_FuncType_17", "a*exp(-(y/b)^2)+g"                    ); //  ft_expM2d
   Q_CLASSINFO( "enum_FuncType_18", "a/((|y|/b+1)+g"                       ); //  ft_divMod
   Q_CLASSINFO( "enum_FuncType_19", "a*exp(-|y|/b)+g"                      ); //  ft_expMod
   Q_CLASSINFO( "enum_FuncType_20", "a*sinh(b*y)+g"                        ); //  ft_sinh
   Q_CLASSINFO( "enum_FuncType_21", "a*cosh(b*y)+g"                        ); //  ft_cosh
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   /** type of function */
   PRM_LIST( type, 0, "Type", "Function type", "enum=FuncType" );
   /** coefficients and shift */
   PRM_PARAMD( a,  0, "a", "Parameter a", "sep=col\ndef=1" );
   PRM_PARAMD( b,  0, "b", "Parameter b", "def=1" );
   PRM_PARAMD( c,  0, "c", "Parameter c", "" );
   PRM_PARAMD( d,  0, "d", "Parameter d", "sep=col" );
   PRM_PARAMD( e,  0, "e", "Parameter e", "" );
   PRM_PARAMD( g,  0, "g", "Parameter g", "" );
   PRM_PARAMD( x0, 0, "x0", "Input shift: y = in_0 - in_1 - x0;", "" );

   PRM_DOUBLE( y,  efInner, "y",   "in_0 - in_1 - x0", "" );

   PRM_INPUT( in_0, 0, "in_0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Second input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "ft_" );
   DCL_DEFAULT_STATIC;
};

#endif



