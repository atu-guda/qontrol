/***************************************************************************
                          root.cpp  -  TRootData implementation
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
 ***************************************************************************/

#include <cstring>
#include <iostream>
#include "rootdata.h"
using namespace std;

const char* TRootData::helpstr = "<H1>TRootData</H1>\n"
 "Root element for all elemsnts tree";


TClassInfo TRootData::class_info = 
 {  CLASS_ID_TRootData, "TRootData",  TRootData::create, 
    &TDataContainer::class_info, "" };


TRootData::TRootData( TDataSet* apar )
          :TDataContainer( apar )
{
  allow_add = 1; 
  n_reg = 0;
  regClass( &TDataSet::class_info );
  regClass( &TDataContainer::class_info );
  regClass( &TRootData::class_info );
  initHash();
}

TRootData::~TRootData()
{
  n_reg = 0;
}

int TRootData::getClassId(void) const 
{
  return CLASS_ID_TRootData; 
}

const char* TRootData::getClassName(void) const
{ 
  return "TRootData"; 
}

const TClassInfo* TRootData::getClassInfo(void) const
{
  return &class_info;
}

TDataSet* TRootData::create( TDataSet* apar )
{
  return new TRootData( apar );
}

const char *TRootData::getHelp(void) const
{
  return helpstr;
}



TDataSet* TRootData::createObj( int id, TDataSet* apar ) 
{
  int n;
  PFDataSet f;
  TDataSet* ob;
  n = findClass( id );
  if( n < 0 ) return 0;  // no class found
  f = cl_info[n].creator;
  if( f == 0 ) return 0;
  ob = (*f)( apar );
  return ob;
}

int TRootData::regClass( const TClassInfo *ci )
{
  int j;
  if( n_reg >= MAX_CLASSES )
    return -1;

  j = findClass( ci->id );
  if( j >= 0 ) return -1; // id busy
  j = findClass( ci->className );
  if( j >= 0 ) return -1; // name busy
  cl_info[n_reg].id = ci->id;
  strncpy( cl_info[n_reg].className, ci->className, MAX_NAMELEN );
  cl_info[n_reg].className[MAX_NAMELEN-1] = 0;
  cl_info[n_reg].creator = ci->creator;
  cl_info[n_reg].parent_class = ci->parent_class;
  cl_info[n_reg].helpstr = ci->helpstr;
  n_reg++;
  return n_reg;
}

int TRootData::findClass( int a_id ) const
{
  int i;
  for( i=0; i<n_reg; i++ ) {
    if( cl_info[i].id == a_id )
      return i;
  };
  return -1;
}

int TRootData::findClass( const char *name ) const
{
  int i;
  // uneffictive, but simple - will be rarely used
  for( i=0; i<n_reg; i++ ) {
    if( strcmp( cl_info[i].className, name ) == 0 )
      return i;
  };
  return -1;
}

int TRootData::saveDatas( ostream *os )
{
  int i;
  *os << "%!r root: known classes:\n";
  for( i=0; i<n_reg; i++ ) {
    *os << "%!c " << cl_info[i].id << ' ' << cl_info[i].className << '\n';
  };
  i = TDataContainer::saveDatas( os );
  *os << "\n# end of TRootData\n\n";
  return i;
}

int TRootData::getNClasses( int parent_id /* = -1*/ , 
		            int countParent /*= 0*/ ) const
{
  int i, n;
  if( parent_id <= 0 )
    return n_reg;
  n = 0;
  for( i=0; i<n_reg; i++ ) {
    if( isHisParent( cl_info[i].id, parent_id, countParent ) )
       n++;
  };
  return n;
}

int TRootData::isHisParent( int child_id, int parent_id, 
		            int countParent /*=0*/ ) const 
{
  int nc, np;
  const TClassInfo *ci;
  nc = findClass( child_id );
  if( nc < 0 ) return 0;
  np = findClass( parent_id );
  if( np < 0 ) return 0;
  if( nc == np ) {
    return ( countParent != 0 )	? 1 : 0;  
  };
  ci = &cl_info[nc];
  while( ci != 0 ) {
    if( ci->id == parent_id )
      return 1;
    ci = ci->parent_class;
  };
  return 0;
}

const TClassInfo* TRootData::classInfo( int id ) const
{
  int n;
  n = findClass( id );
  if( n < 0 )
    return 0;
  return &cl_info[n];
}

const TClassInfo* TRootData::classInfoByNum( int n ) const
{
  if( n < 0 || n >= n_reg )
    return 0;
  return &cl_info[n];
}

// end of rootdata.cpp


