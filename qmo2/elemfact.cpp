/***************************************************************************
                          elemfact.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.07.03
    copyright            : GPL (C) 2012-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "dataset.h"



ElemFactory::ElemFactory()
{
}

ElemFactory& ElemFactory::theFactory()
{
  static ElemFactory instance;
  return instance;
}


HolderData* ElemFactory::createElem( const QString &a_type,
             ARGS_CTOR_MIN ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBGx( "WARN: fail to find class \"%s\" for object \"%s\"",
          qP(a_type), qP(obj_name) );
    return nullptr;
  }

  // check for manual abstract elements
  if( i.value()->props & clpPure ) {
    DBGx( "WARN: refuse to create object \"%s\" with abstract type \"%s\"",
          qP(obj_name), qP(a_type) );
    return nullptr;
  }

  // check parent for name
  if( a_parent->getElem(obj_name) ) {
    DBGx( "WARN: name \"%s\" exists in parent \"%s\"",
          qP( obj_name ), qP( a_parent->getFullName() ) );
    return nullptr;
  }

  HolderData *ob =  (i.value()->creator)( ARGS_CTOR_NAMES );
  if( !ob ) {
    DBGx( "err: fail to create obj \"%s\" type \"%s\"", qP(obj_name), qP(a_type) );
    return nullptr;
  }
  return ob;
}


bool ElemFactory::registerElemType( const TClassInfo *cl_info )
{
  if( ! cl_info ) {
    return false;
  }
  QString cl_name = L8B( cl_info->className );
  if( str_class.contains( cl_name ) ) {
    DBGx( "ERR: reg: class \"%s\" already exists", qP(cl_name) );
    return false;
  }
  str_class.insert( cl_name, cl_info );

  int props = cl_info->props;
  if( ( props & clpData ) && ! ( props & clpPure ) ) {
    param_names << cl_name;
  }
  //DBGx( "dbg:  registered \"%s\" %d", cl_info->className, cl_info->props );
  return true;
}


QStringList ElemFactory::goodTypeNames( const QString & allows,
             bool no_obj, bool no_param ) const
{
  QStringList res;
  int mask = clpPure | clpSpecial;
  if( allows.contains( "+SPECIAL" ) ) {
    mask &= ~clpSpecial;
  }

  QStringList atp = allows.split(',');
  for( auto i : str_class ) {
    if( i->props & mask ) {
      continue;
    }

    for( auto ptp : atp ) {
      if( ptp[0] == '+' ) { // ignore special names, like +SPECIAL
        continue;
      }
      if( no_obj && isChildOf( i->className, "TDataSet" ) ) {
        continue;
      }
      if( no_param && ( i->props & clpData ) ) {
        continue;
      }
      if( isChildOf( i->className, ptp ) ) {
        res << i->className;
      }
    }

  }
  return res;
}


const TClassInfo* ElemFactory::getInfo( const QString &a_type ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBG2q( "ERR: fail to find class", a_type );
    return nullptr;
  }
  return i.value();
}

bool ElemFactory::isChildOf( const QString &cl, const QString &par_cl ) const
{
  // DBGx( "dbg: test: is \"%s\" a child of \"%s\"", qP(cl), qP(par_cl) );
  if( cl == par_cl ) {
    return true;
  }

  auto ic = str_class.find( cl );
  if( ic == str_class.end() ) {
    return false;
  }
  auto icp = str_class.find( par_cl );
  if( icp == str_class.end() ) {
    return false;
  }

  const QMetaObject *mob = ic.value()->meta;
  while( mob ) {
    if( par_cl == mob->className() ) {
      // DBGx( "dbg: \"%s\" is child of \"%s\"", qP(cl), qP(par_cl) );
      return true;
    }
    mob = mob->superClass();
  }
  return false;
}

