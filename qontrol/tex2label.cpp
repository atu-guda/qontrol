/***************************************************************************
                          tex2label.cpp  - function and data to convert
                          some TeX commands to HTML subset for QLabel
                             -------------------
    begin                : 2015.01.15
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/
#include <list>
#include <QString>
#include <QStringList>
#include <QMap>

#include "defs.h"

using namespace std;

struct TexCodeChar {
  const char *t;
  uint16_t c;
};


TexCodeChar tex_cc[] = {
  { "alpha",         0x03B1 },
  { "beta",          0x03B2 },
  { "gamma",         0x03B3 },
  { "delta",         0x03B4 },
  { "epsilon",       0x03B5 },
  { "zeta",          0x03B6 },
  { "eta",           0x03B7 },
  { "theta",         0x03B8 },
  { "iota",          0x03B9 },
  { "kappa",         0x03BA },
  { "lambda",        0x03BB },
  { "mu",            0x03BC },
  { "textmu",        0x00B5 },
  { "nu",            0x03BD },
  { "xi",            0x03BE },
  { "pi",            0x03C0 },
  { "varpi",         0x03D6 },
  { "rho",           0x03C1 },
  { "varrho",        0x03F1 },
  { "sigma",         0x03C3 },
  { "tau",           0x03C4 },
  { "upsilon",       0x03C5 },
  { "phi",           0x03C6 },
  { "chi",           0x03C7 },
  { "psi",           0x03C8 },
  { "omega",         0x03C9 },
  { "Gamma",         0x0393 },
  { "Delta",         0x0394 },
  { "Theta",         0x0398 },
  { "Lambda",        0x039B },
  { "Xi",            0x039E },
  { "Pi",            0x03A0 },
  { "Sigma",         0x03A3 },
  { "Upsilon",       0x03A5 },
  { "Phi",           0x03A6 },
  { "Psi",           0x03A8 },
  { "Omega",         0x03A9 },

  { "approx",        0x2248 },
  { "bullet",        0x2219 },
  { "cdot",          0x00B7 },
  { "diameter",      0x00F8 },
  { "emptyset",      0x2205 },
  { "exists",        0x2203 },
  { "forall",        0x2200 },
  { "geq",           0x2265 },
  { "in",            0x220A },
  { "infty",         0x221E },
  { "int",           0x222B },
  { "iint",          0x222C },
  { "iiint",         0x222D },
  { "leq",           0x2264 },
  { "mho",           0x2227 },
  { "mp",            0x2213 },
  { "nabla",         0x2207 },
  { "neq",           0x2260 },
  { "notin",         0x2209 },
  { "ohm",           0x03A9 },
  { "oint",          0x222E },
  { "oiint",         0x222F },
  { "oiiint",        0x2230 },
  { "onehalf",       0x00BD },
  { "onequarter",    0x00BC },
  { "partial",       0x2202 },
  { "pm",            0x00B1 },
  { "prod",          0x220F },
  { "propto",        0x221D },
  { "ring",          0x2218 },
  { "sim",           0x223C },
  { "sqrt",          0x221A }, // ?
  { "sum",           0x2211 },
  { "surd",          0x221A },
  { "threesuperior", 0x00B3 },
  { "times",         0x00D7 },
  { "to",            0x2192 },
  { "twosuperior",   0x00B2 },

  { "arrow",         0x2192 },
  { "Arrow",         0x21D2 },
  { "downarrow",     0x2193 },
  { "Downarrow",     0x21D3 },
  { "leftarrow",     0x2190 },
  { "Leftarrow",     0x21D0 },
  { "leftrightarrow",0x2194 },
  { "Leftrightarrow",0x21D4 },
  { "uparrow",       0x2191 },
  { "Uparrow",       0x21D1 },
  { "updownarrow",   0x2195 },
  { "Updownarrow",   0x21D5 },

  { "n",             '\n'   },
  { nullptr,           0 }
};

using MapSC = QMap<QString,QChar>;

MapSC init_tex_map()
{
  MapSC r;
  for( auto e : tex_cc ) {
    QString nm = L8B( e.t );
    r[nm] = QChar( e.c );
  }
  return r;
}

QString tex2label( const QString &t, bool noSub )
{
  static MapSC tex_map = init_tex_map();

  // way to ignore conversion
  if( t.startsWith( QSL("<div") ) ) {
    return t;
  }

  QString r, nm, post;
  QStringList stk, stk_p;
  bool estate = false;
  list<QChar> lt  ( t.cbegin(), t.cend() );

  while( !lt.empty() ) {
    QChar c = lt.front();
    lt.pop_front();
    char cc = c.toLatin1();

    if( ! estate ) {
      switch( cc ) {
        case '\\':
          estate = true;  nm = "";
          break;
        case '{':
          stk.push_back( r ); r = "";
          stk_p.push_back( post ); post = "";
          break;
        case '}':
          if( ! stk.isEmpty() ) {
            QString t = r + post;
            post = stk_p.takeLast();
            // r = stk.takeLast()  + "<span>" + t + "</span>" + post;
            r = stk.takeLast()  + t + post;
            post = "";
          }
          break;
        case '^':
          if( noSub ) {
            r += c + post; post = "";
          } else {
            r += "<sup>"; post = "</sup>";
          }
          break;
        case '_':
          if( noSub ) {
            r += c + post; post = "";
          } else {
            r += "<sub>"; post = "</sub>";
          }
          break;
        default:
          r += c + post; post = "";
          break;
      };
      continue;
    }
    else // --------- escape state
    {
      if( c.isLetterOrNumber() ) { nm += c; continue; } // continue name collecting
      if( ! nm.isEmpty() ) {
        if( tex_map.contains(nm) ) {
          r += tex_map.value(nm) + post;
        } else {
          r += "?" + nm + "?" + post;
        }
        nm = ""; post = "";

        if( cc == '\\' ) { continue; } // new name collecting
        estate = false;

        lt.push_front( c ); // re-read
        continue;

      }

      switch( cc ) {
        case '\\': case '{': case '}': case '_': case '^':
          r += c; estate = false; break;
        case ' ':
          r += QChar( 0x0080 );  break;
        default:
          r += '?';  break;
      }
      estate = false;

    } // end escape state

  }

  if( estate && ! nm.isEmpty() ) {
    if( tex_map.contains(nm) ) {
      r += tex_map.value(nm) + post;
    } else {
      r += "?" + nm + "?" + post;
    }
    nm = ""; post = "";
  }

  while( ! stk.isEmpty() ) {
    QString t = r;
    // r = stk.takeLast()  + "<span>" + t + "</span>";
    r = stk.takeLast()  + t;
  }

  return r;
}

