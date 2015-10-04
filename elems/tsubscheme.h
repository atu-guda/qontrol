/***************************************************************************
                          tsubscheme.h  - subscheme element defs
    begin                : 2015.10.03
    copyright            : (C) 2000-2015 by atu
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

#ifndef _TSUBSCHEME_H
#define _TSUBSCHEME_H


#include "tmiso.h"
#include "scheme.h"


/** Provides interface to Subscheme
  *@author atu
  */

class TSubScheme : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TSubScheme);
   DCL_CREATE;
   DCL_STD_INF;
   virtual ~TSubScheme() override;
 protected:
   /** main computation function */
   virtual double f( double t ) override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   virtual int do_postRun( int good ) override;
   virtual int do_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;
   // TODO: all do_ -- pass to cheme

   /** Scheme name */
   PRM_STRING( sch_name, efNRC, "Scheme", "Scheme name", "" );

   PRM_INPUT( in_u, 0, "input", "Single input",  "sep=block" );

   //* ptr to shadow scheme copy
   Scheme *sch = nullptr;
   static constexpr const char* const sch_ename = "sch";

   DCL_DEFAULT_STATIC;
};

#endif


