#pragma once
/***************************************************************************
  tsearcher.h - extremum searcher element declaration
                             -------------------
    begin                : 2016.04.26
    copyright            : (C) 2016-2016 by atu
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

/** Realizes some tactics in extremum search
 * in non-linear dynamic system identification task.
  *@author atu
  */

class TSearcher : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSearcher);
   DCL_CREATE;
   DCL_STD_INF;

 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( lim_s,  0,     "lim_{s}", "Limit Scale", "def=0.99" );
   PRM_SWITCH( limitG, efNRC, "Limit as given", "Limit X by given values", "sep=col" );
   // Outputs
   PRM_DOUBLE( a_1,   efInner, "a_1", "Coefficient at x", "" );
   PRM_DOUBLE( a_2,   efInner, "a_2", "Coefficient at x^2", "" );
   PRM_DOUBLE( p_cn,  efInner, "p_{cn}", "x of calculated extremum point", "" ); // main
   PRM_DOUBLE( p_cnt, efInner, "p_{cn}", "Relative x of calculated extremum point", "" );
   PRM_DOUBLE( F_cn,  efInner, "F_{cn}", "y of calculated extremum point", "" );
   PRM_DOUBLE( dy,    efInner, "dy",     "F_cn - F_c", "" );
   PRM_DOUBLE( dF_dx, efInner, "dy/dx",  "dy/(p_cn-p_c)", "" );
   PRM_DOUBLE( dF_sx, efInner, "dF_sx",  "dy*sign(p_cnt)", "" );
   PRM_DOUBLE( f_c,   efInner, "f_c",    "a-la Coulomb force", "" );
   // aux
   PRM_DOUBLE( p_lt,  efInner, "p_{lt}", "Relative x left point = p_l-p_c", "" );
   PRM_DOUBLE( p_rt,  efInner, "p_{rt}", "Relative x right point = p_r-p_c", "" );
   PRM_DOUBLE( F_lt,  efInner, "F_{lt}", "Relative y left point = F_l-F_c", "" );
   PRM_DOUBLE( F_rt,  efInner, "F_{rt}", "Relative y right point = F_r-F_c", "" );

   PRM_INPUT( p_l,   0, "p_l", "Left point parameter",  "sep=block" );
   PRM_INPUT( F_l,   0, "F_l", "Left point F",  "" );
   PRM_INPUT( F_c,   0, "F_c", "Central point F",  "sep=col" );
   PRM_INPUT( p_r,   0, "p_r", "Right point parameter",  "sep=col" );
   PRM_INPUT( F_r,   0, "F_r", "Right point F",  "" );

   Q_CLASSINFO( "nameHintBase",  "se_" );
   DCL_DEFAULT_STATIC;
};



