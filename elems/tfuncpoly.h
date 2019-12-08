#pragma once
/***************************************************************************
                          tfuncpoly.h  -  description
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

/**polinomial functions
  *@author atu
  */

class TFuncPoly : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFuncPoly);
   DCL_CREATE;
   DCL_STD_INF;

   enum FuncType {
     ft_lin = 0, ft_square, ft_cube, ft_biSquare, ft_sqrt, ft_hypot, ft_4square,
     ft_vibro, ft_msquare, ft_xExp, ft_lin2, ft_linAbs,
     ft_pow4, ft_pow5, ft_pow, ft_pows, ft_hypot4, ft_muldiv, ft_mulsum
   };
   Q_ENUM(FuncType);
   Q_CLASSINFO( "enum_FuncType_0", "ae*y+g"                                  ); // ft_lin
   Q_CLASSINFO( "enum_FuncType_1", "ae*y^2+be*y+g"                           ); // ft_square
   Q_CLASSINFO( "enum_FuncType_2", "ae*y^3+be*y^2+c*y+g"                     ); // ft_cube
   Q_CLASSINFO( "enum_FuncType_3", "ae*u0^2+be*u0*u1+c*u1^2+g"               ); // ft_biSquare
   Q_CLASSINFO( "enum_FuncType_4", "ae*sqrt(be*y+c)+g"                       ); // ft_sqrt
   Q_CLASSINFO( "enum_FuncType_5", "e*hypot(ae*u0,be*u1)+g"                  ); // ft_hypot
   Q_CLASSINFO( "enum_FuncType_6", "ae*u0^2+..+d*u3^2+g"                     ); // ft_4square
   Q_CLASSINFO( "enum_FuncType_7", "Vibro Ampl(\\omega); A=c0 B=\\omega"     ); // ft_vibro
   Q_CLASSINFO( "enum_FuncType_8", "ae*(u1-u0^2)^2+be*(1-u0)^2"              ); // ft_msquare
   Q_CLASSINFO( "enum_FuncType_9", "1-exp(-ae*((u0^2+u1^2-1)^2-be*u0-c*u1))" ); // ft_xExp
   Q_CLASSINFO( "enum_FuncType_10","ae*(1+be*y)+g"                           ); // ft_lin2
   Q_CLASSINFO( "enum_FuncType_11","ae*(be*y+c*abs(y))+g"                    ); // ft_linAbs
   Q_CLASSINFO( "enum_FuncType_12","ae*y^4+be*y^3+c*y^2+d*y+g"               ); // ft_pow4
   Q_CLASSINFO( "enum_FuncType_13","ae*y^5+be*y^4+c*y^3+d*y^2+e*y+g"         ); // ft_pow5
   Q_CLASSINFO( "enum_FuncType_14","ae*pow(be*y,c)+g"                        ); // ft_pow
   Q_CLASSINFO( "enum_FuncType_15","ae*pow(be*|y|,c)*sign(y)+g"              ); // ft_pows
   Q_CLASSINFO( "enum_FuncType_16", "e*hypot4(ae*u0,be*u1,c*u2,d*u3)+g"      ); // ft_hypot4
   Q_CLASSINFO( "enum_FuncType_17", "ae*(y*in_2)/be+g"                       ); // ft_muldiv
   Q_CLASSINFO( "enum_FuncType_18", "ae/be*(c*u0*u1 + d*u2*u3) +g"           ); // ft_mulsum
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
   PRM_DOUBLE( y2, efInner, QSL("y^2"),  QSL("y^2")             , QES );
   PRM_DOUBLE( ae, efInner, QSL("a/ra"), QSL("effective a")     , QES );
   PRM_DOUBLE( be, efInner, QSL("b/rb"), QSL("effective b")     , QES );

   PRM_INPUT( in_0, 0, QSL("in_{&0}"), QSL("First input"),  QSL("sep=block") );
   PRM_INPUT( in_1, 0, QSL("in_{&1}"), QSL("Second input"), QSL("sep=col") );
   PRM_INPUT( in_2, 0, QSL("in_{&2}"), QSL("Third input"),  QSL("sep=block") );
   PRM_INPUT( in_3, 0, QSL("in_{&3}"), QSL("Fourth input"), QSL("sep=col") );

   PRM_DOUBLE( x2, efInner, QSL("x^2"), QSL("Squared output"), QES );

   Q_CLASSINFO( "nameHintBase",  "fp_" );
   DCL_DEFAULT_STATIC;
};



