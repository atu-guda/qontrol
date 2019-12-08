#pragma once
/***************************************************************************
                          tfuncmisc.h  -  description
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
     ft_frac, ft_aasin, ft_erf, ft_min, ft_max, ft_2slopeEx, ft_copysign, ft_clamp, ft_switch
   };
   Q_ENUM(FuncType);
   Q_CLASSINFO( "enum_FuncType_0",  "ae*deadLine(y,be)+c*y+g"      );    // ft_deadLine
   Q_CLASSINFO( "enum_FuncType_1",  "ae*limitLine(y,be)+c*y+g"     );    // ft_limitLine
   Q_CLASSINFO( "enum_FuncType_2",  "ae*deadLimitLine(y,be,c,d)+g" );    // ft_dealLimitLine
   Q_CLASSINFO( "enum_FuncType_3",  "ae*sign(y)+c*y+g"             );    // ft_sign
   Q_CLASSINFO( "enum_FuncType_4",  "ae*Thetta(y)+c*y+g"           );    // ft_theta
   Q_CLASSINFO( "enum_FuncType_5",  "ae*posval(y)+c*y+g"           );    // ft_diod
   Q_CLASSINFO( "enum_FuncType_6",  "ae*threeState(y,be)+c*y+g"    );    // ft_threeState
   Q_CLASSINFO( "enum_FuncType_7",  "ae*triangle(y,be)+c*y+g"      );    // ft_triangle
   Q_CLASSINFO( "enum_FuncType_8",  "ae*rectLine(y,be,d)+c*y+g"    );    // ft_reactLine
   Q_CLASSINFO( "enum_FuncType_9",  "((y>e)?ae:be)*y+g"            );    // ft_2slope
   Q_CLASSINFO( "enum_FuncType_10", "ae*in_0/(in_1+be)+c*y+g"      );    // ft_div
   Q_CLASSINFO( "enum_FuncType_11", "ae*int(y*be)+c*y+g"           );    // ft_int
   Q_CLASSINFO( "enum_FuncType_12", "ae*frac(y*be)+g"              );    // ft_frac
   Q_CLASSINFO( "enum_FuncType_13", "ae*asin(abs(sin(y*be)))+g"    );    // ft_aasin
   Q_CLASSINFO( "enum_FuncType_14", "ae*erf(y*be)+g"               );    // ft_erf
   Q_CLASSINFO( "enum_FuncType_15", "ae*min(in_0,in_1)+g"          );    // ft_min
   Q_CLASSINFO( "enum_FuncType_16", "ae*max(in_0,in_1)+g"          );    // ft_max
   Q_CLASSINFO( "enum_FuncType_17", "ae*((y>e)?be:c)*y+g"          );    // ft_2slopeEx
   Q_CLASSINFO( "enum_FuncType_18", "ae*copysign(in_0,in_1)+g"     );    // ft_copysign
   Q_CLASSINFO( "enum_FuncType_19", "ae*clamp(y,be,d)+c*t+g"       );    // ft_clamp
   Q_CLASSINFO( "enum_FuncType_20", "ae*((in_2>d)?(be*in_0):(c*in_1))+g" );  // ft_switch
 protected:
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_LIST(    type, 0, QSL("&Type"), QSL("Function type")                      , QSL("enum=FuncType") );
   PRM_PARAMD(     a, 0, QSL("&a")   , QSL("Parameter a")                        , QSL("def=1") );
   PRM_PARAMD(     b, 0, QSL("&b")   , QSL("Parameter b")                        , QSL("def=1") );
   PRM_PARAMD(     c, 0, QSL("&c")   , QSL("Parameter c")                        , QES );
   PRM_PARAMD(     d, 0, QSL("&d")   , QSL("Parameter d")                        , QES );
   PRM_PARAMD(     e, 0, QSL("&e")   , QSL("Parameter e")                        , QES );
   PRM_PARAMD(     g, 0, QSL("&g")   , QSL("Parameter g")                        , QSL("sep=col") );
   PRM_PARAMD(    ra, 0, QSL("ra")   , QSL("Reverse a")                          , QSL("def=1") );
   PRM_PARAMD(    rb, 0, QSL("rb")   , QSL("Reverse b")                          , QSL("def=1") );
   PRM_PARAMD(    x0, 0, QSL("x0")   , QSL("Input shift: y = in_0 - in_1 - x0;") , QES );
   PRM_PARAMD(  vmin, 0, QSL("vmin") , QSL("Minimum value")                      , QSL("def=-1e100") );
   PRM_PARAMD(  vmax, 0, QSL("vmax") , QSL("Maximum value")                      , QSL("def=1e100") );

   PRM_DOUBLE( y,  efInner, QSL("y"),    QSL("in_0 - in_1 - x0"), QES );
   PRM_DOUBLE( ae, efInner, QSL("a/ra"), QSL("effective a"),      QES );
   PRM_DOUBLE( be, efInner, QSL("b/rb"), QSL("effective b"),      QES );

   PRM_INPUT( in_0, 0, QSL("in_{&0}"), QSL("First input"),  QSL("sep=block") );
   PRM_INPUT( in_1, 0, QSL("in_{&1}"), QSL("Second input"), QES );
   PRM_INPUT( in_2, 0, QSL("in_{&2}"), QSL("Input N 2"),    QSL("sep=col") );

   // misc values
   PRM_DOUBLE(  ax,  efInner,  QSL("ax"), QSL("|in_0|"),         QES  );
   PRM_DOUBLE(  ay,  efInner,  QSL("ay"), QSL("|y|"),            QES  );
   PRM_DOUBLE(  ao,  efInner,  QSL("ao"), QSL("|out|"),          QES  );
   PRM_DOUBLE(  x2,  efInner, QSL("x^2"), QSL("Squared output"), QES );

   Q_CLASSINFO( "nameHintBase",  "fm_" );
   DCL_DEFAULT_STATIC;
};



