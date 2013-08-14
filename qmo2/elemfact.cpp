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


HolderData* ElemFactory::createElem( const QString &a_type, 
             ARGS_CTOR_MIN ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBGx( "WARN: fail to find class \"%s\" for object \"%s\"", 
          qP(a_type), qP(obj_name) ); 
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
  if( ! cl_info )
    return false;
  QString cl_name = L8B( cl_info->className );
  if( str_class.contains( cl_name ) ) {
    DBG2q( "ERR: reg: class already exists", cl_name ); 
    return false;
  }
  str_class.insert( cl_name, cl_info );
  if( cl_info->props & clpData ) {
    param_names << cl_name; 
  }
  //DBGx( "dbg:  registered \"%s\" %d", cl_info->className, cl_info->props ); 
  return true;
}

//bool ElemFactory::unregisterElemType( const QString &a_type )
//{
//  auto i = str_class.find( a_type );
//  if( i == str_class.end() ) {
//    DBG2q( "ERR: fail to find class", a_type ); 
//    return 0;
//  }
//  str_class.erase( i );
//  return true;
//}

const TClassInfo* ElemFactory::getInfo( const QString &a_type ) const
{
  auto i = str_class.find( a_type );
  if( i == str_class.end() ) {
    DBG2q( "ERR: fail to find class", a_type ); 
    return nullptr;
  }
  return i.value();
}


