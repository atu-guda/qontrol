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
   PRM_SWITCH( useNInp0, efNRC, "Inverse u[0]", "Inverse input u[0]", "sep=col" );
   PRM_SWITCH( useNInp1, efNRC, "Inverse u[1]", "Inverse input u[1]", ""  );
   PRM_SWITCH( useNInp2, efNRC, "Inverse u[2]", "Inverse input u[2]", ""  );
   PRM_SWITCH( useNInp3, efNRC, "Inverse u[3]", "Inverse input u[3]", ""  );
   PRM_SWITCH( useNOut, efNRC,  "Inverse out" , "Inverse input output", "sep=col"  );
   PRM_SWITCH( useMinus, efNRC, "Negative out", "Negative output is -1, not 0", "" );

   DCL_DEFAULT_STATIC;
};

#endif


