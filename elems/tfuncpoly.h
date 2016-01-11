/***************************************************************************
                          tfuncpoly.h  -  description
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

#ifndef TFUNCPOLY_H
#define TFUNCPOLY_H

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
     ft_pow4, ft_pow5, ft_pow, ft_pows,
   };
   Q_ENUMS(FuncType);
   Q_CLASSINFO( "enum_FuncType_0", "a*y+g"                                 ); // ft_lin
   Q_CLASSINFO( "enum_FuncType_1", "a*y^2+b*y+g"                           ); // ft_square
   Q_CLASSINFO( "enum_FuncType_2", "a*y^3+b*y^2+c*y+g"                     ); // ft_cube
   Q_CLASSINFO( "enum_FuncType_3", "a*u0^2+b*u0*u1+c*u1^2+g"               ); // ft_biSquare
   Q_CLASSINFO( "enum_FuncType_4", "a*sqrt(b*y+c)+g"                       ); // ft_sqrt
   Q_CLASSINFO( "enum_FuncType_5", "hypot(a*u0,b*u1)+g"                    ); // ft_hypot
   Q_CLASSINFO( "enum_FuncType_6", "a*u0^2+..+d*u3^2+g"                    ); // ft_4square
   Q_CLASSINFO( "enum_FuncType_7", "Vibro Ampl(\\omega); A=c0 B=\\omega"   ); // ft_vibro
   Q_CLASSINFO( "enum_FuncType_8", "a*(u1-u0^2)^2+b*(1-u0)^2"              ); // ft_msquare
   Q_CLASSINFO( "enum_FuncType_9", "1-exp(-a*((u0^2+u1^2-1)^2-b*u0-c*u1))" ); // ft_xExp
   Q_CLASSINFO( "enum_FuncType_10","a*(1+b*y)+g"                           ); // ft_lin2
   Q_CLASSINFO( "enum_FuncType_11","a*(b*y+c*abs(y))+g"                    ); // ft_linAbs
   Q_CLASSINFO( "enum_FuncType_12","a*y^4+b*y^3+c*y^2+d*y+g"               ); // ft_pow4
   Q_CLASSINFO( "enum_FuncType_13","a*y^5+b*y^4+c*y^3+d*y^2+e*y+g"         ); // ft_pow5
   Q_CLASSINFO( "enum_FuncType_14","a*pow(b*y,c)+g"                        ); // ft_pow
   Q_CLASSINFO( "enum_FuncType_15","a*pow(b*|y|,c)*sign(y)+g"              ); // ft_pows
 protected:
   /** main computation function */
   virtual double f() noexcept override;

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
   PRM_DOUBLE( y2, efInner, "y^2", "y^2", "" );

   PRM_INPUT( in_0, 0, "in_0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "in_2", "Third input",  "sep=block" );
   PRM_INPUT( in_3, 0, "in_3", "Fourth input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "fp_" );
   DCL_DEFAULT_STATIC;
};

#endif


