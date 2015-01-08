/***************************************************************************
                          datacont.cpp  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : GPL (C) 2000-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/


#include "datacont.h"

const char* TDataContainer::helpstr = "<H1>TDataContainer</H1>\n"
 "Base class for all elements, which can hold \n"
 "unspecified at compile time elements";

STD_CLASSINFO( TDataContainer,clpContainer );


CTOR(TDataContainer,TDataSet)
{
}

TDataContainer::~TDataContainer()
{
}

DEFAULT_FUNCS_REG(TDataContainer)

