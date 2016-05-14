#pragma once
/***************************************************************************
                          tfuncmisc.h  -  description
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


#include <tmiso.h>


/**miscelenios nonlinear functions
  *@author atu
  */

class TFuncMisc : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFuncMisc);
   DCL_CREATE;
   DCL_STD_INF;

   enum FuncType {
     ft_deadLine = 0, ft_limitLine, ft_dealLimitLine, ft_sign, ft_theta, ft_diod,
     ft_threeState, ft_triangle, ft_reactLine, ft_2slope, ft_div, ft_int,
     ft_frac, ft_aasin, ft_erf, ft_min, ft_max, ft_2slopeEx, ft_copysign
   };
   Q_ENUMS(FuncType);
   Q_CLASSINFO( "enum_FuncType_0",  "a*deadLine(y,b)+g"          );    // ft_deadLine
   Q_CLASSINFO( "enum_FuncType_1",  "a*limitLine(y,b)+g"         );    // ft_limitLine
   Q_CLASSINFO( "enum_FuncType_2",  "a*deadLimitLine(y,b,c,d)+g" );    // ft_dealLimitLine
   Q_CLASSINFO( "enum_FuncType_3",  "a*sign(y)+g"                );    // ft_sign
   Q_CLASSINFO( "enum_FuncType_4",  "a*Thetta(y)+g"              );    // ft_theta
   Q_CLASSINFO( "enum_FuncType_5",  "a*diod(y)+g"                );    // ft_diod
   Q_CLASSINFO( "enum_FuncType_6",  "a*threeState(y,b)+g"        );    // ft_threeState
   Q_CLASSINFO( "enum_FuncType_7",  "a*triangle(y,b)+g"          );    // ft_triangle
   Q_CLASSINFO( "enum_FuncType_8",  "a*rectLine(y,b,c)+g"        );    // ft_reactLine
   Q_CLASSINFO( "enum_FuncType_9",  "((y>0)?a:b)*y+g"            );    // ft_2slope
   Q_CLASSINFO( "enum_FuncType_10", "a*in_0/(in_1+b)+g"          );    // ft_div
   Q_CLASSINFO( "enum_FuncType_11", "a*int(y*b)+g"               );    // ft_int
   Q_CLASSINFO( "enum_FuncType_12", "a*frac(y*b)+g"              );    // ft_frac
   Q_CLASSINFO( "enum_FuncType_13", "a*asin(abs(sin(y*b)))+g"    );    // ft_aasin
   Q_CLASSINFO( "enum_FuncType_14", "a*erf(y*b)+g"               );    // ft_erf
   Q_CLASSINFO( "enum_FuncType_15", "a*min(in_0,in_1)+g"         );    // ft_min
   Q_CLASSINFO( "enum_FuncType_16", "a*max(in_0,in_1)+g"         );    // ft_max
   Q_CLASSINFO( "enum_FuncType_17", "a*((y>0)?b:c)*y+g"          );    // ft_2slopeEx
   Q_CLASSINFO( "enum_FuncType_18", "a*copysign(in_0,in_1)+g"    );    // ft_copysign
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
   PRM_PARAMD( d,  0, "&d", "Parameter d", "sep=col" );
   PRM_PARAMD( e,  0, "&e", "Parameter e", "" );
   PRM_PARAMD( g,  0, "&g", "Parameter g", "" );
   PRM_PARAMD( x0, 0, "x0", "Input shift: y = u[0] - u[1] - x0;", "" );

   PRM_DOUBLE( y,  efInner, "y",   "in_0 - in_1 - x0", "" );

   PRM_INPUT( in_0, 0, "in_{&0}", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_{&1}", "Second input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "fm_" );
   DCL_DEFAULT_STATIC;
};



