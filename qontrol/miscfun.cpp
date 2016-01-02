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
#include "prog_opts.h"

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
     { "MR", "NRC", "ND", "ROD", "NS", "RO", "St", "Im", "Ca", "?200?" };
  QString s = "";
  for( int j=0; j<elm_known_flags; j++ ) {
    if( fl & ( 1 << j ) ) {
      s += lbl[j]; s += ",";
    };
  };
  return s;
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

QString substValues( const QString &s, const HolderData *ho )
{
  if( !ho ) { return s; }
  QString r = s;
  QRegExp su( SUBST_RE );
  if( !su.isValid() ) {
    return QSL("BAD_RE");
  }

  int n_subst = 0;
  const int max_n_subst = 100;
  while( su.indexIn( r, 0 ) >= 0 && n_subst < max_n_subst ) { // return value show '1'?
    int idx = su.pos( 0 );
    int l = su.matchedLength();
    QStringList sl = su.cap( 1 ).split( ':', QString::KeepEmptyParts );

    // qWarning() << "idx = " << idx << " l= " << l << " cap[0]= " << su.cap(0) << WHE;
    // qWarning() << "cap[1] = " << su.cap(1) << " n= " << sl.size() << WHE;

    if( sl.size() < 1 ) { sl << QSL("NONAMED"); }
    if( sl.size() < 2 ) { sl << QSL("64"); }
    if( sl.size() < 3 ) { sl << QSL("%.7g"); }
    QString nm = sl[0];
    int maxlen = sl[1].toInt();
    if( maxlen < 1 ) { maxlen = 64; };

    ++n_subst;
    // qWarning() << "nm = " << nm << " maxlen= " << maxlen << " dprec= " << dprec <<  " cnv= " << cnvType << WHE;
    r.remove( idx, l );

    QString repl = QSL("???");
    HolderData *da = ho->getObj( nm );
    if( da->isChildOf( QSL("TMiso") ) )  { // auto 'out0' add for TMiso
      da = ho->getObj( nm % QSL(".out0") );
    }
    if( da ) {
      if( da->getTp() == QVariant::Double ) {
        double v = 0;
        HolderDouble *hod = qobject_cast<HolderDouble*>( da );
        if( hod ) {
          v = hod->cval();
        }
        repl = QString::asprintf( sl[2].toLatin1(), v );
      } else if ( da->isChildOf( QSL("HolderValue") ) ) {
        repl = da->toString();
      } else { // TODO: TMiso
        repl = da->objectName();
      }
    }

    repl.truncate( maxlen );

    r.insert( idx, repl );
  }

  return r;
}


// ****************************************


