/***************************************************************************
    elemfact.cpp  - widgets factory for DataDialog
                             -------------------
    begin                : 2012.07.03
    copyright            : GPL (C) 2012-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "dataset.h"
#include "miscfun.h"



ObjFactory::ObjFactory()
{
}

ObjFactory& ObjFactory::theFactory()
{
  static ObjFactory instance;
  return instance;
}


HolderData* ObjFactory::createObj( const QString &a_type,
             ARGS_CTOR_MIN ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    qWarning() << "fail to find class " << a_type << " for object " << obj_name << WHE;
    return nullptr;
  }

  // check for manual abstract elements
  if( i.value()->props & clpAbstract ) {
    qWarning() << "refuse to create object" << obj_name << " with abstract type " << a_type << WHE;
    return nullptr;
  }

  // check parent for name
  if( a_parent->getObj(obj_name) ) {
    qWarning() << "name " << obj_name << " exists in parent " <<  a_parent->getFullName() << WHE;
    return nullptr;
  }

  if( ! isGoodName( obj_name ) ) {
    qWarning() << " bad object name " << obj_name << " in parent " <<  a_parent->getFullName() << WHE;
    return nullptr;
  }

  HolderData *ob =  (i.value()->creator)( ARGS_CTOR_NAMES );
  if( !ob ) {
    qWarning() << "fail to create obj " << obj_name << " type "<< a_type << WHE;
    return nullptr;
  }
  return ob;
}


bool ObjFactory::registerObjType( const TClassInfo *cl_info )
{
  if( ! cl_info ) {
    return false;
  }
  const QString &cl_name = cl_info->className;
  if( str_class.contains( cl_name ) ) {
    qWarning() << "reg: class " << cl_name << " already exists" << WHE;
    return false;
  }
  str_class.insert( cl_name, cl_info );

  return true;
}


QStringList ObjFactory::goodTypeNames( const QString & allows, const QString &hidden_types ) const
{
  QStringList res;
  int mask = clpAbstract | clpSpecial;
  if( allows.contains( "+SPECIAL" ) ) {
    mask &= ~clpSpecial;
  }

  QStringList atp = allows.split(',');
  QStringList btp = hidden_types.split(',');

  for( auto i : str_class ) {
    if( i->props & mask ) {
      continue;
    }
    if( btp.contains( i->className ) ) {
      continue;
    }

    for( auto ptp : atp ) {
      if( ptp[0] == '+' ) { // ignore special names, like +SPECIAL
        continue;
      }
      if( isChildOf( i->className, ptp ) ) {
        res << i->className;
      }
    }

  }
  return res;
}


const TClassInfo* ObjFactory::getInfo( const QString &a_type ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    qWarning() << "fail to find class " << a_type << WHE;
    return nullptr;
  }
  return i.value();
}

bool ObjFactory::isChildOf( const QString &cl, const QString &par_cl ) const
{
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
      return true;
    }
    mob = mob->superClass();
  }
  return false;
}

