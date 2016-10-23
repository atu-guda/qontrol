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
   virtual int miso_startLoop( long acnx, long acny ) override;
   virtual int do_endLoop() override;

   int readLine() noexcept;

   PRM_STRING( filename, efNRC, "File name"   , "Input file name"          , "" );
   PRM_STRING(      sep, efNRC, "Separator"   , "Input field separator"    , "def= " );
   PRM_INT(         rnc, efNRC, "N of columns", "Number of columns to read", "min=1\nmax=32\ndef=1" );

   PRM_INT( skipBefore, efNRC, "Skip before", "Number of lines to skip before run", "min=0\ndef=0\nsep=col" );
   PRM_INT(    every_n, efNRC, "Every N", "Skip (n-1) lines after converted first", "min=1\ndef=1" );
   PRM_INT(     hold_n, efNRC, "Hold  N", "Hold converted line for N steps"       , "min=1\ndef=1" );

   PRM_PARAMD ( scale,      0, "scale", "common coefficient for all outputs"            , "def=1.0\nsep=col" );
   PRM_PARAMD( defVal,      0, "Default", "Default value for EOF and EOL case"          , "def=0.0" );
   PRM_SWITCH( useDef,  efNRC, "Use default", "Use default value, old value - elsewhere", "def=0" );

   PRM_PARAMD(    a0,       0, "a0"      , "scale for v[0]"                , "def=1.0\nsep=block" );
   PRM_PARAMD(    b0,       0, "b0"      , "shift v[0]"                    , "def=0\nsep=col" );
   PRM_SWITCH(  sub0,   efNRC, "sub v[0]", "Substract first value from all", "def=0\nsep=col" );
   PRM_DOUBLE(   v00, efInner, "v00"     , "first read value for v[0]"     , "" );

   PRM_PARAMD(    a1,       0, "a1"      , "scale for v[1]"                , "def=1.0\nsep=block" );
   PRM_PARAMD(    b1,       0, "b1"      , "shift v[1]"                    , "def=0\nsep=col" );
   PRM_SWITCH(  sub1,   efNRC, "sub v[1]", "Substract first value from all", "def=0\nsep=col" );
   PRM_DOUBLE(   v10, efInner, "v10"     , "first read value for v[1]"     , "" );

   PRM_PARAMD(   cx0,       0, "cx0"     , "factor for v[0] in x"          , "def=1.0\nsep=block" );
   PRM_PARAMD(   cx1,       0, "cx1"     , "factor for v[1] in x"          , "def=-1.0" );
   PRM_PARAMD(   cx2,       0, "cx2"     , "factor for v[2] in x"          , "sep=col" );
   PRM_PARAMD(   cx3,       0, "cx3"     , "factor for v[3] in x"          , "" );
   PRM_PARAMD(    bx,       0, "bx"      , "shift for  x"                  , "sep=col" );

   PRM_DOUBLE(    x,  efInner, "x"       , "linear combination for v[0..3]", "" );

   PRM_INT( n_total,  efInner, "n_total", "Total number of read and converted lines", "" );
   PRM_INT(   n_col,  efInner, "n_{col}", "Current number of read columns ", "" );

   PRM_DOUBLE_ARR( v, efInner, "v", "Current data", "N=32\ndef=0.0" ); // 32 is max_col

   // NO inputs

   QFile file;
   QByteArray lin;
   char sep_c { ' ' };
   int hold_i { 0 };
   bool first_read = true;

   Q_CLASSINFO( "nameHintBase",  "file_" );
   DCL_DEFAULT_STATIC;
};

