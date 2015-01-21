/***************************************************************************
         tfilesource.h  - element to read text data from file (defs)
                             -------------------
    begin                : 2015.01.21
    copyright            : (C) 2015-2015 by atu
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

#ifndef TFILESOURCE_H
#define TFILESOURCE_H

#include <QFile>

#include "tmiso.h"


class TFileSource : public TMiso  {
  Q_OBJECT
 public:
   DCL_CTOR(TFileSource);
   DCL_CREATE;
   DCL_STD_INF;

   enum InterpolateType {
     no = 0, linear
   };
   Q_ENUMS(InterpolateType);
   Q_CLASSINFO( "enum_InterpolateType_0", "No" );         // no
   Q_CLASSINFO( "enum_InterpolateType_1", "Linera" );     // linear
 protected:

   virtual double f( double t ) override;
   virtual int do_preRun( int run_tp, int an, int anx, int any, double adt ) override;
   virtual int do_startLoop( int acnx, int acny ) override;
   virtual int do_endLoop() override;

   PRM_STRING( filename, efNRC, "File name", "Input file name", "" );
   PRM_STRING( sep,  efNRC, "Separator", "Input field separator", "def=," );

   PRM_INT( greed,  efNRC, "Greed", "Number of lines to read at once, <1 = all", "def=1\nsep=col" );

   PRM_DOUBLE( tau, efNRC, "\\tau", "manual time step before file lines", "def=0.001" );

   PRM_LIST( itype, efNRC, "Interpolate", "Interpolation type", "enum=InterpolateType\nsep=col" );

   // NO inputs

   QFile file;

   DCL_DEFAULT_STATIC;
};

#endif
