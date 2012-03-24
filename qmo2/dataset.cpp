/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <vector>
#include <cstdio>
#include "miscfun.h"
#include "dataset.h"

using namespace std;

// ---------------- TDataInfo -----------------------

// new really neeed only name type, and subtype -- all other fake
int TDataInfo::read( istream *is )
{
  int k, l, j, nline;
  char str[MAX_INPUTLEN];
  tp = subtp = 0; dlg_x = dlg_y = dlg_w = dlg_h = 0;
  v_max = 0; v_min = 1; // no limits
  flags = 0;
  name[0] = 0;
  descr = listdata = "";
  
  nline = 0;  // must consist of 4 lines of data (don't realy)
  while( 1 ) {
    is->getline( str, MAX_INPUTLEN );
    if( is->eof() || is->fail() )  
      return -1; 
    l = strlen( str );
    if( l < 1 ) continue;
    k = typeOfLine( str, 0, 0, 0, 0 );
    if( k == ltpComment ) continue;  // ordinary comment;
    if( k != ltpStruct )             // only @ -start strings;
       return -2;
    str[0] = ' ';
    switch( nline ) {
      case 0: j = sscanf( str, "%d %d %d %d %d %d %d %d %lf %lf",
                &tp, &subtp, &max_len, &dlg_x, &dlg_y, &dlg_w, &dlg_h,
                &flags, &v_min, &v_max );
              k = ( j != 10 );  break;
      case 1: j = sscanf( str, "%32s" , name );
              k =  j != 1; break;
      case 2: k = 0; break;
      case 3: k = 0; break;
      default: fprintf( stderr, "Too many input lines (%d):TDataInfo::read", 
		        nline );
	       return -7;
    };
    nline++;
    if( nline >= 4 ) break;
    if( k ) { 
      return -3;
    };   
  };
  if( tp < 0 || !isGoodName(name) ) {
    return -1;
  };
  hval = hashVal( name );
  dyna = 1;
  return 0;
}

int TDataInfo::save( ostream *os ) const
{
  *os << "@ " << tp << ' ' << subtp << ' ' << max_len << ' '
      << dlg_x << ' ' << dlg_y << ' ' << dlg_w << ' ' << dlg_h << ' '
      << flags << ' ' << v_min << ' ' << v_max << '\n';
  *os << "@ " << name << '\n';
  *os << "@ \n@ \n";
  return 0;
}
// ---------------- HolderData .... ----------------------

HolderData::HolderData( const QString &obj_name, QObject *a_parent )
           :QObject( a_parent ), old_tp(0), old_subtp(0), dyn(0),
	    tp(QVariant::Invalid), ptr(0)
{
  setObjectName( obj_name );
}

// ---------------- HolderInt ---------

HolderInt::HolderInt( int *p, const QString &obj_name, QObject *a_parent  ) // if p==0 - autocreate 
          :HolderData( obj_name, a_parent ),
	   val(p)
{
  if( !val ) {
    val = new int; dyn = 1;
  }
  ptr = val; tp=QVariant::Int; old_tp = dtpInt; old_subtp = dtpsInt;
}

HolderInt::~HolderInt()
{
  if( val )
    delete val;
  val = 0; ptr = val;
}

bool HolderInt::set( const QVariant & x )
{
  bool ok;
  *val = x.toInt( &ok );
  return 0;
}

QVariant HolderInt::get() const
{
  return QVariant( *val );
}

QString HolderInt::toString() const
{
  return QString::number( *val );
}

bool HolderInt::fromString( const QString &s )
{
  bool ok;
  *val = s.toInt( &ok, 0 ); // 0 = auto base
  return ok;
}


// ---------------- HolderDouble ---------

HolderDouble::HolderDouble( double *p, const QString &obj_name, QObject *a_parent ) // if p==0 - autocreate 
          :HolderData( obj_name, a_parent ),
	   val(p)
{
  if( !val ) {
    val = new double; dyn = 1;
  }
  ptr = val; tp=QVariant::Double; old_tp = dtpDouble;
}

HolderDouble::~HolderDouble()
{
  if( val )
    delete val;
  val = 0; ptr = val;
}

bool HolderDouble::set( const QVariant & x )
{
  bool ok;
  *val = x.toDouble( &ok );
  return 0;
}

QVariant HolderDouble::get() const
{
  return QVariant( *val );
}

QString HolderDouble::toString() const
{
  return QString::number( *val, 'g', 16 );
}

bool HolderDouble::fromString( const QString &s )
{
  bool ok;
  *val = s.toDouble( &ok ); 
  return ok;
}

// ---------------- TDataSet ------------------------

TClassInfo TDataSet::class_info = 
 {  CLASS_ID_TDataSet, "TDataSet",  TDataSet::create, 0, helpstr };

TDataInfo TDataSet::dataset_d_i[1] = {
 { dtpEnd, 0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, 0, 0 , "", "", "" }
};

const char* TDataSet::helpstr = 
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

TDataSet::TDataSet( TDataSet* aparent )
         :QObject( aparent )
{
 parent = aparent;
 nelm = 0; allow_add = 0; d_i_alloc = 0; state = stateGood; modified = 0;
 ptrs.reserve( 64 ); // good value for simple objects
 d_i = dataset_d_i;
 initHash();
}

TDataSet::~TDataSet()
{
  int i;
  TDataSet* ob;
  for( i=0; i<nelm; i++ ) {
    if( d_i[i].dyna == 0 && d_i[i].tp != dtpObj ) continue;
    ob = static_cast<TDataSet*>( ptrs[i] ); 
    delete ob;
    ptrs[i] = 0;
  };
  nelm = 0; state = stateBad;
  if( d_i_alloc ) {
    delete d_i; d_i = 0;
  };
}

int TDataSet::getClassId(void) const 
{ 
  return CLASS_ID_TDataSet; 
}

const char* TDataSet::getClassName(void) const 
{ 
  return "TDataSet"; 
}

const TClassInfo* TDataSet::getClassInfo(void) const
{
  return &class_info;
}

const char* TDataSet::getName(void) const
{
  if( parent == 0 )
    return "";
  return parent->getChildName( this );    
}

const char* TDataSet::getChildName( const TDataSet* child ) const
{
  int i;
  for( i=0; i<nelm; i++ ) {
    if( static_cast<const void*>(child) == ptrs[i] )
      return d_i[i].name;
  };
  return 0;
}

int TDataSet::getFullName( char *dst ) const
{
  const TDataSet *cob, *th;
  const char *s;
  char tmp[MAX_INPUTLEN];
  int i, j, k, ne, l, pt[256];
  if( dst == 0 ) return -2;
  dst[0] = tmp[0] = 0;
  cob = parent; th = this; ne = 0; pt[0] = 0;
  while( cob != 0 && ne < 128 ) {
    s = cob->getChildName( th );
    if( s == 0 ) return -1;
    strncat( tmp, s, MAX_INPUTLEN-1 );
    strncat( tmp, ".", MAX_INPUTLEN-1 );
    l = strlen( tmp ); ne++; pt[ne] = l;
    th = cob; cob = cob->parent;
  };
  k = 0;
  for( i=ne-1; i>=0; i-- ) {
    for( j=pt[i]; j<pt[i+1]-1; j++ ) {
      dst[k++] = tmp[j];
    };
    if( i > 0 )
      dst[k++] = '.';
  };
  dst[k] = 0;
  return 0;
}

TDataSet* TDataSet::create( TDataSet* apar )
{
  return new TDataSet( apar );
}

TDataSet* TDataSet::createObj( int id, TDataSet* apar )
{
  if( id == getClassId() )  // same object type
    return create( apar );
  if( parent == 0 )         // no parent - no creating
    return 0;
  return parent->createObj( id, apar );
}

const char *TDataSet::getHelp(void) const
{
  return helpstr;
}

int TDataSet::getN(void) const
{
  return nelm;
}

void* TDataSet::getObj( int ni )
{
  if( ni < 0 || ni >= nelm )
    return 0;
  return ptrs[ni];
}

void* TDataSet::getObj( const char *ename )
{
  int ni;
  ni = getDataIdx( ename );
  if( ni < 0 || ni >= nelm )
    return 0;
  return ptrs[ni];
}

const TDataInfo* TDataSet::getDataInfo( int ni ) const
{
  if( ni >= nelm || ni < 0 ) 
    return 0;
  return &d_i[ni];
}

// TODO: use real hash or map !!!
int TDataSet::getDataIdx( const char *nm ) const
{
  int i, hv;
  hv = hashVal( nm );
  for( i=0; i<nelm; i++ ) {
    if( hv == d_i[i].hval ) {
      if( strcmp( d_i[i].name, nm ) == 0 )
        return i;
    };
  };
  return -1;
}

// TODO: use one functions name with overload and hooks for overriding
int TDataSet::getDataII( int ni, int *da, int allowConv )
{
  int tp, v;
  if( ni >= nelm || ni < 0 ) return -1;
  tp = d_i[ni].tp;
  if( tp == dtpInt ) {
    *da = *((int*)(ptrs[ni])); 
    return 0;
  };
  if( ! allowConv ) return -2;
  if( tp == dtpDbl ) {
    v = int( *((double*)(ptrs[ni])) );
    *da = v;
    return 0;
  };
  if( tp == dtpStr ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    v = s->toInt();
    *da = v;
    return 0;
  };
  return -1;
}

int TDataSet::getDataSI( const char *nm, int *da, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = getDataII( i, da, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    if( ob == 0 ) return -5;
    k = ob->getDataSI( rname, da, allowConv );
  };    
  return k;
}

int TDataSet::getDataID( int ni, double *da, int allowConv )
{
  int tp;
  double v;
  if( ni >= nelm || ni < 0 ) return -1;
  tp = d_i[ni].tp;
  if( tp == dtpDbl ) {
    *da = *((double*)(ptrs[ni])); 
    return 0;
  };
  if( tp == dtpInt ) {
    v = ( *((int*)(ptrs[ni])) );
    *da = v;
    return 0;
  };
  if( ! allowConv ) return -2;
  if( tp == dtpStr ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    v = s->toDouble();
    *da = v;
    return 0;
  };
  return -1;
}

int TDataSet::getDataSD( const char *nm, double *da, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = getDataID( i, da, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    if( ob == 0 ) return -5;
    k = ob->getDataSD( rname, da, allowConv );
  };    
  return k;
}

int TDataSet::getDataIS( int ni, QString *da, int maxlen, int allowConv )
{
  int tp, iv; double dv;
  if( ni >= nelm || ni < 0  || maxlen < 1 ) return -1;
  tp = d_i[ni].tp;
  if( tp == dtpStr ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    *da = *s;
    da->truncate( maxlen ); // TODO: unused?
    return 0;
  };
  if( ! allowConv ) return -2;
  if( tp == dtpInt ) {
    iv = int( *((double*)(ptrs[ni]))); 
    QString t;  t.setNum( iv );
    *da = t;
    return 0;
  };
  if( tp == dtpDbl ) {
    dv = *((double*)(ptrs[ni])); 
    QString t;
    t.setNum( dv, 'g', 16 );
    *da = t;
    return 0;
  };
  return -1;
}

int TDataSet::getDataSS( const char *nm, QString *da, int maxlen, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = getDataIS( i, da, maxlen, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    if( ob == 0 ) return -5;
    k = ob->getDataSS( rname, da, maxlen, allowConv );
  };    
  return k;
}

int TDataSet::setDataII( int ni, int da, int allowConv )
{
  int tp;
  if( ni >= nelm || ni < 0 ) return -1;
  if( d_i[ni].flags & efRO ) return -2;
  if( state == stateRun  &&  (d_i[ni].flags & efNoRunChange) ) return -2;
  if( d_i[ni].v_min < d_i[ni].v_max ) {
    if( da < d_i[ni].v_min ) da = int( d_i[ni].v_min );
    if( da > d_i[ni].v_max ) da = int( d_i[ni].v_max );
  };
  if( ni >= nelm || ni < 0  ) return -1;
  tp = d_i[ni].tp;
  if( tp == dtpInt ) {
    *(static_cast<int*>(ptrs[ni])) = da;
    modified |= 1; return 0;
  };
  if( tp == dtpDbl ) {
    *(static_cast<double*>(ptrs[ni])) = da;
    modified |= 1; return 0;
  };
  if( allowConv && tp == dtpStr && d_i[ni].max_len > 1 ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    s->setNum( da );
    modified |= 1; return 0;
  };
  return -1;
}

int TDataSet::setDataSI( const char *nm, int da, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = setDataII( i, da, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    k = ob->setDataSI( rname, da, allowConv );
  };
  if( k == 0 ) // parent assumed modified if any of it's child modified.
    modified |= 1;
  return k;
}

int TDataSet::setDataID( int ni, double da, int allowConv )
{
  int tp;
  if( ni >= nelm || ni < 0 ) return -1;
  if( d_i[ni].flags & efRO ) return -2;
  if( state == stateRun  &&  (d_i[ni].flags & efNoRunChange) ) return -2;
  if( d_i[ni].v_min < d_i[ni].v_max ) {
    if( da < d_i[ni].v_min ) da = d_i[ni].v_min;
    if( da > d_i[ni].v_max ) da = d_i[ni].v_max;
  };
  if( ni >= nelm || ni < 0 ) return -1;
  tp = d_i[ni].tp;
  if( tp == dtpDbl ) {
    *(static_cast<double*>(ptrs[ni])) = da;
    modified |= 1; return 0;
  };
  if( ! allowConv ) return -2;
  if( tp == dtpInt ) {
    *(static_cast<int*>(ptrs[ni])) = int( da );
    modified |= 1; return 0;
  };
  if( tp == dtpStr && d_i[ni].max_len > 1 ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    s->setNum( da, 'g', 16 );
    modified |= 1; return 0;
  };
  return -1;
}

int TDataSet::setDataSS( const char *nm, const QString *da, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = setDataIS( i, da, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    k = ob->setDataSS( rname, da, allowConv );
  };
  if( k == 0 ) // parent assumed modified if any of it's child modified.
    modified |= 1;
  return k;
}

int TDataSet::setDataIS( int ni, const QString *da, int allowConv )
{
  int tp, iv; double dv;
  if( ni >= nelm || ni < 0 ) return -1;
  if( d_i[ni].flags & efRO ) return -2;
  if( state == stateRun  &&  (d_i[ni].flags & efNoRunChange) ) return -2;
  tp = d_i[ni].tp;
  if( tp == dtpStr ) {
    QString *s = static_cast<QString*>(ptrs[ni]); // TODO: qobject_cast
    *s = *da;
    s->truncate( d_i[ni].max_len );
    modified |= 1; return 0;
  };
  if( ! allowConv ) return -2;
  if( tp == dtpInt ) {
    iv = da->toInt();
    if( d_i[ni].v_min < d_i[ni].v_max ) {
      if( iv < d_i[ni].v_min ) iv = int( d_i[ni].v_min );
      if( iv > d_i[ni].v_max ) iv = int( d_i[ni].v_max );
    };
    *(static_cast<int*>(ptrs[ni])) = iv;
    modified |= 1; return 0;
  };
  if( tp == dtpDbl ) {
    dv = da->toDouble();
    if( d_i[ni].v_min < d_i[ni].v_max ) {
      if( dv < d_i[ni].v_min ) dv = d_i[ni].v_min;
      if( dv > d_i[ni].v_max ) dv = d_i[ni].v_max;
    };
    *(static_cast<double*>(ptrs[ni])) = dv;
    modified |= 1; return 0;
  };
  return -1;
}

int TDataSet::setDataSD( const char *nm, double da, int allowConv )
{
  int i, k;
  char fname[MAX_NAMELEN], rname[MAX_INPUTLEN];
  TDataSet* ob;
  //  aaa.bbb handling
  k = splitName( nm, fname, rname );
  if( k < 0 )  return -1;  // bad name
  i = getDataIdx( fname );
  if( i < 0 )   // no such name
      return i;
  if( k == 1 ) { // only left part of name w/o '.'
    k = setDataID( i, da, allowConv );
  } else {  // both component of name: aa.bb.cc -> aa  bb.cc
    if( d_i[i].tp != dtpObj  ||  ptrs[i] == 0 )
      return -1;
    ob = static_cast<TDataSet*> (ptrs[i]);
    k = ob->setDataSD( rname, da, allowConv );
  };
  if( k == 0 ) // parent assumed modified if any of it's child modified.
    modified |= 1;
  return k;
}


int TDataSet::checkData( int /* ni */ )
{
  return 0;
}


int TDataSet::saveDatas( ostream *os )
{
  int i;
  *os << getClassId() << '(' << getClassName() << ")={\n";
  // some debug? values
  *os << "%!d n=" << nelm << "; allow_add=" << allow_add << '\n';
  for( i=0; i<nelm; i++ ) {
     saveData( i, os );
  };
  *os << "}; -- end of " << getClassName();
  modified = 0;
  return 0;
}

int TDataSet::loadDatas( istream *is )
{
  int k, r_id;
  //const char *mcn;
  char str[MAX_INPUTLEN], nm[MAX_NAMELEN];
  //mcn = getClassName(); //lm = strlen( mcn );
  while( 1 ) {                           // find: id(ClassName)={
    is->getline( str, MAX_INPUTLEN );  
    if( !is->good() ) return -1;
    k = typeOfLine( str, MAX_INPUTLEN, &r_id, nm, 0 );
    if( k == ltpComment ) continue;
    if( k != ltpStart ) return -2;
    break;
  };    
  if( r_id != getClassId() ) return -3;  // not my class ??
  // atu: debug:
  //fprintf( stderr, "TDataSet::loadDatas: reading object %d(%s)\n",
  //         r_id, nm );
  
  k = ltpComment; 
  while( k > 0 && k != ltpEnd ) {
    k = processElem( is );
  };    
  checkData(-1);
  state = 1; modified = 0;
  return 0;
}

// protected:
int TDataSet::initHash(void)
{
   int i, hv;
   nelm = 0; 
   for( nelm=0; nelm < MAX_DATAELEM && d_i[nelm].tp !=0; nelm++ ) { /*NOP */} ;
   for( i=0; i<nelm; i++ ) {
     hv = hashVal( d_i[i].name );
     d_i[i].hval = hv;
   };
   return 0;
}

int TDataSet::saveData( int ni, ostream *os ) const
{
  TDataSet* ob;
  int k, iv;
  double dv;
  const QString *sv;
  if( ni >= nelm || ni < 0 ) return -1;
  if( d_i[ni].flags & ( efNoSave | efStatic ) ) return 0;
  if( d_i[ni].dyna ) {  // was created dynamycaly - save info
       *os << "@ -- datainfo for " << d_i[ni].name << "; \n";
       d_i[ni].save( os );
  };
  switch( d_i[ni].tp ) {
    case dtpEnd: return -1;
    case dtpInt: iv = *((int*)(ptrs[ni]));
          *os << d_i[ni].name << '=' << iv; break;
    case dtpDou: dv = *((double*)(ptrs[ni]));
          *os << d_i[ni].name << '=' << dv; break;
    case dtpStr: sv = (const QString*)(ptrs[ni]);
          *os << d_i[ni].name << '=';
          saveStr( os, sv->toLocal8Bit().constData() ); break;
    case dtpFun: *os << "# func: " << d_i[ni].name; break;
    case dtpFunPure: *os << "# func pure: " << d_i[ni].name; break;
    case dtpObj: ob = static_cast<TDataSet*>( ptrs[ni] );
          if( ob == 0 ) return -1;
          if( ob->getClassId() != d_i[ni].subtp ) return -1;
          *os << d_i[ni].name << '=' << d_i[ni].subtp
              << '(' << ob->getClassName() << ")\n";
          k = ob->saveDatas( os );
          if( k ) return k;
          break;
    case dtpDial: case dtpLabel: case dtpGroup: return 0; // dialog elements
    default: return -1;
  };
  *os << " ; \n";
  return 0;
}

int TDataSet::processElem( istream *is )
{
  int i, j, k, l, tp, iv;
  double dv;
  TDataSet* ob;
  QString val, delim, tbuf;
  char nm[MAX_NAMELEN];
  char str[MAX_INPUTLEN];
  TDataInfo inf;
  inf.descr = inf.listdata = 0; // safe values 
  is->getline( str, MAX_INPUTLEN );
  if( is->fail() ) return -1;
  k = typeOfLine( str, MAX_INPUTLEN, &l, nm, &val );
  if( k == ltpEnd )
    return k;
  if( k == ltpUnk ) {
    fprintf( stderr, "TDataSet::processElem: unknown line:\n%s\n", str );
    return k;
  };
  if( k == ltpComment ) return ltpComment;
  if( k == ltpVal ) {
    j = getDataIdx( nm );
    if( j < 0 ) {
      fprintf( stderr, "TDataSet::processElem: unknown name:\n%s\n", nm );
      return ltpComment;
    };
    tp = d_i[j].tp;
    // atu: debug
    //fprintf( stderr, "TDataSet::processElem: reading %s[%d](%d.%d)\n" ,
    //         nm, j, tp, d_i[j].subtp  );
    switch( tp ) {
      case dtpEnd:
             fprintf( stderr, "TDataSet::processElem: end datainfos?\n" );
             return -4;         // must not happend
      case dtpInt:
             iv = val.toInt();
             setDataII( j, iv, 1 ); break;
      case dtpDou:
             dv =  val.toDouble();      // double
             setDataID( j, dv, 1 ); break;
      case dtpStr:
             setDataIS( j, &val, 1 );  break;
      case dtpFun:      return ltpComment;  // for funcs -- can't be read
      case dtpFunPure:  return ltpComment;  // ==
      case dtpObj:
             ob = static_cast<TDataSet*> (ptrs[j]);
	     if( ob == 0 ) {
	       fprintf( stderr, "TDataSet::processElem: empty ptrs[%d]\n", j );
	       return -7;
	     };
	     k = ob->loadDatas( is );
             break;
      case dtpDial: case dtpLabel: case dtpGroup: return ltpComment;
      default:
             fprintf( stderr, "TDataSet::processElem: bad type %d\n", tp );
             return -5;   // undefined type
    };
    return ltpVal;
  };
  
  if( k == ltpValStart ) {  // multi-line string value
    j = getDataIdx( nm );
    if( j < 0 ) return -3;
    tp = d_i[j].tp;
    if( tp != dtpStr ) return -4; // ???? != ????
    delim = val;
    i = readMlStr( is, &tbuf, d_i[j].max_len, delim.toLocal8Bit().constData() );
    if( i ) { return -8; };
    setDataIS( j, &tbuf, 1 );
    return ltpValStart;
  };
  
  if( k == ltpStruct ) {        // structure def comment -- first line empty
    if( !allow_add || !d_i_alloc ) 
      return -12; 
    j = inf.read( is );         // here allocated descr, listdata
    if( j ) {
      fprintf( stderr, "TDataSet::processElem: fail to read structure:\n %s %d\n",
              str, j );
      return -10;
    };
    j = add_obj( &inf );
    if( j != 0 ) {
      fprintf( stderr, "TDataSet::processElem: fail to create %s\n",
               inf.name );
      // delete[] inf.descr; delete[] inf.listdata;
      return -14;
    };  
    return ltpStruct;
  };
  return 0;
}

int TDataSet::add_obj( const TDataInfo* dai )
{
  int j, k;
  TDataSet* ob;
  void *sob; char *cob;
  if( !allow_add || !d_i_alloc )
    return -1;
  if( nelm >= MAX_DATAELEM )
    return -1;
  if( dai == 0 )
    return -1;
  k = getDataIdx( dai->name );
  if( k >= 0 ) return -1; // name busy
  if( !isValidType( dai->tp, dai->subtp ) )
    return -1;	  
  switch( dai->tp ) {
    case dtpInt: sob = new int;     break;
    case dtpDou: sob = new double;  break;
    case dtpStr: j = dai->max_len;
            if( j < 1 ) return -1;
            sob = cob = new char[j+2];
	    cob[0] = 0;  break;
    case dtpObj: ob = createObj( dai->subtp, this );
            if( ob == 0 ) {
              fprintf( stderr, "TDataSet::add_obj: fail to create: %s %d\n",
                       dai->name, dai->subtp );
              return -1;
            };
            ob->setObjectName( QString::fromLocal8Bit(dai->name) );
            sob = ob;
            break;
    case dtpDial: case dtpLabel: case dtpGroup: sob = 0; break;
    default: return -1;  // unknown object type
  };
  if( int(ptrs.size()) <= nelm )
    ptrs.push_back(0);
  ptrs[nelm] = sob;
  d_i[nelm].tp = dai->tp; d_i[nelm].subtp = dai->subtp;
  d_i[nelm].max_len = dai->max_len;
  d_i[nelm].dlg_x = dai->dlg_x; d_i[nelm].dlg_y = dai->dlg_y;
  d_i[nelm].dlg_w = dai->dlg_w; d_i[nelm].dlg_h = dai->dlg_h;
  d_i[nelm].v_min = dai->v_min; d_i[nelm].v_max = dai->v_max;
  d_i[nelm].dyna = 1; d_i[nelm].flags = dai->flags;
  d_i[nelm].name[0] = 0;
  strncat( d_i[nelm].name, dai->name, MAX_NAMELEN-1 );
  d_i[nelm].descr = dai->descr;
  d_i[nelm].listdata = dai->listdata;
  d_i[nelm].hval = hashVal( d_i[nelm].name );
  nelm++;
  d_i[nelm].tp = dtpEnd; ptrs[nelm]=0;
  return 0;
}

int TDataSet::del_obj( int n_ptrs )
{
  int i, *i_ob;
  double *d_ob; char *s_ob;
  TDataSet* ob;
  if( !allow_add || !d_i_alloc
       || n_ptrs >= nelm || n_ptrs < 0 ) return -1;
  if( d_i[n_ptrs].dyna == 0 ) return -2;
  switch( d_i[n_ptrs].tp ) {
    case dtpInt: i_ob = (int*)ptrs[n_ptrs];
                 delete i_ob; break;
    case dtpDbl: d_ob = (double*)ptrs[n_ptrs];
                 delete d_ob; break;
    case dtpStr: s_ob = (char*)ptrs[n_ptrs];
                 delete[] s_ob; break;
    case dtpObj: ob = static_cast<TDataSet*>( ptrs[n_ptrs] );
                 delete ob; break;
  };
  ptrs[n_ptrs] = 0;
  //delete[] d_i[n_ptrs].descr;
  //delete[] d_i[n_ptrs].listdata;
  for( i=n_ptrs; i<nelm-1; i++ ) {
    ptrs[i] = ptrs[i+1];
    d_i[i] = d_i[i+1];
  };
  ptrs[nelm] = 0; d_i[nelm].tp = dtpEnd; nelm--;
  return 0;
}

int TDataSet::isValidType( int /* a_tp*/, int /* a_subtp */ ) const
{
  return 1;
}

int TDataSet::isChildOf( const char *cname )
{
  const TClassInfo *ci, *pci;
  int l;
  l = strlen( cname );
  if( l < 1 || l >=MAX_NAMELEN ) return 0;
  if( strcmp( getClassName(), cname ) == 0 )
    return 1;
  ci = getClassInfo();    
  while( (pci = ci->parent_class) != 0 ) {
    if( strcmp( pci->className, cname ) == 0 )
      return 1;
    ci = pci;
  };
  return 0;
}

int TDataSet::isChildOf( int cid )
{
  const TClassInfo *ci, *pci;
  if( cid == getClassId() )
    return 1;
  ci = getClassInfo();        
  while( (pci = ci->parent_class) != 0 ) {
    if( pci->id == cid )
      return 1;
    ci = pci;
  };
  return 0;
}

// end of dataset.cpp


