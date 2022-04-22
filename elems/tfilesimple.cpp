/***************************************************************************
         tfilesimple.c  - element to simple read text data from file (implementation)
                          -------------------
    begin                : 2016.01.25
    copyright            : (C) 2016-2022 by atu
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
 "<p>Simple (without interpolation and bind to time) text data file reader.<br>\n"
 "Some minimal calculations may be done</p>\n";

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

  v.val().assign( (unsigned)(max_col), (double)(defVal) );

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
    for( const auto &s : lba ) {
      bool ok;
      double a = s.toDouble( &ok );
      if( ok ) {
        v[i] = a * scale; ++nread_col;
      }
      ++i;
    }

    if( nread_col > 0 ) {
      v[0] = v[0] * a0 + b0; // special column 0 and 1
      if( first_read ) {  v00 = v[0];  }
      if( sub0 ) {     v[0] -= v00;   }
      v[1] = v[1] * a1 + b1;
      if( first_read ) {  v10 = v[1];  }
      if( sub1 ) {     v[1] -= v10;   }

      if( calcMore ) {
        x = ( ax / dx ) * ( cx0 * v[0] + cx1 * v[1] + cx2 * v[2] + cx3 * v[3] + cx4 * v[4] + cx5 * v[5] + cx6 * v[6] + cx7 * v[7] ) + bx;
        y = ( ay / dy ) * ( cy0 * v[0] + cy1 * v[1] + cy2 * v[2] + cy3 * v[3] + cy4 * v[4] + cy5 * v[5] + cy6 * v[6] + cy7 * v[7] ) + by;
        if( first_read ) {
          xo = x; yo = y;
        }
        vx = ( x - xo ) / ctdt;
        vy = ( y - yo ) / ctdt;
        xo = x; yo = y;
      };

      ++n_total; hold_i = 0;
      first_read = false;
      break;
    }
  };
  n_col = nread_col;

  // skip every_n-1 lines TODO: in common loop
  for( int i=1; i<every_n && ! file.atEnd(); /* NOP */ ) { // 1: sic, one line is read beforehand
    lin = file.readLine( buf_sz ).simplified();
    if( lin.size() < 1 || lin[0] == '#' || lin[0] == ';' ) { // empty or comment
      continue;
    }
    ++i;
  }

  return nread_col;
}


int TFileSimple::do_preRun()
{
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


int TFileSimple::miso_startLoop( long /*acnx*/, long /*acny*/ )
{
  n_total = 0; hold_i = 0; n_col = 0; x = 0; y = 0; xo = 0; yo = 0; vx = 0; vy = 0;
  v00 = out0_init;
  first_read = true;

  QString c_fn;
  if( filename.cval().contains( QSL("%1") ) ) {
    c_fn = filename.cval().arg( (int)(findex), index_w.cval(), 10, QChar('0') );
  } else {
    c_fn = filename.cval();
  }
  file.setFileName( c_fn );
  // qWarning() << "obj_filename: " << filename << " QFile filename" << file.fileName() << NWHE;

  if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
    qWarning() << "Fail to open data file " << file.fileName() << NWHE;
    return 0;
  }
  v.val().assign( max_col, out0_init );

  for( int i=0; i<skipBefore; ++i ) {
    lin = file.readLine( buf_sz );
  }

  if( !ca_noCalcStart ) {
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

