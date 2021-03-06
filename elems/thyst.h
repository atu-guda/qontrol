#pragma once
/***************************************************************************
                          thyst.h  -  description
                             -------------------
    begin                : Sat Aug 26 2000
    copyright            : (C) 2000-2018 by atu
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

/**basic hysteresis simulation
  *@author atu
  */
class THyst : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(THyst);
   DCL_CREATE;
   DCL_STD_INF;

   enum HystType {
      ht_sAlphaD = 0, ht_alphaD, ht_thetaD, ht_signD,
      ht_tanhAlphaD, ht_tanhSAlphaD
   };
   Q_ENUM(HystType);
   Q_CLASSINFO( "enum_HystType_0", "s+\\alpha*d"       );   // ht_sAlphaD
   Q_CLASSINFO( "enum_HystType_1", "\\alpha*d"         );   // ht_alphaD
   Q_CLASSINFO( "enum_HystType_2", "\\Theta(d)"        );   // ht_thetaD
   Q_CLASSINFO( "enum_HystType_3", "sign(d)"           );   // ht_signD
   Q_CLASSINFO( "enum_HystType_4", "tanh(\\alpha*d)"   );   // ht_tanhAlphaD
   Q_CLASSINFO( "enum_HystType_5", "tanh(s+\\alpha*d)" );   // ht_tanhSAlphaD

 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;
   void fixState(void);
 protected:
   PRM_LIST(    type, efNRC,    "&Type", "Type of hysteresis", "enum=HystType" );
   PRM_PARAMD(    x0,     0,     "&x_0", "x0 - width if hysteresis", "min=0\nsep=col\ndef=1\nfixparm=1" );
   PRM_PARAMD( alpha,     0,  "\\alpha", "Hysteresis slope", "def=0.2"  );
   PRM_PARAMD(   s_u,     0,      "s_u", "u input shift", ""  );
   PRM_SWITCH(  absU, efNRC,      "|u|", "use |u| on input", "" );
   PRM_PARAMD(     a,     0, "&a scale", "output scale", "sep=col\ndef=1" );
   PRM_PARAMD(     b,     0, "&b shift", "output shift", ""  );
   PRM_SWITCH(  mulU, efNRC,       "*u",  "multiply to u on output", "" );
   PRM_SWITCH( mulsU, efNRC, "*sing(u)",  "multiply to sign(u) on output", "" );
   PRM_PARAMD(    au,     0,       "au", "out: +u*au", ""  );

   PRM_DOUBLE(   d, efInner,        "d", "inner state 'd'", "" );
   PRM_DOUBLE(   s, efInner,        "s", "inner state 's'", "" );

   PRM_INPUT( in_u,       0,    "&in_u", "input u(t)",  "sep=block" );

   Q_CLASSINFO( "nameHintBase",  "hyst_" );
   DCL_DEFAULT_STATIC;
};

