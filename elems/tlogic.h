/***************************************************************************
                          tlogic.h  -  description
                             -------------------
    begin                : Mon Sep 4 2000
    copyright            : (C) 2000-2013 by atu
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

#ifndef TLOGIC_H
#define TLOGIC_H

#include <tmiso.h>

static const char* const tlogic_list = 
 "AND\n"  // 0 
 "OR\n"   // 1
 "XOR\n"  // 2
 "u[0]\n" // 3
 "0\n"    // 4
 "1"      // 5
;

/**simple logical operation: AND, OR, XOR
  *@author atu
  */

class TLogic : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TLogic);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   /** main computation function */
   virtual double f( double t ) override;

   /** type of logic element */
   PRM_LIST( type, efNRC, "Type", "Type of logic", "", tlogic_list );
   /** level of 1  */
   PRM_DOUBLE( level, 0, "level of 1", "Minimal level of '1' signal on input", "def=0.1" );
   /** misc flags */
   PRM_SWITCH( useNInp0, efNRC, "Inverse u_0", "Inverse input u_0", "sep=col" );
   PRM_SWITCH( useNInp1, efNRC, "Inverse u_1", "Inverse input u_1", ""  );
   PRM_SWITCH( useNInp2, efNRC, "Inverse u_2", "Inverse input u_2", ""  );
   PRM_SWITCH( useNInp3, efNRC, "Inverse u_3", "Inverse input u_3", ""  );
   PRM_SWITCH( useNOut, efNRC,  "Inverse out" , "Inverse input output", "sep=col"  );
   PRM_SWITCH( useMinus, efNRC, "Negative out", "Negative output is -1, not 0", "" );
   
   PRM_INPUT( in_0, 0, "input 0", "First input",  "sep=block" );
   PRM_INPUT( in_1, 0, "input 1", "Second input", "sep=col" );
   PRM_INPUT( in_2, 0, "input 2", "Third input",  "sep=col" );
   PRM_INPUT( in_3, 0, "input 3", "Fourth input", "sep=col" );

   DCL_DEFAULT_STATIC;
};

#endif


