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

   enum F_g_Type  {
     fg_p_e = 0,
     fg_diff
   };
   Q_ENUMS(F_g_Type);
   Q_CLASSINFO( "enum_F_g_Type_0",  "p_e based"            );    // fg_p_e
   Q_CLASSINFO( "enum_F_g_Type_1",  "dF/dp based"          );    // fg_diff

 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   PRM_PARAMD( lim_s,  0,     "lim_{s}", "Limit Scale", "def=0.99" );
   PRM_PARAMD( v_f,    0,     "v_f",     "Velocity factor", "def=1\nsep=col" );
   // PRM_SWITCH( limitG, efNRC, "Limit as given", "Limit X by given values", "sep=col" );
   // f_c
   PRM_PARAMD( k_cl,  0,     "k_{cl}",  "Linear f_c factor", "def=0.1\nsep=block" );
   PRM_PARAMD( k_ch,  0,     "k_{ch}",  "Hyperbolic f_c factor", "def=1.0\nsep=col" );
   PRM_PARAMD( k_ch2, 0,     "k_{ch2}", "Hyperbolic^2 f_c factor", "def=0.0\nsep=col" );
   // f_n
   PRM_PARAMD( k_nl,  0,     "k_{nl}",  "Linear f_n factor", "def=0.0\nsep=block" );
   PRM_PARAMD( k_nh,  0,     "k_{nh}",  "Hyperbolic f_n factor", "def=0.0\nsep=col" );
   PRM_PARAMD( k_nh2, 0,     "k_{nh2}", "Hyperbolic^2 f_n factor", "def=1.0\nsep=col" );
   // f_g
   PRM_LIST( f_g_type, 0, "Type", "f_g type", "enum=F_g_Type\nsep=block" );
   PRM_PARAMD( k_g,   0,     "k_g",    "f_g factor", "def=2.0\nsep=col" );

   // Outputs
   PRM_DOUBLE( L_l0,  efInner, "L_{l0}", "Initial distance to left model", "" );
   PRM_DOUBLE( L_r0,  efInner, "L_{r0}", "Initial distance to right model", "" );
   PRM_DOUBLE( a_1,   efInner, "a_1", "Coefficient at x", "" );
   PRM_DOUBLE( a_2,   efInner, "a_2", "Coefficient at x^2", "" );
   PRM_DOUBLE( p_e,   efInner, "p_{e}", "x of calculated extremum point", "" ); // main
   PRM_DOUBLE( p_et,  efInner, "p_{et}", "Relative p of calculated extremum point", "" );
   PRM_DOUBLE( F_e,   efInner, "F_{e}", "F of calculated extremum point", "" );
   PRM_DOUBLE( dF,    efInner, "dF",     "F_e - F_c", "" );
   PRM_DOUBLE( dF_dp, efInner, "dF/dp",  "dF/(p_e-p_c)", "" );
   // forces
   PRM_DOUBLE( f_c,   efInner, "f_c",    "force to initial parameter value", "" );
   PRM_DOUBLE( f_n,   efInner, "f_n",    "force to neigbours", "" );
   PRM_DOUBLE( f_g,   efInner, "f_g",    "force to extremum", "" );
   PRM_DOUBLE( f_t,   efInner, "f_t",    "total force", "" );
   // aux
   PRM_DOUBLE( p_lt,  efInner, "p_{lt}", "Relative x left point = p_l-p_c", "" );
   PRM_DOUBLE( p_rt,  efInner, "p_{rt}", "Relative x right point = p_r-p_c", "" );
   PRM_DOUBLE( F_lt,  efInner, "F_{lt}", "Relative y left point = F_l-F_c", "" );
   PRM_DOUBLE( F_rt,  efInner, "F_{rt}", "Relative y right point = F_r-F_c", "" );

   PRM_INPUT( p_l,   0, "p_l", "Left point parameter",  "sep=block" );
   PRM_INPUT( F_l,   0, "F_l", "Left point F",  "" );
   PRM_INPUT( p_c_fake,   efRO, "No p_c", "placeholder for p_c",  "sep=col" );
   PRM_INPUT( F_c,   0, "F_c", "Central point F",  "" );
   PRM_INPUT( p_r,   0, "p_r", "Right point parameter",  "sep=col" );
   PRM_INPUT( F_r,   0, "F_r", "Right point F",  "" );

   Q_CLASSINFO( "nameHintBase",  "se_" );
   DCL_DEFAULT_STATIC;
};



