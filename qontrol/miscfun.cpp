/***************************************************************************
                          miscfun.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <cmath>
#include <sys/time.h>

#include <QWidget>
#include <QString>
#include <QRegExp>
#include <QMessageBox>
#include <QDomNode>
#include <QDebug>

#include "miscfun.h"
#include "datawidget.h"

using namespace std;

const char* getStateString( int stat_num )
{
  static const char * const statStr[] = {
    "Bad", "Good", "Done", "Run", "Pre", "Post", "??6", "??7", "???"
  };
  if( stat_num < 0 || stat_num > 8 ) {
    stat_num = 8;
  }
  return statStr[ stat_num ];
}

int QString2IntEx( const QString &s, bool *ok )
{
  bool r_ok;
  int v = s.toInt( &r_ok, 0 ); // first try as simple int
  if( ! r_ok ) {
    if( s == "RND" ) {
      v = rand(); r_ok = true;
    } else if( s == "IMIN" ) {
      v = IMIN; r_ok = true;
    } else if( s == "IMAX" ) {
      v = IMAX; r_ok = true;
    }
  }
  if( ok ) {
    *ok = r_ok;
  }
  return v;
}



double get_real_time()
{
  double t;
  timeval tv;
  gettimeofday( &tv, 0 );
  t = (double)( tv.tv_sec ) + 1e-6 * (double)( tv.tv_usec );
  return t;
}

NameType splitName( const QString &name, QString &first, QString &rest, int &idx )
{
  QRegExp re( RE_NAME_IDX );
  if( re.indexIn( name ) != -1 ) {
    first = re.cap(1); rest = QString();
    idx = re.cap(2).toInt();
    return simpleName;
  }
  QRegExp ref( RE_FULLNAME );
  if( ref.indexIn( name ) != -1 ) {
    first = ref.cap(1); rest = ref.cap(2);
    idx = 0; // no array of objects for now
    return complexName;
  }
  first = rest = QString(); idx = 0;
  return badName;
}


int isGoodName( const QString &s )
{
  QRegExp re( RE_NAME );
  if( re.indexIn( s ) == -1 ) {
    return 0;
  }
  return 1;
}

QString flags2str( int fl ) {
  static const char *lbl[elm_known_flags+1]
     { "MR", "NRC", "ND", "ROD", "NS", "RO" , "Im", "?1" };
  QString s = "";
  for( int j=0; j<elm_known_flags; j++ ) {
    if( fl & ( 1 << j ) ) {
      s += lbl[j]; s += ",";
    };
  };
  return s;
}

double thetta( double x )
{
  return ( x>0 ) ? 1 : 0;
}


double sign( double x )
{
  return ( x>0 ) ? 1 : ( ( x<0 ) ? -1 : 0 );
}

double deadLine( double x, double x0 )
{
  if( x > x0 ) {
    return ( x - x0 );
  }
  if( x < -x0 ) {
    return ( x + x0 );
  }
  return 0;
}

double limitLine( double x, double x0 )
{
  if( x0 < x0*(numeric_limits<double>::min()) ) {
    return 0;
  }
  if( x > x0 ) {
    return 1;
  }
  if( x < -x0 ) {
    return -1;
  }
  return (x / x0);
}

double deadLimitLine( double x, double x0, double x1, double a )
{
  if( x > x1 ) {
    return (x1-x0) * a;
  }
  if( x < -x1 ) {
    return -(x1-x0) * a;
  }
  if( x > x0 ) {
    return (x-x0) * a;
  }
  if( x < - x0 ) {
    return ( x + x0 ) * a;
  }
  return 0;
}


double triangleLine( double x, double x0 )
{
  if( x > x0 || x < -x0 ) {
    return 0;
  }
  if( x < 0 ) {
    x = -x;
  }
  if( x0 < 1e-30 ) { // just for safety
    x0 = 1e-30;
  }
  return ( x0 - x ) / x0;
}

double rectLine( double x, double xl, double xr )
{
  return ( x >= xl && x <= xr ) ? 1 : 0;
}

double threeStateLine( double x, double x0 )
{
  if( x > x0 ) {
    return 1;
  }
  if( x < -x0 ) {
    return -1;
  }
  return 0;
}

double waveWave( double x )
{
  return x * exp( -0.5 * x * x );
}

double waveMhat( double x )
{
  double x2 = x * x;
  return ( 1 - x2 ) * exp( -0.5 * x2 );
}


// int  fourier( int n, double l, const double *v,
//                      int m, double *a, double *b )
// {
//   int i , j;
//   double tc, ts, t1, t2, h;
//   if( !v || !a || !b || n==0 ) {
//     return 1;
//   }
//   h = l / n;
//   for( i=0; i<m; i++ ) {
//      tc = ts = 0; t1 = 2 * M_PI * i / l;
//      for( j=0; j<n; j++ ) {
//         t2 = t1 * j * h;
//         tc += v[j] * cos( t2 );  ts += v[j] * sin( t2 );
//      };
//      a[i] = 2 * h * tc / l;  b[i] = 2 * h * ts / l;
//   };
//   return 0;
// }




int DatasInfo::dump( QTextStream& os, const QString &delim )
{
  int no = 0;
  int dl_sz0 = delim.size() + 2, dl_sz = dl_sz0 - 1;
  os << "#";
  for( auto lbl : labels ) {
    os << qSetFieldWidth( prec+6 ) << lbl << qSetFieldWidth( dl_sz ) << delim;
    dl_sz = dl_sz0; // to more actions, but less then 'if'
  }
  os << endl;
  os.setRealNumberPrecision( prec );

  for( int i=0; i<nn; ++i ) {
    for( auto ve: ves ) {
      os << qSetFieldWidth( prec+6 ) << (*ve)[i] << qSetFieldWidth( dl_sz ) << delim;
    }
    os << endl;
    ++no;
  }
  return no;
}


// double perpLen( double xs, double ys, double xe, double ye,
//                 double xp, double yp )
// {
//   double p, r1, xf, yf;
//   xe -= xs; ye -= ys; xp -= xs; yp -= ys;
//   r1 = xe * xe + ye * ye;
//   if( r1 == 0 )
//     return 0;
//   xf = xe * ( xe * xp + ye * yp ) / r1;
//   yf = ye * ( xe * xp + ye * yp ) / r1;
//   xf -= xp; yf -= yp;
//   p = sqrt( xf * xf + yf * yf );
//   return p;
// }

double limitAngleDeg( double a )
{
  int ia = int( a / 360 );
  if( a < 0 ) { --ia; }
  a -= ia * 360;
  return a;
}

// ------------- for iface ---------------------------------------

void handleError( QWidget* par, const QString &s )
{
  qCritical() << s << WHE; // TODO: macros
  if( ! prog_opts.batch ) {
    QMessageBox::critical( par, PACKAGE ": Error", s,  QMessageBox::Ok );
  }
}

void handleWarn( QWidget* par, const QString &s )
{
  qWarning() << s << WHE; // TODO: macros
  if( ! prog_opts.batch ) {
    QMessageBox::warning( par, PACKAGE ": Warning", s,  QMessageBox::Ok );
  }
}

bool confirmDelete( QWidget *par, const QString &obj, const QString &nm )
{
  int rpl = QMessageBox::question( par, PACKAGE " delete confirmation",
       QString("Do you really want to delete %1 \"%2\"?").arg(obj).arg(nm) );
  return rpl == QMessageBox::Yes;
}

bool editObj( QWidget *par, HolderData *obj )
{
  if( !obj ) {
    return false;
  }

  auto dia = new DataDialog( *obj, par );
  int rc = dia->exec();
  delete dia;
  return( rc == QDialog::Accepted );
}

bool showTree( QWidget *par, HolderData *obj )
{
  if( !par || !obj ) {
    return false;
  }

  auto dia = new QDialog( par );
  dia->setWindowTitle( QString( PACKAGE ": Element tree: ") + obj->objectName() );

  QFontMetrics fm( dia->font() );
  int em = fm.width( 'W' );

  auto lay = new QVBoxLayout();


  auto treeView = new QTreeView( dia );
  treeView->setModel( obj );
  lay->addWidget( treeView );


  auto bt_ok = new QPushButton( "Done", dia );
  bt_ok->setDefault( true );
  lay->addWidget( bt_ok );
  dia->setLayout( lay );

  par->connect( bt_ok, &QPushButton::clicked, dia, &QDialog::accept );

  dia->resize( 86*em, 50*em ); // TODO: unmagic
  treeView->setColumnWidth( 0, 35*em );
  treeView->setColumnWidth( 1, 10*em );
  treeView->setColumnWidth( 2, 35*em );
  treeView->setColumnWidth( 3,  6*em );
  treeView->collapseAll();
  treeView->expand( obj->index( 0, 0, QModelIndex() ) );
  dia->exec();
  delete dia;

  return true;
}

QString getDomText( QDomNode &p )
{
  QString r;
  for( QDomNode no = p.firstChild(); !no.isNull() ; no = no.nextSibling() ) {
    if ( ! no.isText() ) {
      continue;
    }
    QDomText t = no.toText();
    r += t.data();
  }
  return r;
}



// ****************************************


