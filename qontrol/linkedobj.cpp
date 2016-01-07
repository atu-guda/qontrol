/***************************************************************************
                          linkedobj.h - object with link capabilities
                             -------------------
    begin                : 2015.10.24
    copyright            : GPL (C) 2015-2016 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include "linkedobj.h"
#include "miscfun.h"

using namespace std;


STD_CLASSINFO(LinkedObj,clpSpecial|clpPure);


const char* LinkedObj::helpstr =
 "<H1>LinkedObj</H1>\nThis is a base element for all linked with double elements\n"
 "Never to be used directly";

CTOR(LinkedObj,TDataSet)
{
}

LinkedObj::~LinkedObj()
{
}


// ----------------------------------



DEFAULT_FUNCS_REG(LinkedObj);




const double* LinkedObj::getDoublePtr( const QString &nm, int *lt,
              const LinkedObj **targ, int lev  ) const
{
  static int clt;
  int *plt = lt ? lt : &clt;
  if( nm.isEmpty() ) {
    *plt = LinkNone; return nullptr;
  }
  if( isIgnored() ) {
    *plt = LinkBad;
    return nullptr;
  }
  QString nmf = nm, first, rest;

  int idx;
  NameType nm_type = splitName( nmf, first, rest, idx );
  if( nm_type == badName ) {
    *plt = LinkBad;
    qWarning() << "bad source name " << nmf << NWHE;
    return nullptr;
  }

  HolderData *ho = getObj( first );

  if( !ho ) {
    *plt = LinkBad;
    // qWarning() << "elem" << first << "not found, nmf= " << nmf << NWHE;
    return nullptr;
  }

  if( nm_type == simpleName ) { // -- simple name ----- first only -----
    LinkedObj *ds= qobject_cast<LinkedObj*>(ho);
    if( ds ) {
      return ds->getDoublePtr( QSL("out0"), plt, targ, lev+1 ); // default output
    }

    HolderDouble *hod = qobject_cast<HolderDouble*>(ho);
    if( hod ) {
      *plt = ( lev == 1 ) ? LinkElm : LinkSpec;
      if( targ ) {  *targ = this;   }
      return hod->caddr();
    }

    auto *hoda = qobject_cast<HolderDoubleArray*>(ho); // array special case
    if( hoda ) {
      int na = hoda->arrSize();
      if( idx >= na ) {
        qWarning() << "Bad index " << idx << " while access to " << hoda->getFullName()
                   << " size " << na << NWHE;
        *plt = LinkBad;
        return nullptr;
      }
      *plt = ( lev == 1 ) ? LinkElm : LinkSpec;
      if( targ ) {  *targ = this;   }
      return & ( hoda->operator[](idx) );
    }

    return nullptr;
  } // -------------------- simple name end ----------------------------

  // both part of name exists
  LinkedObj *ds = qobject_cast<LinkedObj*>(ho);
  if( !ds ) {
    *plt = LinkBad;
    qWarning() << "Complex name " << nm << " is given for simple object "
               << ho->getFullName() << NWHE;
    return nullptr;
  }
  return ds->getDoublePtr( rest, plt, targ, lev+1 ); // pass to child

}


const double* LinkedObj::getSchemeDoublePtr( const QString &nm, int *lt,
        const LinkedObj **src_ob, int lev) const
{
  int clt = LinkNone;
  int *plt = ( lt ) ? lt : &clt;  // failsafe link

  if( isIgnored() || nm.isEmpty() ) {
    *plt = LinkNone;
    return nullptr;
  }

  LinkedObj *ds = qobject_cast<LinkedObj*>( par );  // parent-less object or root
  if( !ds ) {
    *plt = LinkBad;
    return nullptr;
  }
  return ds->getSchemeDoublePtr( nm, lt, src_ob, lev );
}

// not const - change param
double* LinkedObj::getDoublePrmPtr( const QString &nm, int *flg )
{
  if( nm.isEmpty() ) {
    return nullptr;
  }

  HolderDouble *hod = getObjT<HolderDouble*>( nm );

  if( !hod ) {
    // qWarning() << "fail to find name " << nm << NWHE;
    return 0;
  }

  if( flg ) {
    *flg = hod->getFlags();
  }
  return hod->addr();
}

bool LinkedObj::isIgnored() const
{
  int ign = 0;
  for( const HolderData *p = this; p; p=p->getParent() ) {
    p->getData( "ignored", &ign, false ); // not direct access, TMiso+... property
    if( ign ) {
      return true;
    }
  }
  return false;
}


void LinkedObj::do_structChanged()
{
  TDataSet::do_structChanged();
  inps.clear(); inps_a.clear(); inps_s.clear(); inps_ap.clear();

  for( auto in : TCHILD(InputAbstract*) ) {
    in->set_link();
    inps.append( in );

    int lt = in->getLinkType();
    if( ( lt == LinkElm || lt == LinkSpec ) && ! in->getOnlyFirst()  ) {
      inps_a.append( in );
    }

    InputSimple *in_s = qobject_cast<InputSimple*>( in );
    if( in_s ) {
      inps_s.append( in_s );
    }

    ParamDouble *in_p = qobject_cast<ParamDouble*>( in );
    if( in_p && (( lt == LinkElm || lt == LinkSpec )) ) {
      inps_ap.append( in_p );
      prm_will_mod += in_p->isFixparmNeed();
    }
  }
}

void LinkedObj::readInputs()
{
  if( needReadInputsRecurse ) {
    for( auto in : TCHILD(LinkedObj*) ) {
      in->readInputs();
    }
  }

  for( auto in : inps_a ) {
    in->readInput();
  }
  prm_mod += prm_will_mod;
}

void LinkedObj::readAllInputs()
{
  if( needReadInputsRecurse ) {
    for( auto in : TCHILD(LinkedObj*) ) {
      in->readAllInputs();
    }
  }

  for( auto in : inps  ) {
    in->readInput();
  }
  prm_mod += prm_will_mod;
}

// ------------------------------------ InputAbstract ---------
//
STD_CLASSINFO(InputAbstract,clpInput|clpSpecial|clpPure);


CTOR(InputAbstract,LinkedObj)
{
  if( getParm( QSL("props") ) == QSL("OBJ") ) {
    setParm( QSL("props"), QSL("OBJ,INPUT") );
  }
}

InputAbstract::~InputAbstract()
{
}

void InputAbstract::reset_dfl()
{
  LinkedObj::reset_dfl();
  QString s = getParm( QSL("def") );
  source = s;
}

void InputAbstract::do_post_set()
{
  // TODO: uncomment, commenrd only for debug
  // MORE: works much better with commenting
  // reportStructChanged(); // changed link means changes structure
}

void InputAbstract::do_structChanged()
{
  LinkedObj::do_structChanged();
  set_link();
}

void InputAbstract::set_link()
{
  direct_in = 0;
  p = &direct_in; src_obj = nullptr; linkType = LinkBad; srcObjName = QSL(":BAD:");
  if( isIgnored() || source.cval().isEmpty() ) {
    linkType = LinkNone; srcObjName = QSL(":NONE:"); return;
  }
  // if( source.cval() == QSL(":one") ) { // special local case TODO: remove
  //   direct_in = 1.0;
  //   linkType = LinkNone; srcObjName = QSL(":VALUE:"); return;
  // }

  bool ok = false;
  double v = source.cval().toDouble( &ok );
  if( ok ) {
    direct_in = v;
    linkType = LinkNone;  srcObjName = QSL("=") % QSN( v );
    return;
  }


  int lt;
  const LinkedObj *srct = nullptr;

  const double *cp = getSchemeDoublePtr( source, &lt, &srct, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;  src_obj = srct; linkType = lt;
    if( srct ) {
      srcObjName = srct->getFullName();
    } else {
      srcObjName = QSL("$");
    }
  } else {
    qWarning() << "ptr not found for " << source << NWHE;
  }

}

QVariant InputAbstract::dataObj( int col, int role ) const
{
  if( role == Qt::StatusTipRole && col < 2 ) { // used for button labels in dialogs

    QString s; // = source;
    int lt  = linkType;
    QChar ac;
    switch( lt ) {
      case LinkNone:
        ac = '=';
        break;
      case LinkElm:
        ac = QChar( 0x27BC ); // >>->
        if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
          ac = QChar( 0x2794 ); // ->
        }
        break;
      case LinkSpec:
        ac = QChar( 0x27A4 ); // >>
        if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
          ac = QChar( 0x27A0 ); // =>
        }
        break;
      case LinkBad:
        ac = QChar( 0x274C ); // X
        break;
      default:
        ac = '?';
        break;
    }

    s += ac;
    if( onlyFirst ) {
      s += QChar( 0x271D ); // +
    }
    // s += objectName();
    return s;
  }

  return LinkedObj::dataObj( col, role );
}


const char* InputAbstract::helpstr { "Abstract link" };

DEFAULT_FUNCS_REG(InputAbstract);

// ------------------------------------ InputSimple ---------
STD_CLASSINFO(InputSimple,clpInput|clpSpecial);

CTOR(InputSimple,InputAbstract)
{
}

InputSimple::~InputSimple()
{
}



void InputSimple::do_post_set()
{
  InputAbstract::do_post_set(); // report is here
}


void InputSimple::set_link()
{
  InputAbstract::set_link();
}


const char* InputSimple::helpstr { "Link to source of simple double data" };

DEFAULT_FUNCS_REG(InputSimple);


// ------------------------------------ ParamDouble ---------
STD_CLASSINFO(ParamDouble,clpInput|clpSpecial);

CTOR(ParamDouble,InputAbstract)
{
  need_fixparm = getParmInt( "fixparm", 0 );
}

ParamDouble::~ParamDouble()
{
}



void ParamDouble::do_post_set()
{
  InputAbstract::do_post_set(); // report is here
}


void ParamDouble::set_link()
{
  InputAbstract::set_link();
}


const char* ParamDouble::helpstr { "Double parameter" };

DEFAULT_FUNCS_REG(ParamDouble);



// ------------------------------------ InputParam ---------
STD_CLASSINFO(InputParam,clpInput|clpSpecial);

CTOR(InputParam,InputAbstract)
{
  line_color.setParm( "def", "red" );
  line_w.setParm( "def", "2" );
}

InputParam::~InputParam()
{
}


void InputParam::do_post_set()
{
  InputAbstract::do_post_set(); // report is here
}


void InputParam::set_link()
{
  InputAbstract::set_link();
  target_flag = 0;
  targ = &fake_target;
  if( isIgnored() || !par ) { // par seems to be InputParams, but may be dangling objects
    return;
  }

  LinkedObj *el = qobject_cast<LinkedObj*>( par->getParent() ); // grand par may by TMiso, but ...
  if( !el ) {
    return;
  }

  targ = el->getDoublePrmPtr( tparam, &target_flag );
  if( !targ ) {
    qWarning() << "fail to find target " << tparam <<  " for " << source << NWHE;
    // qWarning() << this->ls();
    targ = &fake_target;
  }
}

QVariant InputParam::dataObj( int col, int role ) const
{
  if( role == Qt::StatusTipRole && col < 2 ) { // used for button labels in dialogs
    QString s; // = source;
    QChar ac = QChar( 0x274C ); // X
    int lt = linkType;
    if( lt == LinkElm || lt == LinkSpec ) {
      ac = QChar( 0x27BC );     // >>->
      if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
        ac = QChar( 0x2794 );   // ->
      }
    }
    s += ac;

    // s += objectName() % "." % tparam.cval();
    if( targ == &fake_target ) {
      s += QChar( 0x274C ); // X
    }
    if( onlyFirst ) {
      s += QChar( 0x271D ); // +
    }
    return s;
  }

  return InputAbstract::dataObj( col, role );
}



const char* InputParam::helpstr { "Link from source to inner parameter" };

DEFAULT_FUNCS_REG(InputParam);


// end of linkedobj.cpp


