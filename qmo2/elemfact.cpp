/***************************************************************************
                          elemfact.cpp  - widgets for DataDialog
                             -------------------
    begin                : 2012.07.03
    copyright            : GPL (C) 2012 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QStringBuilder>
#include "dataset.h"


// old example
//int TDataSetXXX::reg()
//{
//  static TDataSetXXX p { create, "" };
//  return ElemFactory::theFactory().registerElemType( p );
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
    qDebug( "ERR: create: fail to find class %s", qPrintable( a_type ) ); 
    return 0;
  }
  // check parent for name
  QObject *no = parent->findChild<QObject*>(ob_name);
  if( no != nullptr ) {
    qDebug( "ERR: create: name %s exists in parent", qPrintable( ob_name ) ); 
    return 0;
  }
  
  TDataSet *ob =  i.value()->creator( parent );
  //  create holder for it (auto set name and owner)
  new HolderObj( ob, ob_name, ob_name, parent, 0, "", "dyn=1" );
  return ob;
}


bool ElemFactory::registerElemType( const TClassInfo *cl_info )
{
  if( ! cl_info )
    return false;
  QString cl_name = L8B( cl_info->className );
  if( str_class.contains( cl_name ) ) {
    qDebug( "ERR: reg: class %s already exists", qPrintable( cl_name ) ); 
    return false;
  }
  str_class.insert( cl_name, cl_info );
  //qDebug( "dbg:  ElemFactory::registerElemType: registered \"%s\" %d",
  //    cl_info->className, cl_info->props ); 
  return true;
}

bool ElemFactory::unregisterElemType( const QString &a_type )
{
  MapStrClass::iterator i = str_class.find( a_type );
  if( i == str_class.end() ) {
    qDebug( "ERR: unr: fail to find class %s", qPrintable( a_type ) ); 
    return 0;
  }
  str_class.erase( i );
  return true;
}

const TClassInfo* ElemFactory::getInfo( const QString &a_type ) const
{
  MapStrClass::const_iterator i = str_class.find( a_type );
  if( i == str_class.end() ) {
    qDebug( "ERR: getInfo: fail to find class %s", qPrintable( a_type ) ); 
    return 0;
  }
  return i.value();
}


