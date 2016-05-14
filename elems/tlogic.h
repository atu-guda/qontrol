#pragma once
/***************************************************************************
                          tlogic.h  -  description
                             -------------------
    begin                : Mon Sep 4 2000
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

/**simple logical operation: AND, OR, XOR
  *@author atu
  */

class TLogic : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLogic);
   DCL_CREATE;
   DCL_STD_INF;

   enum LogicType {
     logAnd = 0, logOr, logXor, logU0, logZero, logOne
   };
   Q_ENUMS(LogicType);
   Q_CLASSINFO( "enum_LogicType_0", "AND" );   // logAnd
   Q_CLASSINFO( "enum_LogicType_1", "OR" );    // logOr
   Q_CLASSINFO( "enum_LogicType_2", "XOR" );   // logXor
   Q_CLASSINFO( "enum_LogicType_3", "u[0]" );  // logU0
   Q_CLASSINFO( "enum_LogicType_4", "0" );     // logZero
   Q_CLASSINFO( "enum_LogicType_5", "1" );     // logOne
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_LIST(    type, efNRC, "&Type", "Type of logic", "enum=LogicType" );
   PRM_PARAMD( level,     0, "&level of 1", "Minimal level of '1' signal on input", "def=0.1" );
   /** misc flags */
   PRM_SWITCH( useNInp0, efNRC, "Inverse u_0", "Inverse input u_0", "sep=col" );
   PRM_SWITCH( useNInp1, efNRC, "Inverse u_1", "Inverse input u_1", ""  );
   PRM_SWITCH( useNInp2, efNRC, "Inverse u_2", "Inverse input u_2", ""  );
   PRM_SWITCH( useNInp3, efNRC, "Inverse u_3", "Inverse input u_3", ""  );
   PRM_SWITCH(  useNOut, efNRC, "Inverse out" , "Inverse input output", "sep=col"  );
   PRM_SWITCH( useMinus, efNRC, "&Negative out", "Negative output is -1, not 0", "" );

   PRM_INPUT(      in_0,     0, "u_{&0}", "First input",  "sep=block" );
   PRM_INPUT(      in_1,     0, "u_{&1}", "Second input", "sep=col" );
   PRM_INPUT(      in_2,     0, "u_{&2}", "Third input",  "sep=col" );
   PRM_INPUT(      in_3,     0, "u_{&3}", "Fourth input", "sep=col" );

   Q_CLASSINFO( "nameHintBase",  "lgc_" );
   DCL_DEFAULT_STATIC;
};



