/***************************************************************************
                          elemfact.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.07.03
    copyright            : GPL (C) 2012-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "dataset.h"


// old example
//int TDataSetXXX::reg()
//{
//  static TDataSetXXX p { create, "" };
//  return EFACT.registerElemType( p );
//}


ElemFactory::ElemFactory()
{
}

ElemFactory& ElemFactory::theFactory()
{
  static ElemFactory instance;
  return instance;
}


TDataSet* ElemFactory::createElem( const QString &a_type, 
             const QString &ob_name, TDataSet *parent ) const
{
  MapStrClass::const_iterator i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBG2q( "WARN: fail to find class %s", a_type ); 
    return nullptr;
  }
  // check parent for name
  if( parent->getHolder(ob_name) ) {
    DBGx( "WARN: name \"%s\" exists in parent \"%s\"", 
	     qP( ob_name ), qP( parent->getFullName() ) );
    return nullptr;
  }
  
  TDataSet *ob =  i.value()->creator( parent );
  //  create holder for it (auto set name and owner)
  new HolderObj( ob, ob_name, ob_name, parent, 0, "", "dyn=1" );
  return ob;
}

HolderData* ElemFactory::createParam( const QString &a_type, 
       const QString &ob_name, TDataSet *parent  ) const
{
  MapStrHolder::const_iterator i = str_holder.find( a_type );
  if( i == str_holder.end() ) {
    DBG2q( "WARN: fail to find type", a_type ); 
    return nullptr;
  }
  // check parent for name
  if( parent->getHolder(ob_name) ) {
    DBGx( "WARN: name \"%s\" exists in parent %s", 
	     qP( ob_name ), qP(parent->getFullName()) );
    return nullptr;
  }
  
  HolderData *ho = i.value()->creator( ob_name, ob_name, parent, 0, "", "dyn=1" );
  return ho;
}

bool ElemFactory::registerElemType( const TClassInfo *cl_info )
{
  if( ! cl_info )
    return false;
  QString cl_name = L8B( cl_info->className );
  if( str_class.contains( cl_name ) ) {
    DBG2q( "ERR: reg: class already exists", cl_name ); 
    return false;
  }
  str_class.insert( cl_name, cl_info );
  //DBGx( "dbg:  registered \"%s\" %d", cl_info->className, cl_info->props ); 
  return true;
}

bool ElemFactory::unregisterElemType( const QString &a_type )
{
  MapStrClass::iterator i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBG2q( "ERR: fail to find class", a_type ); 
    return 0;
  }
  str_class.erase( i );
  return true;
}

bool ElemFactory::registerSimpleType( const HolderInfo *ho_info )
{
  if( ! ho_info )
    return false;
  QString tp_name = L8B( ho_info->name );
  if( str_holder.contains( tp_name ) ) {
    DBG2q( "WARN: type already exists", ho_info->name ); 
    return false;
  }
  str_holder.insert( tp_name, ho_info );
  return true;
}

const TClassInfo* ElemFactory::getInfo( const QString &a_type ) const
{
  MapStrClass::const_iterator i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBG2q( "ERR: fail to find class", a_type ); 
    return nullptr;
  }
  return i.value();
}

bool ElemFactory::isChildOf( const QString &cl, const QString &par_cl )
{
  auto i = str_class.find( cl );
  if( i == str_class.end() ) {
    DBG2q( "WARN:  fail to find class",	cl ); 
    return false;
  }
  const TClassInfo *ci = i.value();
  while( ci ) {
    if( par_cl == ci->className )
      return true;
    ci = ci->parent_class;
  }
  return false;
}

