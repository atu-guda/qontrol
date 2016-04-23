#pragma once
/***************************************************************************
   textrtest.h - testremum test functions declaration
                             -------------------
    begin                : 2016.04.22
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

/** Special functions to test extremum searching algorithms
  * @author atu
  */

class TExtrTest : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TExtrTest);
   DCL_CREATE;
   DCL_STD_INF;

   enum FuncType {
     ft_Ackley = 0,
     ft_Quartic,
     ft_Griewangk,
     ft_HyperEllipsoid,
     ft_ParaEllip,
     ft_Rastrigin,
     ft_RastriginNovg,
     ft_Rosenbrock,
     ft_Schwefel,
     ft_Step,
     ft_PotentialAdd,
     ft_Bosom,
     ft_EggHolder,
     ft_Himmelblau,
     ft_Kantikov,
     ft_PotentialMult,
     ft_Rana,
     ft_ShekelsFoxholes,
     ft_Sombrero,
     ft_Multi1,
     ft_Multi2,
     ft_Wave,
     ft_Easom
   };
   Q_ENUMS(FuncType);
   Q_CLASSINFO( "enum_FuncType_0",  "Ackley"          );    // ft_Ackley
   Q_CLASSINFO( "enum_FuncType_1",  "Quartic"         );    // ft_Quartic
   Q_CLASSINFO( "enum_FuncType_2",  "Griewangk"       );    // ft_Griewangk
   Q_CLASSINFO( "enum_FuncType_3",  "HyperEllipsoid"  );    // ft_HyperEllipsoid
   Q_CLASSINFO( "enum_FuncType_4",  "ParaEllip"       );    // ft_ParaEllip
   Q_CLASSINFO( "enum_FuncType_5",  "Rastrigin"       );    // ft_Rastrigin
   Q_CLASSINFO( "enum_FuncType_6",  "RastriginNovg"   );    // ft_RastriginNovg
   Q_CLASSINFO( "enum_FuncType_7",  "Rosenbrock"      );    // ft_Rosenbrock
   Q_CLASSINFO( "enum_FuncType_8",  "Schwefel"        );    // ft_Schwefel
   Q_CLASSINFO( "enum_FuncType_9",  "Step"            );    // ft_Step
   Q_CLASSINFO( "enum_FuncType_10", "PotentialAdd"    );    // ft_PotentialAdd
   Q_CLASSINFO( "enum_FuncType_11", "Bosom"           );    // ft_Bosom
   Q_CLASSINFO( "enum_FuncType_12", "EggHolder"       );    // ft_EggHolder
   Q_CLASSINFO( "enum_FuncType_13", "Himmelblau"      );    // ft_Himmelblau
   Q_CLASSINFO( "enum_FuncType_14", "Kantikov"        );    // ft_Kantikov
   Q_CLASSINFO( "enum_FuncType_15", "PotentialMult"   );    // ft_PotentialMult
   Q_CLASSINFO( "enum_FuncType_16", "Rana"            );    // ft_Rana
   Q_CLASSINFO( "enum_FuncType_17", "ShekelsFoxholes" );    // ft_ShekelsFoxholes
   Q_CLASSINFO( "enum_FuncType_18", "Sombrero"        );    // ft_Sombrero
   Q_CLASSINFO( "enum_FuncType_19", "Multi1"          );    // ft_Multi1
   Q_CLASSINFO( "enum_FuncType_20", "Multi2"          );    // ft_Multi2
   Q_CLASSINFO( "enum_FuncType_21", "Wave"            );    // ft_Wave
   Q_CLASSINFO( "enum_FuncType_22", "Easom"           );    // ft_Easom
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   /** type of function */
   PRM_LIST( type, 0, "Type", "Function type", "enum=FuncType" );
   /** coefficients and shift */
   PRM_PARAMD( scale,  0, "scale", "Common scale", "sep=col\ndef=1" );
   PRM_PARAMD( shift,  0, "shift", "Common shift", "" );
   PRM_PARAMD( x0_0,   0, "x0_0",  "First  parameter base", "" );
   PRM_PARAMD( x1_0,   0, "x1_0",  "Second parameter base", "" );
   PRM_PARAMD( c0,     0, "c0",    "First  parameter scale", "def=1\nsep=col" );
   PRM_PARAMD( c1,     0, "c1",    "Second parameter scale", "def=1" );
   PRM_PARAMD( p0,     0, "p0",    "Common factor 0", "def=1" );
   PRM_PARAMD( p1,     0, "p1",    "Common factor 1", "def=1" );

   PRM_DOUBLE( x0,  efInner, "x0",    "x0", "" );
   PRM_DOUBLE( x1,  efInner, "x1",    "x1", "" );
   PRM_DOUBLE( x02, efInner, "x02",   "x0^2", "" );
   PRM_DOUBLE( x12, efInner, "x12",   "x1^2", "" );
   PRM_DOUBLE( xc,  efInner, "xc",   "x0^2+x1^2", "" );

   PRM_INPUT( in_0, 0, "in_0", "First coordinate",  "sep=block" );
   PRM_INPUT( in_1, 0, "in_1", "Second coordinate", "sep=col" );

   static constexpr const int n_foxhole = 25;
   static const int foxhole_xy[n_foxhole][2];

   Q_CLASSINFO( "nameHintBase",  "extr_" );
   DCL_DEFAULT_STATIC;
};



