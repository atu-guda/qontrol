#pragma once
/***************************************************************************
  tqsearcher.h - q-based searcher element declaration
                             -------------------
    begin                : 2016.12.30
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

/** Realizes some tactics in q-search
 * in non-linear dynamic system identification task.
  *@author atu
  */

class TQSearcher : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TQSearcher);
   DCL_CREATE;
   DCL_STD_INF;

   enum FeType {
     fe_lin = 0, fe_sign, fe_lim
   };
   Q_ENUMS(FeType);
   Q_CLASSINFO( "enum_FeType_0",  "Linear"       );    // fe_lin
   Q_CLASSINFO( "enum_FeType_1",  "Sign"         );    // fe_sign
   Q_CLASSINFO( "enum_FeType_2",  "Limit"        );    // fe_lim

   enum QualType { // like TQuality
     qa_Gauss = 0, qa_Para, qa_Lin, qa_Hyper, qa_Log
   };
   Q_ENUMS(QualType);
   Q_CLASSINFO( "enum_QualType_0",  "Gauss"       );    // qa_Gauss
   Q_CLASSINFO( "enum_QualType_1",  "Parabola"    );    // qa_Para
   Q_CLASSINFO( "enum_QualType_2",  "Linear"      );    // qa_Lin
   Q_CLASSINFO( "enum_QualType_3",  "Hyperbolic"  );    // qa_Hyper
   Q_CLASSINFO( "enum_QualType_4",  "Logariphmic" );    // qa_Log

   static const constexpr double oneMinusEm1 { 1.0 - 1.0/M_E };
   static const constexpr double eMinus1 { M_E - 1.0 };
   static const constexpr double logQaScale { oneMinusEm1 / M_LN2 };

 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;

   // f_e
   PRM_PARAMD( v_f     , 0 , "&v_f"        , "Velocity factor"        , "def=1\nsep=block" );
   PRM_LIST( fe_type   , 0 , "f_e type"    , "f_e calculation rule"   , "enum=FeType\nsep=col" );
   PRM_PARAMD( k_e     , 0 , "k_{&e}"      , "f_e factor"             , "def=2.0\nsep=col" );
   PRM_PARAMD( lin_rlim, 0 , "lin_{rlim}"  , "Linear f_e limit scale" , "def=0.1\nsep=col" );
   // f_c
   PRM_PARAMD( k_cl    , 0 , "k_{&cl}"     , "Linear f_c factor"      , "def=0.5\nsep=block" );
   PRM_PARAMD( k_ch    , 0 , "k_{ch}"      , "Hyperbolic f_c factor"  , "def=0.0\nsep=col" );
   PRM_PARAMD( p_min   , 0 , "p_{min}"     , "Low p limit (for f_c)"  , "def=-1000.0\nsep=col" );
   PRM_PARAMD( p_max   , 0 , "p_{max}"     , "High p limit (for f_c)" , "def=1000.0\nsep=col" );
   // f_n
   PRM_PARAMD( k_nl    , 0 , "k_{&nl}"     , "Linear f_n factor"      , "def=1.0\nsep=block" );
   PRM_PARAMD( k_nh    , 0 , "k_{nh}"      , "Hyperbolic f_n factor"  , "def=0.0\nsep=col" );
   // F
   PRM_LIST( F_type    , 0 , "F type"      , "Quality type"           , "enum=QualType\nsep=block" );
   PRM_PARAMD( q_gamma , 0 , "q_{\\gamma}" , "f_e factor"             , "def=2.0\nsep=col" );
   PRM_SWITCH( limitF  , 0 , "Limit F"     , "Limit F to [0;1]"       , "def=1\nsep=col" );

   // Outputs
   PRM_DOUBLE( p_e,   efInner, "p_e", "x of calculated extremum point", "" );
   PRM_DOUBLE( pr_e,  efInner, "pr_e", "Relative p of calculated extremum point", "" );
   PRM_DOUBLE( F_c,   efInner, "F_c", "F of central point", "" );
   PRM_DOUBLE( S_e,   efInner, "S_e", "Sure level for extremum point", "" );
   PRM_DOUBLE( FS_e,  efInner, "FS_e", "F_C * S_e", "" );
   // forces
   PRM_DOUBLE( f_c,   efInner, "f_c",    "force to initial parameter value", "" );
   PRM_DOUBLE( f_n,   efInner, "f_n",    "force to neigbours", "" );
   PRM_DOUBLE( f_e,   efInner, "f_e",    "force to extremum", "" );
   PRM_DOUBLE( f_t,   efInner, "f_t",    "total force", "" );
   // aux
   PRM_DOUBLE( pr_l,  efInner, "pr_l", "Relative x left point = p_l-p_c", "" );
   PRM_DOUBLE( pr_r,  efInner, "pr_r", "Relative x right point = p_r-p_c", "" );
   PRM_DOUBLE( qr_l,  efInner, "qr_l", "Relative y left point = (q_l-q_p)/q_c", "" );
   PRM_DOUBLE( qr_c,  efInner, "qr_l", "Relative y left point = (q_l-q_p)/q_c", "" );
   PRM_DOUBLE( qr_r,  efInner, "qr_r", "Relative y right point = (q_r-q_o)/q_c", "" );

   PRM_INPUT( p_l,   0, "p_{&l}", "Left point parameter",  "sep=block" );
   PRM_INPUT( q_l,   0, "&q_l",   "Left point q",  "" );
   // p_c is inner state (out0)
   PRM_INPUT( q_c,   0, "q_c",    "Central point q",  "sep=col" );
   PRM_INPUT( q_o,   0, "q_o",    "Object q",  "" );
   PRM_INPUT( p_r,   0, "p_{&r}", "Right point parameter",  "sep=col" );
   PRM_INPUT( q_r,   0, "q_r",    "Right point q",  "" );

   Q_CLASSINFO( "nameHintBase",  "a_" );
   DCL_DEFAULT_STATIC;
};



