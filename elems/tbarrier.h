#pragma once
/***************************************************************************
   tbarrier.h - barrier functions object declararion
                             -------------------
    begin                : 2016.04.24
    copyright            : (C) 2016-2020 by atu
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

class TBarrier : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TBarrier);
   DCL_CREATE;
   DCL_STD_INF;

   enum BarrierType {
     bt_HypUp = 0,
     bt_HypDown,
     bt_HypUpUp,
     bt_HypUpDown,
     bt_Hyp2Up,
     bt_Hyp2Down,
     bt_Hyp2UpUp,
     bt_Hyp2UpDown
   };
   Q_ENUM(BarrierType);
   Q_CLASSINFO( "enum_BarrierType_0",  "HyperbolicUp"      );    // bt_HypUp
   Q_CLASSINFO( "enum_BarrierType_1",  "HyperbolicDown"    );    // bt_HypDown
   Q_CLASSINFO( "enum_BarrierType_2",  "HyperbolicUpUp"    );    // bt_HypUpUp
   Q_CLASSINFO( "enum_BarrierType_3",  "HyperbolicUpDown"  );    // bt_HypUpDown
   Q_CLASSINFO( "enum_BarrierType_4",  "Hyperbolic2Up"     );    // bt_Hyp2Up
   Q_CLASSINFO( "enum_BarrierType_5",  "Hyperbolic2Down"   );    // bt_Hyp2Down
   Q_CLASSINFO( "enum_BarrierType_6",  "Hyperbolic2UpUp"   );    // bt_Hyp2UpUp
   Q_CLASSINFO( "enum_BarrierType_7",  "Hyperbolic2UpDown" );    // bt_Hyp2UpDown
 protected:
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_LIST( type,     0, "&Type",  "Barrier type", "enum=BarrierType" );
   PRM_PARAMD( b,      0, "&b",     "Barrier x value", "def=5" );
   PRM_PARAMD( scale,  0, "sc&ale", "Output scale", "sep=col\ndef=1" );
   PRM_PARAMD( shift,  0, "&shift", "Output shift", "" );
   PRM_PARAMD( x_0,    0, "x_{&0}", "Parameter shift", "" );

   PRM_INPUT( x,   0, "&x",   "X input",  "sep=block" );
   PRM_INPUT( x_n, 0, "&x_n", "negative input",  "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "br_" );
   DCL_DEFAULT_STATIC;
};


