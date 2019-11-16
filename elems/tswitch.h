#pragma once
/***************************************************************************
                          tswitch.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
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

/**connects output to one of {u_0-u_7} inputs, depend of u_s value
  *@author atu
  */

class TSwitch : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSwitch);
   DCL_CREATE;
   DCL_STD_INF;
 protected:
   virtual double f() noexcept override;

   PRM_PARAMD( level_0,  efNRC, QSL("&Level 0"), QSL("Level to switch from in_0 to in_1"), QSL("def=0") );
   PRM_PARAMD( step,     efNRC, QSL("&Step"),    QSL("Step between levels"), QSL("sep=col\ndef=1\nmin=1e-20") );
   PRM_INT(    n_max,    efNRC, QSL("Max &n"),   QSL("Maximum level number"), QSL("sep=col\ndef=7\nmin=1\nmax=7") );

   PRM_DOUBLE( n_cur,  efInner, QSL("Current n"),   QSL("Current input number (double)"), "" );
   PRM_DOUBLE( in_all, efInner, QSL("in_{all}"),    QSL("summ of all inputs - level_0"),  "" );

   PRM_INPUT( in_0,          0, QSL("in_{&0}"), QSL("Input 0"),  QSL("sep=block") );
   PRM_INPUT( in_1,          0, QSL("in_{&1}"), QSL("Input 1"),  QSL("") );
   PRM_INPUT( in_2,          0, QSL("in_{&2}"), QSL("Input 2"),  QSL("sep=col") );
   PRM_INPUT( in_3,          0, QSL("in_{&1}"), QSL("Input 3"),  QSL("") );
   PRM_INPUT( in_4,          0, QSL("in_{&4}"), QSL("Input 4"),  QSL("sep=col") );
   PRM_INPUT( in_5,          0, QSL("in_{&5}"), QSL("Input 5"),  QSL("") );
   PRM_INPUT( in_6,          0, QSL("in_{&6}"), QSL("Input 6"),  QSL("sep=col") );
   PRM_INPUT( in_7,          0, QSL("in_{&7}"), QSL("Input 7"),  QSL("") );
   //
   PRM_INPUT( in_s,          0, QSL("&in_{s}"), QSL("switch input"),   QSL("sep=block") );
   PRM_INPUT( in_s1,         0, QSL("in_{s1}"), QSL("switch input 1"), QSL("sep=col")  );
   PRM_INPUT( in_s2,         0, QSL("in_{s2}"), QSL("switch input 2"), QSL("sep=col")  );

   PRM_DOUBLE( x2, efInner, QSL("x^2"), QSL("Squared output"), QSL("") );

   Q_CLASSINFO( "nameHintBase",  "swi_" );
   DCL_DEFAULT_STATIC;
};


