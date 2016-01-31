/***************************************************************************
         tfilesimple.c  - element to simple read text data from file (implementation)
                          -------------------
    begin                : 2016.01.25
    copyright            : (C) 2016-2016 by atu
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

#include "tfilesimple.h"

const char* TFileSimple::helpstr = "<H1>TFileSimple</H1>\n"
 "<p>Simple (without interpolation and bind to time) text data file reader<br>\n";

STD_CLASSINFO(TFileSimple,clpElem);

CTOR(TFileSimple,TMiso)
  , lin( buf_sz, 0 )
{
}

double TFileSimple::f() noexcept
{
  ++hold_i;
  if( hold_i < hold_n ) {
    return v[0];
  }
  hold_i = 0;

  if( useDef ) {
    v.val().assign( (unsigned)(rnc), (double)(defVal) );
  }

  readLine();

  return v[0];
}

int TFileSimple::readLine() noexcept
{
  int nread_col = 0;
  while( ! file.atEnd() ) { // EOF may be temporary, main exit - by break
    lin = file.readLine( buf_sz ).simplified();
    if( lin.size() < 1 || lin[0] == '#' || lin[0] == ';' ) { // empty or comment
      continue;
    }

    QList<QByteArray> lba = lin.split( sep_c );
    // qWarning() << "Split: [" << lba.size() << "] " << lba << " c=" << (int)sep_c <<  NWHE;
    int i = 0;
    for( auto s : lba ) {
      bool ok;
      double a = s.toDouble( &ok );
      if( ok ) {
        v[i] = a * scale; ++nread_col;
      }
      ++i;
    }
    if( nread_col > 0 ) {
       ++n_total; hold_i = 0;
       break;
    }
  };

  // skip every_n-1 lines
  for( int i=1; i<every_n; ++i ) { // 1: sic, one line is read beforehand
    file.readLine( buf_sz );
  }
  return nread_col;
}


int TFileSimple::do_preRun()
{
  file.setFileName( filename );
  sep_c = sep.cval()[0].toLatin1();
  if( !sep_c ) {
    sep_c = ' '; // workaround bug: collapsed spaces-only strings
  }
  return 1;
}


int TFileSimple::do_postRun( int /*good*/ )
{
  file.close();
  return 1;
}


int TFileSimple::miso_startLoop( int /*acnx*/, int /*acny*/ )
{
  n_total = 0; hold_i = 0;
  if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open data file " << file.fileName() << NWHE;
    return 0;
  }
  v.val().assign( max_col, out0_init );

  for( int i=0; i<skipBefore; ++i ) {
    lin = file.readLine( buf_sz );
  }

  if( !noCalcStart ) {
    readLine(); out0 = v[0];
    hold_i = 0; // to force ignore read
  }

  return 1;
}

int TFileSimple::do_endLoop()
{
  file.close();
  return 1;
}


DEFAULT_FUNCS_REG(TFileSimple)


// end of tfilesimple.cpp
