#pragma once
/***************************************************************************
                          tcounter.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
    copyright            : (C) 2000-2017 by atu
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

/**Counts input pulses and generates output pulse after n input.
  *@author atu
  */


class TCounter : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TCounter);
   DCL_CREATE;
   DCL_STD_INF;

 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_INT(        n,  efNRC, "&n",    "Number to count", "sep=col\ndef=2" );
   PRM_SWITCH(  modn,  efNRC, "&modn", "Calc to mod(N), else - limit", "sep=col\ndef=1" );
   PRM_PARAMD(     a,      0, "&a",    "output scale", "sep=block\ndef=1" );
   PRM_PARAMD(     b,      0, "&b",    "output shift", "sep=col\ndef=0" );

   PRM_LOGICIN(   in_p,     0, "in_p", "+ input source",  "sep=block" );
   PRM_LOGICIN(   in_m,     0, "in_m", "- input source",  "sep=col" );
   PRM_LOGICIN( in_rst,     0, "rst", "Signal to reset counter", "sep=col" );

   PRM_INT(         cn, efInner,     "cn", "Current counter value", "" );
   PRM_DOUBLE(     avT, efInner,    "avT",        "Average period", "def=1" );
   PRM_DOUBLE(   lastT, efInner,  "lastT",           "Last period", "def=1" );
   PRM_DOUBLE(     avF, efInner,    "avF",     "Average frequency", "def=1" );
   PRM_DOUBLE(   lastF, efInner,  "lastF",        "Last frequency", "def=1" );
   PRM_DOUBLE(    avOm, efInner,   "avOm", "Average circular frequency", "def=6.23318507" );
   PRM_DOUBLE(  lastOm, efInner, "lastOm",    "Last circular freq", "def=6.23318507" );
   PRM_DOUBLE(  tick_t, efInner, "tick_t",        "Last tick time", "def=0" );
   PRM_DOUBLE(   rst_t, efInner,  "rst_t",       "Last reset time", "def=0" );

   Q_CLASSINFO( "nameHintBase",  "cnt_" );
   DCL_DEFAULT_STATIC;
};




