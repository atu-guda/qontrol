#pragma once
/***************************************************************************
         tfilesource.h  - element to read text data from file (defs)
                             -------------------
    begin                : 2015.01.21
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


class TFileSource : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFileSource);
   ~TFileSource();
   DCL_CREATE;
   DCL_STD_INF;
   static const constexpr int max_col = 16;
   static const constexpr int buf_sz  = 4096;

   enum InterpolateType {
     step = 0, linear
   };
   Q_ENUMS(InterpolateType);
   Q_CLASSINFO( "enum_InterpolateType_0", "Step" );       // step
   Q_CLASSINFO( "enum_InterpolateType_1", "Linear" );     // linear
   struct LineData {
     double v[max_col];
     LineData() { reset(); }
     void reset() { for( auto &a :v ) { a = 0.0; }; };
   };
 protected:

   virtual double f() noexcept override;
   virtual int do_preRun() override;
   virtual int do_postRun( int good ) override;
   virtual int miso_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;

   PRM_STRING( filename, efNRC, "File name", "Input file name", "" );
   PRM_SWITCH( asProc, efNRC, "Assume as program", "Assume filesname as program name with arguments", "" );
   PRM_STRING( sep,  efNRC, "Separator", "Input field separator", "def= " );
   PRM_INT( rnc,  efNRC, "N of columns", "Number of columns to read", "min=1\nmax=32\ndef=1" );

   PRM_INT( greed,  efNRC, "Greed", "Number of lines to read at once, <1 = all", "min=1\ndef=1\nsep=col" );
   PRM_DOUBLE( tau, efNRC, "\\tau", "Manual time step before file lines, <0=from simulation", "def=0.001" );
   PRM_INT( time_col,  efNRC, "Time column", "Determine time dy values in given column, -1 = manual", "def=-1" );

   PRM_LIST( itype, efNRC, "Interpolate", "Interpolation type", "enum=InterpolateType\ndef=1\nsep=col" );
   PRM_PARAMD( scale, 0, "scale", "commom coefficient for all outputs", "def=1.0" );

   PRM_DOUBLE_ARR( v, efInner, "v", "Current data", "N=32\ndef=0.0" ); // 32 is max_col

   // NO inputs

   QFile file;
   FILE *p_fi = nullptr;
   QByteArray lin;
   LineData d0, d1, d_c;
   std::vector<LineData> pv; // store for read-ahead values
   double tau_e = 1; // effective tau
   double t0 = 0.0, t1 = 1.0;
   int n_ofs = 0; // start of buffer to file
   int ncl = 0;   // number of lines in current buffer
   int cl = 0;    // current line index in buffer (t0,d0)
   bool wasEOF = false;

   int readLines( int ltr ); // ltr = lines to read, ignoring empty and comments
   int next_tau_e();
   void all_close();

   Q_CLASSINFO( "nameHintBase",  "file_" );
   DCL_DEFAULT_STATIC;
};

