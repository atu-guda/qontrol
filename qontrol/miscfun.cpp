/***************************************************************************
                          miscfun.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2022 by atu
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
#include "linkedobj.h"

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
  long r = QString2LongEx( s, &r_ok );
  if( r < (long)IMIN || r > (long)IMAX ) {
    r_ok = false;
  }
  if( ok ) {
    *ok = r_ok;
  }
  return int( r );
}

long QString2LongEx( const QString &s, bool *ok )
{
  bool r_ok;
  if( !ok ) {
    ok = &r_ok;
  }

  long v = s.toLong( &r_ok, 0 ); // first try as simple int
  if( r_ok ) {
    *ok = true;
    return v;
  }

  if( s == QSL("RND") ) {
    v = rand(); r_ok = true;
  } else if( s == QSL("IMIN") ) {
    v = IMIN; r_ok = true;
  } else if( s == QSL("LMIN") ) {
    v = LMIN; r_ok = true;
  } else if( s == QSL("IMAX") ) {
    v = IMAX; r_ok = true;
  } else if( s == QSL("LMAX") ) {
    v = LMAX; r_ok = true;
  }

  *ok = r_ok;
  return v;
}

QString autoVisName( const QString &in_vis_name, const QString &objName )
{
  if( !in_vis_name.isEmpty() ) {
    return in_vis_name;
  }
  QString r = objName;

  QRegExp re0( QSL("^([a-zA-Z]+)$") ); // over simple case nnn, kappa
  if( re0.indexIn( r ) != -1 ) {
    r = texword2str( r, true );
    return r;
  }

  if( r.contains('_') ) {
    QRegExp re( QSL("^([a-zA-Z0-9]+)_([a-zA-Z0-9_]+)$") ); // simple case
    if( re.indexIn( r ) != -1 ) {
      r = texword2str( re.cap(1) , true ) % QSL("_{") % texword2str( re.cap(2), true ) % QSL("}");
    } else {
      r = QSL("<div>") % r % QSL("</div>" );
    }
  }
  return r;
}



double get_real_time()
{
  timeval tv;
  gettimeofday( &tv, 0 );
  return (double)( tv.tv_sec ) + 1e-6 * (double)( tv.tv_usec );
}

NameType splitName( const QString &name, QString &first, QString &rest, int &idx )
{
  if( name.contains( QSL("..") )  ||  name.contains( ' ' ) ) {
    return badName;
  }

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


QString flags2str( int fl ) // TODO: common code
{
  static const char *elem_flag_names[elm_known_flags+1]
    { "MR", "NRC", "ND", "ROD", "NS", "RO", "St", "Im", "Pp", "SS", "?10" };
  QString s, sep;
  for( int j=0; j<elm_known_flags; j++ ) {
    if( fl & ( 1 << j ) ) {
      s += sep % elem_flag_names[j];
      s += QSL(",");
    };
  };
  return s;
}

// ----------------------- math functions ---------------

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

double barrierHypUp( double x, double lv ) noexcept
{
  x = limitUpEps( x, lv );
  return 1.0 / ( lv - x );
}

double barrierHypDown( double x, double lv ) noexcept
{
  x = limitDownEps( x, lv );
  return 1.0 / ( x - lv );
}

double barrierHypUpUp( double x, double lv ) noexcept
{
  return barrierHypUp( x, lv ) + barrierHypDown( x, -lv );
}

double barrierHypUpDown( double x, double lv ) noexcept
{
  return barrierHypUp( x, lv ) - barrierHypDown( x, -lv );
}

double barrierHyp2Up( double x, double lv ) noexcept
{
  x = limitUpEps( x, lv );
  return 1.0 / pow2( lv - x );
}

double barrierHyp2Down( double x, double lv ) noexcept
{
  x = limitDownEps( x, lv );
  return 1.0 / pow2( x - lv );
}

double barrierHyp2UpUp( double x, double lv ) noexcept
{
  return barrierHyp2Up( x, lv ) + barrierHyp2Down( x, -lv );
}

double barrierHyp2UpDown( double x, double lv ) noexcept
{
  return barrierHyp2Up( x, lv ) - barrierHyp2Down( x, -lv );
}

double qF_fun( double q, double q_gamma, int tp, bool limitF /* = true */ )
{
  double q_rel_a = fabs( q / q_gamma );
  double q_rel2 = q_rel_a * q_rel_a;
  double F_c;

  switch( tp ) {
    case qaf_Gauss  : F_c = exp( - q_rel2 ); break;
    case qaf_Para   : F_c = 1.0 - q_rel2  * oneMinusEm1; break;
    case qaf_Lin    : F_c = 1.0 - q_rel_a * oneMinusEm1; break;
    case qaf_Hyper  : F_c = 1.0 / ( 1.0 + q_rel_a * eMinus1 ); break;
    case qaf_Log    : F_c = 1.0 - log( 1.0 + q_rel_a ) * logQaScale; break;
    default         : F_c = 0.0; break;
  };
  if( limitF ) {
    F_c = clamp(  F_c, 0.0, 1.0 );
  }
  return F_c;
}


// extremum quadtatic approximation funcs and data

bool calcQuadExtr( const QuadExtrIn &in, QuadExtrOut &out )
{
  // fallback values
  out.a_1 = out.a_2 = out.x_et = 0; out.x_e = in.x_c;  out.y_e = in.y_c;
  out.was_limited = false;

  if( in.x_l >= in.x_c  || in.x_c >= in.x_r ) {
    return false;
  }

  // relative values with limits
  out.x_lt = in.x_l  - in.x_c;
  if( out.x_lt >= -D_EPS ) { out.x_lt = -D_EPS; };
  out.x_rt = in.x_r  - in.x_c;
  if( out.x_rt <=  D_EPS ) { out.x_rt =  D_EPS; };
  out.y_lt = in.y_l  - in.y_c;
  out.y_rt = in.y_r  - in.y_c;

  // limit values
  double lim_x_lt, lim_x_rt;
  if( in.limitG ) { // given
    lim_x_lt = in.x_min - in.x_c; lim_x_rt = in.x_max - in.x_c;
  } else {       // relative
    lim_x_lt = in.lim_s * out.x_lt; lim_x_rt = in.lim_s * out.x_rt;
  }

  double x_lt2 = pow2( out.x_lt );
  double x_rt2 = pow2( out.x_rt );
  double denom = x_lt2 * out.x_rt - out.x_lt * x_rt2;
  if( fabs( denom ) < D_AZERO ) { // x_r and x_l is too near
    out.was_limited = true;
    return true;
  }

  out.a_1 =   ( out.y_rt * x_lt2 - out.y_lt * x_rt2 ) / denom;
  out.a_2 = - ( out.y_rt * out.x_lt - out.y_lt * out.x_rt ) / denom;
  if( fabs( out.a_2 ) < D_AZERO ) { // near straigh line
    out.was_limited = true;
    return true;
  }

  out.x_et = - 0.5 * out.a_1 / out.a_2;

  if( in.limitX ) { // often need, else - for tests

    if( !  ( out.a_2 < 0  &&  isInBoundsNE( lim_x_lt, out.x_et, lim_x_rt ) )  ) {

      if( in.y_r > in.y_l ) { //  bound to point with large y, NOT nearest to x_cn!
        out.x_et = lim_x_rt;
      } else {
        out.x_et = lim_x_lt;
      }
      out.was_limited = true;
    }
  }

  out.x_e = in.x_c + out.x_et;
  double dy = out.a_2 * pow2( out.x_et )  + out.a_1 * out.x_et;
  out.y_e = in.y_c + dy;
  return true;
}

const MathConstantsC mathConstantsC [] = {
  { "M_E"        , M_E        }, // Value of e
  { "M_LOG2E"    , M_LOG2E    }, // Value of log_2 e
  { "M_LOG10E"   , M_LOG10E   }, // Value of log_10 e
  { "M_LN2"      , M_LN2      }, // Value of log_e 2
  { "M_LN10"     , M_LN10     }, // Value of log_e 10
  { "M_PI"       , M_PI       }, // Value of π
  { "M_PI_2"     , M_PI_2     }, // Value of π/2
  { "M_PI_4"     , M_PI_4     }, // Value of π/4
  { "M_2PI"      , 2*M_PI     }, // Value of 2π
  { "M_1_PI"     , M_1_PI     }, // Value of 1/π
  { "M_2_PI"     , M_2_PI     }, // Value of 2/π
  { "M_2_SQRTPI" , M_2_SQRTPI }, // Value of 2/√π
  { "M_SQRT2"    , M_SQRT2    }, // Value of √2
  { "M_SQRT1_2"  , M_SQRT1_2  }  // Value of 1/√2

};


MapStrDouble init_math_constants()
{
  MapStrDouble m;
  for( auto e : mathConstantsC ) {
    QString nm = L8B( e.name );
    m[nm] = e.val;
  }
  return m;
}

static MapStrDouble mathConstantsQ = init_math_constants();

const MapStrDouble& getMathConstants()
{
  return mathConstantsQ;
}

double toDoubleEx( const QString &s, bool *ok )
{
  bool ok1;
  bool *p_ok = ( ok ) ? ok : &ok1;
  double v = s.toDouble( p_ok );
  if( *p_ok ) {
    return v;
  }

  if( mathConstantsQ.contains( s ) ) {
    *p_ok = true;
    return mathConstantsQ[s];
  }

  // TODO: more, for example "=expr"

  *p_ok = false;
  return 0;
}

// ---------------------- DatasInfo -------------------------------

int DatasInfo::dump( QTextStream& os, const QString &delim )
{
  int no = 0;
  long dl_sz0 = delim.size() + 2, dl_sz = dl_sz0 - 1;
  os << "#";
  for( const auto &lbl : qAsConst( labels ) ) {
    os << qSetFieldWidth( DOUBLE_PREC+6 ) << lbl << qSetFieldWidth( dl_sz ) << delim;
    dl_sz = dl_sz0; // to more actions, but less then 'if'
  }
  os << Qt::endl;
  os.setRealNumberPrecision( DOUBLE_PREC );

  for( long i=0; i<nn; ++i ) {
    for( auto ve: ves ) {
      os << qSetFieldWidth( DOUBLE_PREC+6 ) << (*ve)[i] << qSetFieldWidth( dl_sz ) << delim;
    }
    os << Qt::endl;
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

void beautifyScale( double &a, double &b )
{
  if( b < a ) { swap( a, b ); };
  if( a == b ) { a -= 0.5; b += -0.5; }

  double dlt = b - a;
  double k1 = log10( dlt * 0.99 );
  k1 = pow( 10, floor( k1 ) );
  double k2 = 0.1 * dlt / k1;
  k2 = ( k2 > 0.5 ) ? 1 : ( ( k2 > 0.2 ) ? 0.5 : 0.2 ) ;
  k1 *= k2;
  a = k1 * floor( a / k1 );
  b = a + k1 * 10;
}

// ------------- for iface ---------------------------------------


bool confirmDelete( QWidget *par, const QString &obj, const QString &nm )
{
  int rpl = QMessageBox::question( par, PACKAGE " delete confirmation",
       QString("Do you really want to delete %1 \"%2\"?").arg( obj, nm ) );
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
  int em = fm.horizontalAdvance( QSL("W") );

  auto lay = new QVBoxLayout();


  auto treeView = new QTreeView( dia );
  treeView->setModel( obj );
  lay->addWidget( treeView );


  auto bt_ok = new QPushButton( QSL("Done"), dia );
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
  HolderData *main_s = nullptr;
  const HolderData *r_root = ho->getRootConst();
  if( r_root ) {
    main_s = r_root->getObj( QSL("model.schems.main_s") ); // may be nullptr - different structures
  }

  // for double get in form of "V.object.value"
  const LinkedObj *lobj = qobject_cast<const LinkedObj*>( ho );

  int n_subst = 0;
  const int max_n_subst = 100;
  while( su.indexIn( r, 0 ) >= 0 && n_subst < max_n_subst ) { // return value show '1'?
    int idx = su.pos( 0 );
    int l = su.matchedLength();
    QStringList sl = su.cap( 1 ).split( ':', Qt::KeepEmptyParts );

    // qWarning() << "idx = " << idx << " l= " << l << " cap[0]= " << su.cap(0) << WHE;
    // qWarning() << "cap[1] = " << su.cap(1) << " n= " << sl.size() << WHE;

    if( sl.size() < 1 ) { sl << QSL("NONAMED"); }
    if( sl.size() < 2 ) { sl << QSL("256"); }
    if( sl.size() < 3 ) { sl << QSL("%.7g"); }
    QString nm = sl[0];
    int maxlen = sl[1].toInt();
    if( maxlen < 1 ) { maxlen = 256; };

    ++n_subst;
    // qWarning() << "nm = " << nm << " maxlen= " << maxlen << " dprec= " << dprec <<  " cnv= " << cnvType << WHE;
    r.remove( idx, l );

    QString repl = QSL("???");

    if( nm.startsWith( QSL("V.") ) ) { // special case 'V.value.name'
      nm.remove( 0, 2 );
      if( lobj ) {
         const double *pv = lobj->getSchemeDoublePtr( nm );
         if( pv ) {
           repl = QString::asprintf( sl[2].toLatin1(), *pv );
         }
      }
    } else {
      HolderData *da = ho->getObj( nm );
      if( !da && main_s ) { // may be in main scheme
        da = main_s->getObj( nm );
      }

      if( da && da->isChildOf( QSL("LinkedObj") ) )  { // auto 'out0' add for TMiso
        da = da->getObj( QSL("out0") );
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
        } else {
          repl = da->objectName();
        }
      }
    }

    repl.truncate( maxlen );

    r.insert( idx, repl );
  }

  return r;
}

int splitNameNum( const QString &nm, QString &bname )
{
  QRegExp re( "^([_a-zA-Z][_a-zA-Z0-9]*[_a-zA-Z])([0-9]+)$" );
  int ne = -1;

  if( re.indexIn( nm ) != -1 ) {
    bname  = re.cap( 1 );
    ne     = re.cap( 2 ).toInt();
  } else {
    bname = nm;
  }
  return ne;
}

QString getClipboardStr()
{
  QClipboard *clp = QApplication::clipboard();
  if( !clp ) {
    return QString();
  }
  return clp->text();
}

bool setClipboardStr( const QString &s )
{
  QClipboard *clp = QApplication::clipboard();
  if( !clp ) {
    return false;
  }
  clp->setText( s );
  return true;
}

// ****************************************


