/***************************************************************************
                          root.cpp  -  TRootData implementation
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "rootdata.h"
#include "tmodel.h"
#include "labodoc.h"

using namespace std;

const char* TRootData::helpstr = "<H1>TRootData</H1>\n"
 "Root element for all elemsnts tree";

STD_CLASSINFO(TRootData,clpContainer|clpSpecial );


CTOR(TRootData,TDataSet)
{
  flags |= efImmutable;
  allowed_types = "TModel";
  TModel *model = addObjT<TModel>( "model" );
  model->setImmutable(); // TODO: need check?

}

TRootData::~TRootData()
{
}

QString TRootData::getFilePath() const
{
  if( !doc ) { return QString(); };
  return doc->getFilePath();
}

QString TRootData::getFileBase() const
{
  if( !doc ) { return QString(); };
  return doc->getFileBase();
}

DEFAULT_FUNCS_REG(TRootData)

// end of rootdata.cpp


