#pragma once
/***************************************************************************
  tqsearcher.h - q-based searcher element declaration
                             -------------------
    begin                : 2016.12.30
    copyright            : (C) 2016-2017 by atu
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

   enum PeType {
     pe_q3p = 0, pe_Fquad, pe_FCog, pe_qquad
   };
   Q_ENUMS(PeType);
   Q_CLASSINFO( "enum_PeType_0",  "q-3points"     );  // pe_q3p
   Q_CLASSINFO( "enum_PeType_1",  "F-quad"        );  // pe_Fquad
   Q_CLASSINFO( "enum_PeType_2",  "F-COG"         );  // pe_FCog
   Q_CLASSINFO( "enum_PeType_3",  "q-quad"        );  // pe_qquad

   enum FeType {
     fe_lin = 0, fe_sign, fe_lim
   };
   Q_ENUMS(FeType);
   Q_CLASSINFO( "enum_FeType_0",  "Linear"       );    // fe_lin
   Q_CLASSINFO( "enum_FeType_1",  "Sign"         );    // fe_sign
   Q_CLASSINFO( "enum_FeType_2",  "Limit"        );    // fe_lim

   enum FeFactorType {
     fef_Se = 0, fef_one, fef_F, fef_Se3, fef_W
   };
   Q_ENUMS(FeFactorType);
   Q_CLASSINFO( "enum_FeFactorType_0",  "Se"      );  // fef_Se
   Q_CLASSINFO( "enum_FeFactorType_1",  "1"       );  // fef_one
   Q_CLASSINFO( "enum_FeFactorType_2",  "F"       );  // fef_F
   Q_CLASSINFO( "enum_FeFactorType_3",  "Se3"     );  // fef_Se3
   Q_CLASSINFO( "enum_FeFactorType_4",  "W"       );  // fef_W

   enum QualType { // keep in sinc with miscfun.h::QualFunType
     qa_Gauss = 0, qa_Para, qa_Lin, qa_Hyper, qa_Log
   };
   Q_ENUMS(QualType);
   Q_CLASSINFO( "enum_QualType_0",  "Gauss"       );    // qa_Gauss
   Q_CLASSINFO( "enum_QualType_1",  "Parabola"    );    // qa_Para
   Q_CLASSINFO( "enum_QualType_2",  "Linear"      );    // qa_Lin
   Q_CLASSINFO( "enum_QualType_3",  "Hyperbolic"  );    // qa_Hyper
   Q_CLASSINFO( "enum_QualType_4",  "Logariphmic" );    // qa_Log

   enum FSOutType {
     fso_FcSe = 0, fso_FcSe3, fso_Se3, fso_Fc
   };
   Q_ENUMS(FSOutType);
   Q_CLASSINFO( "enum_FSOutType_0",  "FcSe"       );    // fso_FcSe
   Q_CLASSINFO( "enum_FSOutType_1",  "FcSe3"      );    // fso_FcSe3
   Q_CLASSINFO( "enum_FSOutType_2",  "Se3"        );    // fso_Se3
   Q_CLASSINFO( "enum_FSOutType_3",  "Fc"         );    // fso_Fc


 protected:
   virtual double f() noexcept override;
   virtual int miso_startLoop( long acnx, long acny ) override;
   // PeType dependant calculation
   void calc_pe_q3p();
   void calc_pe_Fquad();
   void calc_pe_FCog();
   void calc_pe_qquad();

   //  F
   PRM_LIST(   F_type  , 0 , "&F type"     , "Quality function type"     , "enum=QualType\nsep=block" );
   PRM_PARAMD( q_gamma , 0 , "q_{\\gamma}" , "f_e factor"                , "def=2.0\nsep=col" );
   PRM_SWITCH(  limitF , 0 , "Limit F"     , "Limit F to [0;1]"          , "def=1\nsep=col" );
   PRM_LIST(  FS_type  , 0 , "&W type"     , "Worthines stype"           , "enum=FSOutType\nsep=col" );

   // p_e
   PRM_LIST(   pe_type , 0 , "p_e type"    , "Algorithm for p_e estimation", "enum=PeType\nsep=block" );
   PRM_SWITCH( limitPe , 0 , "Limit p_e"   , "Limit p_e to [p_min;p_max]",   "def=1\nsep=col" );
   PRM_PARAMD(   p_min , 0 , "p_{min}"     , "Low p_* limit (for f_c)"  ,    "def=-1000.0\nsep=col" );
   PRM_PARAMD(   p_max , 0 , "p_{max}"     , "High p_* limit (for f_c)" ,    "def=1000.0\nsep=col" );

   // f_e
   PRM_LIST(    fe_type, 0 , "f_e type"    , "f_e calculation rule"   , "enum=FeType\nsep=block" );
   PRM_PARAMD(      k_e, 0 , "k_{&e}"      , "f_e factor"             , "def=2.0\nsep=col" );
   PRM_PARAMD( lin_rlim, 0 , "lin_{rlim}"  , "Linear f_e limit scale" , "def=0.1\nsep=col" );
   PRM_LIST(   fef_type, 0 , "factor type" , "f_e factor value selector", "enum=FeFactorType\nsep=col" );

   // f_c
   PRM_PARAMD(     k_cl, 0 , "k_{&cl}"     , "Linear f_c factor"      , "def=0.5\nsep=block" );
   PRM_PARAMD(     k_ch, 0 , "k_{ch}"      , "Hyperbolic f_c factor"  , "def=0.0\nsep=col" );

   // f_n
   PRM_PARAMD(     k_nl, 0 , "k_{&nl}"     , "Linear f_n factor"      , "def=1.0\nsep=block" );
   PRM_PARAMD(     k_nh, 0 , "k_{nh}"      , "Hyperbolic f_n factor"  , "def=0.0\nsep=col" );

   // dynamics
   // TODO: dynamics type
   PRM_PARAMD(      v_f, 0 , "&v_f"        , "Velocity factor"        , "def=1\nsep=block" );
   PRM_SWITCH(   limitP, 0 , "Limit p"     , "Limit p to [p_min;p_max]"  , "def=1\nsep=col" );

   // Outputs
   PRM_DOUBLE( p_e,   efInner, "p_e",     "x of calculated p point", "" );
   PRM_DOUBLE( p_e0,  efInner, "p_{e0}",  "unconstrained p_e", "" );
   PRM_DOUBLE( pr_e,  efInner, "pr_e",    "Relative p of calculated extremum point", "" );
   PRM_DOUBLE( pr_e0, efInner, "pr_{e0}", "unconstrained pr_e", "" );
   PRM_DOUBLE( F_c,   efInner, "F_c",     "F of central point", "" );
   PRM_DOUBLE( F_l,   efInner, "F_l",     "F of left point", "" );
   PRM_DOUBLE( F_r,   efInner, "F_r",     "F of right point", "" );
   PRM_DOUBLE( S_e,   efInner, "S_e",     "Sure level for extremum point", "" );
   PRM_DOUBLE( S_e3,  efInner, "S_e3",    "Sure level for extremum point by 3 points", "" );
   PRM_DOUBLE( W,     efInner, "W",       "Worthness = FS_e", "" );
   PRM_DOUBLE( FS_e,  efInner, "FS_e",    "FSe output eqiv value = W", "" );
   PRM_DOUBLE( FS_e0, efInner, "FS_e0",   "real F_c * S_e", "" );
   PRM_DOUBLE( brIdx, efInner, "brIdx",   "branch index", "" );
   PRM_DOUBLE( pr_b,  efInner, "pr_b",    "active bound relative", "" );
   PRM_DOUBLE( sure_coeff, efInner, "sure_coeff",   "Coeff for sureness", "" );
   PRM_DOUBLE( dist_coeff, efInner, "dist_coeff",   "Distance coeff", "" );
   // forces
   PRM_DOUBLE( f_c,   efInner, "f_c",    "force to initial parameter value", "" );
   PRM_DOUBLE( f_n,   efInner, "f_n",    "force to neigbours", "" );
   PRM_DOUBLE( f_e,   efInner, "f_e",    "force to extremum", "" );
   PRM_DOUBLE( f_t,   efInner, "f_t",    "total force", "" );
   // aux
   PRM_DOUBLE( pr_l,  efInner, "pr_l", "Relative x left point = p_l-p_c", "" );
   PRM_DOUBLE( pr_r,  efInner, "pr_r", "Relative x right point = p_r-p_c", "" );
   PRM_DOUBLE( qr_l,  efInner, "qr_l", "Relative y left point = (q_l-q_p)/q_c", "" );
   PRM_DOUBLE( qr_c,  efInner, "qr_c", "Relative y center point = (q_c-q_p)/q_c", "" );
   PRM_DOUBLE( qr_r,  efInner, "qr_r", "Relative y right point = (q_r-q_o)/q_c", "" );

   PRM_INPUT( p_l,   0, "p_{&l}", "Left point parameter",  "sep=block" );
   PRM_INPUT( q_l,   0, "&q_l",   "Left point q",  "" );
   // p_c is inner state (out0)
   PRM_INPUT( q_c,   0, "q_c",    "Central point q",  "sep=col" );
   PRM_INPUT( q_o,   0, "q_o",    "Object q",  "" );
   PRM_INPUT( p_r,   0, "p_{&r}", "Right point parameter",  "sep=col" );
   PRM_INPUT( q_r,   0, "q_r",    "Right point q",  "" );

   Q_CLASSINFO( "nameHintBase",  "p_" );
   DCL_DEFAULT_STATIC;
};



