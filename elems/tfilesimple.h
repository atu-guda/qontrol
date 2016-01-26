#pragma once
/***************************************************************************
         tfilesimple.h  - element to simple read text data from file (defs)
                             -------------------
    begin                : 2016.01.25
    copyright            : (C) 2015-2016 by atu
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


#include <cstdio>

#include <QFile>
#include <QByteArray>

#include "tmiso.h"


class TFileSimple : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFileSimple);
   // virtual ~TFileSimple() override;
   DCL_CREATE;
   DCL_STD_INF;
   static const constexpr int max_col = 32;
   static const constexpr int buf_sz  = 4096;
 protected:

   virtual double f() noexcept override;
   virtual int do_preRun() override;
   virtual int do_postRun( int good ) override;
   virtual int miso_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;

   int readLine() noexcept;

   PRM_STRING( filename, efNRC, "File name", "Input file name", "" );
   PRM_STRING( sep,  efNRC, "Separator", "Input field separator", "def= " );
   PRM_INT( rnc,  efNRC, "N of columns", "Number of columns to read", "min=1\nmax=32\ndef=1" );

   PRM_INT( skipBefore,  efNRC, "Skip before", "Number of lines to skip before run", "min=0\ndef=0\nsep=col" );
   PRM_INT( every_n,  efNRC, "Every N", "Skip (n-1) lines after converted first", "min=1\ndef=1" );
   PRM_INT( hold_n,   efNRC, "Hold  N", "Hold converted line for N steps", "min=1\ndef=1" );

   PRM_PARAMD( scale, 0, "scale", "common coefficient for all outputs", "def=1.0\nsep=col" );
   PRM_PARAMD( defVal, 0, "Default", "Default value for EOF and EOL case", "def=0.0" );
   PRM_SWITCH( useDef, efNRC, "Use default", "Use default value, old value - elsewhere", "def=0" );

   PRM_INT( n_total,  efInner, "n_total", "Total number of read and converted lines", "" );

   PRM_DOUBLE_ARR( v, efInner, "v", "Current data", "N=32\ndef=0.0" ); // 32 is max_col

   // NO inputs

   QFile file;
   QByteArray lin;
   char sep_c { ' ' };
   int hold_i { 0 };

   Q_CLASSINFO( "nameHintBase",  "file_" );
   DCL_DEFAULT_STATIC;
};

