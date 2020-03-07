#pragma once
/***************************************************************************
                          tlogic.h  -  description
                             -------------------
    begin                : Mon Sep 4 2000
    copyright            : (C) 2000-2020 by atu
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
     logAnd = 0, logOr, logXor, logU0, logZero, logOne, logEq
   };
   Q_ENUM(LogicType);
   Q_CLASSINFO( "enum_LogicType_0", "AND" );   // logAnd
   Q_CLASSINFO( "enum_LogicType_1", "OR" );    // logOr
   Q_CLASSINFO( "enum_LogicType_2", "XOR" );   // logXor
   Q_CLASSINFO( "enum_LogicType_3", "u[0]" );  // logU0
   Q_CLASSINFO( "enum_LogicType_4", "0" );     // logZero
   Q_CLASSINFO( "enum_LogicType_5", "1" );     // logOne
   Q_CLASSINFO( "enum_LogicType_6", "==" );    // logEq
 protected:
   /** main computation function */
   virtual double f() noexcept override;
   Q_INVOKABLE virtual double f_d( double arg0, double arg1 = 0, double arg2 = 0, double arg3 = 0 ) override;

   PRM_LIST(    type, efNRC, QSL("&Type"), QSL("Type of logic"), QSL("enum=LogicType") );
   // PRM_PARAMD( level, efOld, QSL("&level of 1"), QSL("Minimal level of '1' signal on input"), QSL("def=0.1") );
   /** misc flags */
   PRM_SWITCH( useNInp0, efOld, QSL("Inverse u_0"),   QSL("Inverse input u_0"), QSL("sep=col") );
   PRM_SWITCH( useNInp1, efOld, QSL("Inverse u_1"),   QSL("Inverse input u_1"), ""  );
   PRM_SWITCH( useNInp2, efOld, QSL("Inverse u_2"),   QSL("Inverse input u_2"), ""  );
   PRM_SWITCH( useNInp3, efOld, QSL("Inverse u_3"),   QSL("Inverse input u_3"), ""  );
   PRM_SWITCH(  useNOut, efNRC, QSL("Inverse out") ,  QSL("Inverse input output"), QSL("sep=col")  );
   PRM_SWITCH( useMinus, efNRC, QSL("&Negative out"), QSL("Negative output is -1, not 0"), "" );

   PRM_LOGICIN(    in_0,     0, QSL("u_{&0}"), QSL("First input"),  QSL("sep=block") );
   PRM_LOGICIN(    in_1,     0, QSL("u_{&1}"), QSL("Second input"), QSL("sep=col") );
   PRM_LOGICIN(    in_2,     0, QSL("u_{&2}"), QSL("Third input"),  QSL("sep=block") );
   PRM_LOGICIN(    in_3,     0, QSL("u_{&3}"), QSL("Fourth input"), QSL("sep=col") );

   Q_CLASSINFO( "nameHintBase",  "lgc_" );
   DCL_DEFAULT_STATIC;
};



