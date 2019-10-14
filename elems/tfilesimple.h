#pragma once
/***************************************************************************
         tfilesimple.h  - element to simple read text data from file (defs)
                             -------------------
    begin                : 2016.01.25
    copyright            : (C) 2015-2019 by atu
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

   PRM_STRING( filename,     0, "File name"   , "Input file name"          , "" );
   PRM_STRING(      sep, efNRC, "Separator"   , "Input field separator"    , "def= " );
   PRM_DOUBLE(   findex, efNRC, "File index"  , "value to substitute in file name", "" );
   PRM_INT(     index_w, efNRC, "Index width" , "Number of digits in index", "def=2" );

   PRM_INT( skipBefore, efNRC, "Skip before", "Number of lines to skip before run", "min=0\ndef=0\nsep=col" );
   PRM_INT(    every_n, efNRC, "Every N", "Skip (n-1) lines after converted first", "min=1\ndef=1" );
   PRM_INT(     hold_n, efNRC, "Hold  N", "Hold converted line for N steps"       , "min=1\ndef=1" );

   PRM_PARAMD (   scale,      0, "scale", "common coefficient for all inputs"             , "def=1.0\nsep=col" );
   PRM_PARAMD(   defVal,      0, "Default", "Default value for EOF and EOL case"          , "def=0.0" );
   PRM_SWITCH( calcMore,  efNRC, "Calc More", "Calculate more values (x,y,dv...)"         , "def=1" );

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
   PRM_PARAMD(   cx2,       0, "cx2"     , "factor for v[2] in x"          , "" );
   PRM_PARAMD(   cx3,       0, "cx3"     , "factor for v[3] in x"          , "" );
   PRM_PARAMD(   cx4,       0, "cx4"     , "factor for v[4] in x"          , "sep=col" );
   PRM_PARAMD(   cx5,       0, "cx5"     , "factor for v[5] in x"          , "" );
   PRM_PARAMD(   cx6,       0, "cx6"     , "factor for v[6] in x"          , "" );
   PRM_PARAMD(   cx7,       0, "cx7"     , "factor for v[7] in x"          , "" );
   PRM_PARAMD(    ax,       0, "ax"      , "scale for x"                   , "def=1\nsep=col" );
   PRM_PARAMD(    dx,       0, "dx"      , "denominator for x"             , "def=1" );
   PRM_PARAMD(    bx,       0, "bx"      , "shift for x"                   , "" );

   PRM_PARAMD(   cy0,       0, "cy0"     , "factor for v[0] in y"          , "def=0.0\nsep=block" );
   PRM_PARAMD(   cy1,       0, "cy1"     , "factor for v[1] in y"          , "def=1.0" );
   PRM_PARAMD(   cy2,       0, "cy2"     , "factor for v[2] in y"          , "def=-1.0" );
   PRM_PARAMD(   cy3,       0, "cy3"     , "factor for v[3] in y"          , "" );
   PRM_PARAMD(   cy4,       0, "cy4"     , "factor for v[4] in y"          , "sep=col" );
   PRM_PARAMD(   cy5,       0, "cy5"     , "factor for v[5] in y"          , "" );
   PRM_PARAMD(   cy6,       0, "cy6"     , "factor for v[6] in y"          , "" );
   PRM_PARAMD(   cy7,       0, "cy7"     , "factor for v[7] in y"          , "" );
   PRM_PARAMD(    ay,       0, "ay"      , "scale for y"                   , "def=1\nsep=col" );
   PRM_PARAMD(    dy,       0, "dy"      , "denominator for y"             , "def=1" );
   PRM_PARAMD(    by,       0, "by"      , "shift for y"                   , "" );

   PRM_DOUBLE(    x,  efInner, "x"       , "linear combination for v[0..7]", "" );
   PRM_DOUBLE(    y,  efInner, "y"       , "linear combination for v[0..7]", "" );
   PRM_DOUBLE(   vx,  efInner, "vx"      , "dx/dt", "" );
   PRM_DOUBLE(   vy,  efInner, "vy"      , "dy/dt", "" );
   PRM_DOUBLE(   xo,  efInner, "xo"      , "old x", "" );
   PRM_DOUBLE(   yo,  efInner, "yo"      , "old y", "" );

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

