#pragma once
/***************************************************************************
     tquality.h -- TQuality element decription
                             -------------------
    begin                : 2016.02.29
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


#include <cmath>
#include <tmiso.h>


/** some common quality functions
  *@author atu
  */

class TQuality : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TQuality);
   DCL_CREATE;
   DCL_STD_INF;

   enum QualType { // keep in sinc with miscfun.h::QualFunType
     qa_Gauss = 0, qa_Para, qa_Lin, qa_Hyper, qa_Log
   };
   Q_ENUM(QualType);
   Q_CLASSINFO( "enum_QualType_0",  "Gauss"       );    // qa_Gauss
   Q_CLASSINFO( "enum_QualType_1",  "Parabola"    );    // qa_Para
   Q_CLASSINFO( "enum_QualType_2",  "Linear"      );    // qa_Lin
   Q_CLASSINFO( "enum_QualType_3",  "Hyperbolic"  );    // qa_Hyper
   Q_CLASSINFO( "enum_QualType_4",  "Logariphmic" );    // qa_Log
 protected:
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_LIST( type,       0, "&Type", "Function type", "enum=QualType" );
   PRM_PARAMD( q_gamma,  0, "&q_\\gamma", "Quality scale", "sep=col\ndef=1\nmin=1e-100" );
   PRM_PARAMD( F_min,    0, "&F_{min}", "Minimal F value", "def=-1e100\nsep=col" );
   PRM_PARAMD( F_max,    0, "F_{max}", "Maximal F value", "def=1e100" );

   PRM_DOUBLE( q,    efInner, "q",     "q_o - q_m", "" );
   PRM_DOUBLE( q_r,  efInner, "q_r",   "q/q_\\gamma", "" );
   PRM_DOUBLE( aq_r, efInner, "aq_r",  "|q_r|", "" );
   PRM_DOUBLE( q_r2, efInner, "q_r^2", "q_r^2", "" );

   PRM_INPUT( q_o, 0, "in_{&o}", "Object q",  "sep=block" );
   PRM_INPUT( q_m, 0, "in_{&m}", "Model q",   "sep=col" );

   PRM_DOUBLE( x2, efInner, "x^2", "Squared output", "" );

   Q_CLASSINFO( "nameHintBase",  "qa_" );
   DCL_DEFAULT_STATIC;
};



