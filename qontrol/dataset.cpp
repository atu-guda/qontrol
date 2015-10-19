/***************************************************************************
                          dataset.cpp  -  description
                             -------------------
    begin                : Wed Mar 29 2000
    copyright            : GPL (C) 2000-2015 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

#include <QColor>
#include <QStandardItemModel>

#include "miscfun.h"
#include "dataset.h"

using namespace std;


#define ERR_ABS "ERR: abstract class function called"

// ================================================================
// ---------------- HolderData .... ----------------------
STD_CLASSINFO(HolderData,clpSpecial|clpPure);

HolderData::HolderData( ARGS_CTOR_MIN )
     :QAbstractItemModel( a_parent ),
      flags(a_flags),
      par(a_parent)
{
  setObjectName( obj_name );

  if( a_v_name.isNull() )  {
    setParm( "vis_name", obj_name );
  } else {
    setParm( "vis_name", a_v_name );
  }
  setParm( "descr", a_descr );
  setParm( "extra", a_extra );
  extraToParm();
}

HolderData::~HolderData()
{
}

HolderData* HolderData::create( const QString &, HolderData *,
         int, const QString &, const QString &, const QString & )
{
  qCritical() << "Attempt to create abstract class " << WHE;
  return nullptr;
}

// QAbstractItemModel part
int HolderData::columnCount( const QModelIndex & /*par*/ ) const
{
  return 4;
}

// //* function to debug model indexes
// QString idx2s( const QModelIndex &idx )
// {
//   QString r = "[ ";
//   if( idx.isValid() ) {
//     r += "V ";
//   } else {
//     r += "X ";
//   }
//   void *p = idx.internalPointer();
//   r += QSN( idx.row() ) + " " + QSN( idx.column() ) + " " + QSN( (uint64_t)(p) );
//   if( p ) {
//     const HolderData *ho = static_cast<HolderData*>(p);
//     r += "=\"" + ho->getFullName() + "\"";
//   }
//
//   r += " ]";
//   return r;
// }

int HolderData::rowCount( const QModelIndex &par ) const
{
  int sz = size();
  if( ! par.isValid() ) { // invalid: assume self (examples: root)
    return sz;
  }

  HolderData *po = static_cast<HolderData*>( par.internalPointer() );
  if( !po ) {
    return 0;
  }
  sz = po->size();
  return sz;
}

QVariant HolderData::data( const QModelIndex &idx, int role ) const
{
  if( !idx.isValid() ) {
    return QVariant();
  }
  int c = idx.column();

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    return QVariant();
  }

  return ds->dataObj( c, role );
}

QVariant HolderData::dataObj( int col, int role ) const
{
  // no switch: too complex
  if( role == Qt::DisplayRole ) {
    QString s;
    s = QString( "c " ) + QSN( col ); // fallback value
    switch( col ) {
      case 0:
        s = objectName(); break;
      case 1:
        s = getType(); break;
      case 2:
        if( size() == 0 ) {
          getData( "", s );
          int ls;
          if( (ls = s.size()) > 80 ) {
            s.truncate( 72 );
            s += QSL( "... [" ) + QSN( ls ) + QSL( "]" );
          }
        } else {
          s = QSL("[") + QSN( size() ) + QSL("]");
        }
        break;
      case 3:
        s = getStateStr();
        break;
      default:
        break;
    }
    return s;
  }

  if( role == Qt::CheckStateRole ) {
    if( !show_active || col != 0 ) {
      return QVariant();
    }
    return ( par  &&  par->getActiveObj() == this );
  }

  if( role == Qt::DecorationRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    return getIcon();
  }

  if( role == Qt::WhatsThisRole ) {
    if( col != 0 ) {
      return QVariant();
    }
    QString whats = getParm("descr");
    if( whats.isEmpty() ) {
      whats = "Object \"" + objectName() % "\". type: " % getType();
    }
    return whats;
  }

  if( role == Qt::StatusTipRole ) { // used for button labels in dialogs
    if( col != 0 ) {
      return QVariant();
    }
    QString s = objectName() % " (" % getType() % ") " % getStateStr();
    return s;
  }

  return QVariant();
}


QModelIndex HolderData::index( int row, int column, const QModelIndex &par ) const
{

  if( ! hasIndex( row, column, par ) ) {
    return QModelIndex();
  }

  const HolderData *d_par = static_cast<HolderData*>( par.internalPointer() );
  if( !d_par ) {
    d_par = this;
  }

  const HolderData *d_t = qobject_cast<HolderData*>( d_par->getObj( row ) );
  if( ! d_t ) {
    return QModelIndex();
  }

  return createIndex( row, column, const_cast<HolderData*>(d_t) );
}

QModelIndex HolderData::parent( const QModelIndex & idx ) const
{
  if( ! idx.isValid() ) {
    return QModelIndex();
  }

  HolderData *ds = static_cast<HolderData*>( idx.internalPointer() );
  if( !ds ) {
    return QModelIndex();
  }

  HolderData *ds_p = ds->getParent();
  if( !ds_p ) {
    return QModelIndex();
  }
  if( ds_p == this ) {
    return QModelIndex();
  }

  return createIndex( ds_p->getMyIndexInParent(), 0, ds_p );
}

// my part

int HolderData::indexOfHolder( const HolderData *ho ) const
{
  // children holds <QObject*>, but indexOf dont touch *it
  return children().indexOf( const_cast<HolderData*>(ho) );
}


QString HolderData::getStateStr() const
{
  QString s = modificationChar[ getModified() ];
  if( dyn ) { s += "."; };
  s += getStateString( state );
  s += " ";
  s += QSNX( flags );
  return s;
}

void HolderData::setModified()
{
  // qWarning() << "SET: " << NWHE;
  if( modified & modifManual ) {  // do not repeat on alread modified
    return;
  }
  modified |= modifManual;
  if( flags & efNoSave ) {    // ignore shadow and so on
    return;
  }
  if( par != nullptr ) {
    par->setModified();
  }
}

void HolderData::setUnModified()
{
  modified = 0;
  for( auto ho : TCHILD(HolderData*) ) {
    ho->setUnModified();
  }
}

void HolderData::reset()
{
  for( auto ho : TCHILD(HolderData*) ) {
    ho->reset();
  }
  do_reset();
}


bool HolderData::isChildOf( const QString &cname ) const
{
  const QMetaObject *mob = metaObject();
  while( mob ) {
    if( cname == mob->className() ) {
      return true;
    }
    mob = mob->superClass();
  }
  return false;
}

QStringList HolderData::elemNames() const
{
  QStringList cl;
  for( const auto c : children() ) {
    cl << c->objectName();
  }
  return cl;
}

QString HolderData::ls() const
{
  QString r;
  r = getFullName() + " : " + getType() + " dyn: " + QSN(dyn)
    + " flags: 0x" + QSNX(flags) + " ptr: " + QSNX( (unsigned long)(this));
  if( ! isObject() ) {
    r += " = \"" + toString() + "\"";
  }
  r += "\n;------------------------------------\n";
  int n_el = 0;
  for( const auto c : children() ) {
    r += c->objectName() + ' ';
    ++n_el;
    const HolderData *ho = qobject_cast<HolderData*>( c );
    if( ho ) {
      r += ho->getType();
      if( ! ho->isObject() ) {
        QString vs = ho->toString();
        vs.truncate( 80 );
        r += " = \"" + vs + "\"";
      }
    }
    r += '\n';
  }
  r += "\n;------------------------------------";

  r += "\n;n_el = " + QSN(n_el) + "\n;Params:\n";
  for( auto i= parms.constBegin(); i != parms.constEnd(); ++i ) {
    r += i.key() + " = \"" + i.value() + "\"\n";
  }
  return r;
}

QString HolderData::lsf() const
{
  QString r;
  const QMetaObject *mo = metaObject();
  int nf = mo->methodCount();
  for( int i=0; i<nf; ++i ) {
    QMetaMethod mm = mo->method( i );
    if( mm.isValid()  &&  mm.access() == QMetaMethod::Access::Public
        && ( mm.methodType() == QMetaMethod::MethodType::Method
          || mm.methodType() == QMetaMethod::MethodType::Slot ) ) {
      r += mm.methodSignature() + '\n';
    }
  }
  return r;
}

HolderData* HolderData::getObj( int i ) const
{
  if( i < size() && i >=0 ) {
    return qobject_cast<HolderData*>(children().at(i));
  }
  return nullptr;
}


HolderData* HolderData::getObj( const QString &oname ) const
{
  QString first, rest;
  int idx = 0;

  NameType nt = splitName( oname, first, rest, idx );

  if( nt == badName ) {
    qWarning() << "bad name" << oname << NWHE;
    return nullptr;
  }

  HolderData *ho =  findChild<HolderData*>( first, Qt::FindDirectChildrenOnly );
  if( !ho ) { // need, as here both simpleName and complexName
    // qWarning() << "object" << oname << "not found " << NWHE; // very noisy!
    return nullptr;
  }

  if( nt == simpleName ) {
    return ho;
  }

  if( nt == complexName ) {
    return ho->getObj( rest );
  }

  qWarning() << "unknown name type: " << nt << NWHE;
  return nullptr;
}

void HolderData::setParm( const QString &name, const QString &value )
{
  parms[name] = value;
}


QString HolderData::getParm( const QString &name ) const
{
  if( parms.contains( name ) )
    return parms[name];
  return QString();
}

bool HolderData::setDatas( const QString &datas )
{
  bool was_set = false;
  QStringList sl = datas.split( "\n", QString::SkipEmptyParts );
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );

  for( const auto &s : sl ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      was_set = setData( nm, val ) || was_set; // Order!
    } else {
      qWarning() << "bad param string part: " << s << NWHE;
    }
  }
  return was_set;
}

bool HolderData::isObject( const QString & cl_name ) const
{
  QString cn = cl_name;
  if( cn.isEmpty() ) {
    cn = QSL( "TDataSet" );
  }

  return isChildOf( cn );
}

QString HolderData::hintName( const QString &tp, const QString &nm_start ) const
{
  QString nm = nm_start;

  if( nm.isEmpty() ) {
    nm = "xx_";
    const TClassInfo *ci = EFACT.getInfo( tp );
    if( ci ) {
      const QMetaObject *mci = ci->meta;
      int ci_idx = mci->indexOfClassInfo( "nameHintBase" );
      if( ci_idx >= 0 ) {
        nm = mci->classInfo( ci_idx ).value();
      }
    }
  }

  //             nameDIGITS
  QRegExp re( "^([_a-zA-Z][_a-zA-Z0-9]*[_a-zA-Z])([0-9]+)$" );
  QString bname = nm;
  int ne = 0;

  if( re.indexIn( nm ) != -1 ) {
    bname  = re.cap( 1 );
    ne     = re.cap( 2 ).toInt();
    // qWarning() << "bname= " << bname << " ne " << ne << NWHE;
  }

  int neo = countObjsOfType( tp, bname );
  if( neo > ne ) {
    ne = neo;
  }

  for( int i=ne; i<ne+1000; ++i ) {
    nm = bname + QSN( i );
    if( !getObj( nm ) ) {
      return nm;
    }
  }

  return bname + QSL(" _xx_");
}

int HolderData::countObjsOfType( const QString &tp, const QString &nm_start ) const
{
  int n_el = 0, l_nm = nm_start.size();
  for( auto ho : TCHILD(HolderData*) ) {
    if( ho->isChildOf( tp ) ) {
      if( l_nm < 1 || ho->objectName().startsWith( nm_start ) ) {
        ++n_el;
      }
    }
  }
  return n_el;
}

void HolderData::extraToParm()
{
  QRegExp re( R"(^([_a-zA-Z][_a-zA-Z0-9]*)\s*=(.+)$)" );
  QStringList el = getParm("extra").split( "\n", QString::SkipEmptyParts );

  for( QString &s : el ) {
    if( s.isEmpty() ) {
      continue;
    }

    if( re.indexIn( s ) != -1 ) {
      QString nm  = re.cap(1);
      QString val = re.cap(2);
      setParm( nm, val );
    } else {
      qWarning() << "bad extra string part: " <<  s << NWHE;
    }
  }
  if( getParm("dyn") == "1" ) {
    dyn = 1;
  }
}


QString HolderData::getTypeV() const // = 0;
{
  return "None";
}


void HolderData::reportStructChanged()
{
  HolderData *p = this, *np;
  while( (np = p->getParent()) != nullptr ) { // find root
    p = np;
  }
  // p is root now
  p->handleStructChanged();
}


void HolderData::handleStructChanged()
{
  if( updSuspended ) {
    return;
  }

  for( auto ds : TCHILD(HolderData*) ) {
    ds->handleStructChanged();
  }

  do_structChanged(); // functions to override


  if( ! par ) { // root signals about changes to world? only root?
    emit sigStructChanged();
    beginResetModel(); // ???
    endResetModel();
  }
}

void HolderData::do_structChanged()
{
}

QIcon HolderData::getIcon() const
{
  QString iconName = QString( ":icons/elm_" ) + getType().toLower() + ".png";
  QIcon el_ico( iconName );
  return el_ico;
}

HolderData* HolderData::addObjP( const QString &cl_name, const QString &ob_name, bool ignoreMod )
{

  HolderData *el = getObj( ob_name );
  if( el != nullptr ) {
    qWarning() <<  "name " << ob_name << el->getType() << " exist in " << NWHE;
    return nullptr;
  }
  if( ! isValidType( cl_name ) ) {
    qWarning() <<  "type " << cl_name <<  " for " << ob_name
               << " not allowed in " << NWHE;
    return nullptr;
  }
  beginResetModel();

  HolderData *ob = EFACT.createObj( cl_name, ob_name, this );
  if( !ob ) {
    return nullptr;
  }
  if( !ignoreMod ) {
    setModified();
  }
  endResetModel();
  reportStructChanged();

  return ob;
}

bool HolderData::addObj( const QString &cl_name, const QString &ob_name )
{
  HolderData *ho = addObjP( cl_name, ob_name );
  return ( ho != nullptr );
}

bool HolderData::addObjDatas( const QString &cl_name, const QString &ob_name,
     const QString &datas )
{
  HolderData *ho = addObjP( cl_name, ob_name );
  if( ! ho ) {
    return false;
  }

  ho->setDatas( datas );
  setModified();

  return true;
}

int HolderData::delObj( const QString &ob_name, bool ignoreMod )
{

  HolderData *ho = getObj( ob_name );
  if( !ho ) {
    qWarning() << "not found element " << ob_name << " in " << NWHE;
    return 0;
  }
  if( ! ho->isDyn() ) {
    qWarning() << "object " << ob_name << " is not created dynamicaly in " << getFullName() << NWHE;
    return 0;
  }
  if( ho->getFlags() & efImmutable ) {
    qWarning() << "object " << ob_name << " is Immutable in " << getFullName() << NWHE;
    return 0;
  }

  HolderData *act_obj = getActiveObj();
  QString act_name;
  if( act_obj ) {
    act_name = act_obj->objectName();
  }

  beginResetModel();

  delete ho; // auto remove object and from parent lists
  if( !act_name.isEmpty() ) {
    setActiveObj( act_name );
  }

  if( !ignoreMod ) {
    setModified();
  }
  endResetModel();

  reportStructChanged();
  return 1;
}

int HolderData::renameObj( const QString &ob_name, const QString &new_name )
{
  if( ! isGoodName( new_name ) ) {
    qWarning() << "bad name " << new_name << " to rename" << NWHE;
    return 0;
  }
  HolderData *ho = getObj( ob_name );
  if( !ho ) {
    qWarning() << "not found element " << ob_name << " to rename" << NWHE;
    return 0;
  }
  if( ho->getFlags() & efImmutable ) {
    qWarning() << "element " << ob_name << " is Immutable to rename" << NWHE;
    return 0;
  }

  if( getObj( new_name ) ) {
    qWarning() << "element " << ob_name << " is already exists " << NWHE;
    return 0;
  }
  if( ! ho->isDyn() ) {
    qWarning() << "element " << ob_name << " is not created dynamicaly" << NWHE;
    return 0;
  }
  ho->setObjectName( new_name );
  setModified();

  reportStructChanged();
  return 1;
}

bool HolderData::cloneObj( const QString &old_name, const QString &new_name )
{
  HolderData *old_obj = getObj( old_name );
  if( !old_obj ) {
    qWarning() << " old plot not exist: " << old_name << NWHE;
    return 0;
  }

  QString tp = old_obj->getType();
  QString s = old_obj->toString();

  HolderData *new_obj = addObjP( tp, new_name );
  if( !new_obj ) {
    qWarning() << "fail to create new object" << new_name << " type " << tp << NWHE;
    return 0;
  }
  return new_obj->fromString( s );
}



void HolderData::setActiveIdx( int i )
{
  active_idx = i;
  if( par ) {
    QModelIndex mi1 = createIndex( 0, 0, par );
    QModelIndex mi2 = createIndex( par->size()-1, 0, par );
    emit dataChanged( mi1, mi2 );
  }
};

bool HolderData::setActiveObj( const QString &nm )
{
  int a_idx = -1; HolderData *e = getObj( nm );
  if( e ) {
    a_idx = indexOfHolder( e );
  }
  setActiveIdx( a_idx );
  return ( a_idx >= 0 );
};



int HolderData::isValidType(  const QString &cl_name  ) const
{
  const TClassInfo *ci = EFACT.getInfo( cl_name );
  if( ! ci ) {
    return false;
  }
  if( ci->props & clpPure ) {
    return false;
  }

  QStringList atp = QString(allowTypes()).split(',');
  for( auto c : atp ) {
    if( EFACT.isChildOf( cl_name, c ) ) {
      return true;
    }
  }
  return false;
}


QDomElement HolderData::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "param" );
  de.setAttribute( "name", objectName() );
  if( dyn ) {
    de.setAttribute( "dyn", "1" );
    de.setAttribute( "otype", getType() );
    for( auto p = parms.cbegin(); p != parms.cend(); ++p ) {
      de.setAttribute( "prm_" + p.key(), p.value() );
    }
  }
  QDomText tn = dd.createTextNode( toString() );
  de.appendChild( tn );
  return de;
}

bool HolderData::fromDom( QDomElement &de, QString &errstr )
{
  auto old_updSuspended = updSuspended;
  updSuspended = true;
  bool ok = fromDom_real( de, errstr );

  updSuspended = old_updSuspended;
  return ok;
}

bool HolderData::fromDom_real( QDomElement &de, QString &errstr )
{
  for( QDomNode no = de.firstChild(); !no.isNull() ; no = no.nextSibling() ) {

    if ( ! no.isElement() ) {
      continue;
    }

    QDomElement ee = no.toElement();
    QString elname = ee.attribute( "name" );
    QString tagname = ee.tagName();

    if( tagname == "obj" ) {  // ------------------------------- object
      QString cl_name = ee.attribute("otype");
      if( cl_name.isEmpty() ) {
        errstr = QString( "err: element \"%1\" without type" ).arg(elname);
        qWarning() << errstr << NWHE;
        return false;
      }
      HolderData *ho = getObj( elname );
      if( ho && ! ho->isObject() ) {
        errstr = QString("err: read elem \"%1\" failed. "
            "required: \"%2\" but have \"%3\" in \"%4\"" )
                .arg(elname).arg(tagname).arg(ho->getType()).arg( getFullName() );
        qWarning() << errstr << NWHE;
        return false;
      }
      if( !ho ) { // name not found
        if( ! addObj( cl_name, elname ) ) {
          errstr = QString("TDataSet::fromDom: fail to create obj %1 %2 ")
                   .arg(cl_name).arg(elname);
          qWarning() << errstr << NWHE;
          continue; // for conversion: ignore unknown
          // return false;
        }
      }

      TDataSet* ob = getObjT<TDataSet*>( elname );
      if( !ob ) {
        errstr = QString("TDataSet::fromDom: fail to find created obj %1 %2 in %3")
                 .arg(cl_name).arg(elname).arg( objectName() );
        qWarning() << errstr << NWHE;
        return false;
      }

      if( ! ob->fromDom( ee, errstr ) ) {
        return false;
      }

    } else if( tagname == "param" ) {  // ---------------- simple param
      QString tp_name = ee.attribute("otype");
      HolderData *ho = getObj( elname );
      if( ho && ho->isObject() ) {
        errstr = QString("TDataSet::fromDom: param \"%1\" is an object type \"%2\" ")
                 .arg(elname).arg(ho->getType());
        qWarning() << errstr << NWHE;
        return false;
      }
      if( !ho ) {
        ho =  addObjP( tp_name, elname );
        if( !ho  ) {
          errstr = QString("TDataSet::fromDom: fail to create param \"%1\" in \"%2\" ")
                   .arg(tp_name).arg(elname);
          qWarning() << errstr << NWHE;
          // return false;
          continue; // ignore unused params
        }
      }
      ho->fromString( getDomText(ee) );

      if( ho->isDyn() ) { // restore params
        // qDebug() << "dyn param:" << ho->objectName() <<  NWHE;
        auto attrs = ee.attributes();
        int asz = attrs.size();
        for( int i=0; i<asz; ++i ) {
          QDomNode dann = attrs.item(i);
          QDomAttr dattr = dann.toAttr();
          if( dattr.isNull() ) { continue; }
          QString attr_name = dattr.name();
          // qDebug() << "dyn attr: " <<  attr_name << " to " << dattr.value() << NWHE;
          if( ! attr_name.startsWith( "prm_" ) ) { continue; } // only special names
          attr_name.remove( 0, 4 ); // remove "prm_";
          ho->setParm( attr_name, dattr.value() );
          // qDebug() << "set dyn attr: " <<  attr_name << " to " << dattr.value() << NWHE;
        }
      }

    } else { // ----------- unknown element
      errstr = QString("TDataSet::fromDom: bad element %1 %2 ")
               .arg(tagname).arg(elname);
      qWarning() << errstr << NWHE;
      return false;
    }
  }
  post_set();
  reportStructChanged();

  return true;
}

int HolderData::getMyIndexInParent() const
{
  if( !par ) {
    return -1;
  }
  return par->indexOfHolder( this );
}

QString HolderData::getFullName() const
{
  const HolderData *cob;
  QString res = objectName();
  QString tn;
  cob = par;
  while( cob != 0 ) {
    tn = cob->objectName();
    tn += '.';
    res.prepend( tn );
    cob = cob->par;
  };
  return res;
}

bool HolderData::getData( const QString &nm, QVariant &da, bool er ) const
{
  if( nm.isEmpty() ) {
    da = get();
    return true;
  }

  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    qWarning() << "bad target name " <<  nm << NWHE;
    return false;
  }

  HolderData *ho = getObj( first );
  if( !ho ) {
    if( er ) {
      qWarning() << "fail to find name " << first << NWHE;
    }
    return false;
  }
  if( nm_type == simpleName ) { // first only
    da = ho->get( idx ); // TODO: check for simple data?
    return true;
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType  ||  ! ho->isObject()  ) {
    qWarning() << "compound name required. first: " << first
               << " rest:" << rest << " tp: " << (int)(ho->getTp()) << NWHE;
    return false;
  }

  return ho->getData( rest, da, er );
}


bool HolderData::getData( const QString &nm, int *da, bool er ) const
{
  if( !da ) {
    return false;
  }
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  *da = vda.toInt();
  return true;
}

bool HolderData::getData( const QString &nm, double *da, bool er ) const
{
  if( !da ) {
    return false;
  }
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  *da = vda.toDouble();
  return 1;
}


bool HolderData::getData( const QString &nm, QString &da, bool er ) const
{
  QVariant vda;
  bool rc = getData( nm, vda, er );
  if( ! rc ) {
    return false;
  }
  da = vda.toString();
  return true;
}


int HolderData::getDataD( const QString &nm, int dfl ) const
{
  int r = dfl;
  getData( nm, &r );
  return r;
}

double HolderData::getDataD( const QString &nm, double dfl ) const
{
  double r = dfl;
  getData( nm, &r );
  return r;
}

QString HolderData::getDataD( const QString &nm, const QString &dfl ) const
{
  QString r = dfl;
  getData( nm, r );
  return r;
}

QColor HolderData::getDataD( const QString &nm, const QColor &dfl ) const
{
  QColor r = dfl;
  int ri = 0;
  if( getData( nm,  &ri ) ) {
    r = QColor( QRgb( ri ) );
  }
  return QColor( QRgb( ri ) );
}



bool HolderData::setData( const QString &nm, const QVariant &da )
{
  if( nm.isEmpty() ) {
    return set( da );
  }

  QString first, rest;
  int idx;
  NameType nm_type = splitName( nm, first, rest, idx );
  if( nm_type == badName ) {
    qWarning() << "bad target name " << nm << NWHE;
    return false;
  }

  HolderData *ho = getObj( first );
  if( !ho ) {
    qWarning() << "fail to nind name " << first << NWHE;
    return false;
  }
  if( nm_type == simpleName ) { // first only
    return ho->set( da, idx );
  }

  // both part of name exists
  if( ho->getTp() != QVariant::UserType || ! ho->isObject() ) {
    qWarning() << "compound name required. first: " << first
               << " rest:" << rest << " tp: " << (int)(ho->getTp()) << NWHE;
    return false;
  }

  return ho->setData( rest, da );
}

bool HolderData::setD( const QString &nm, int da )
{
  return setData( nm, QVariant( da ) );
}

bool HolderData::setD( const QString &nm, double da )
{
  return setData( nm, QVariant( da ) );
}

bool HolderData::setD( const QString &nm, const QString &da )
{
  return setData( nm, QVariant( da ) );
}


bool HolderData::getUpData( const QString &nm, int *da ) const
{
  if( !da ) {
    return false;
  }

  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}


bool HolderData::getUpData( const QString &nm, double *da ) const
{
  if( !da ) {
    return false;
  }

  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}

bool HolderData::getUpData( const QString &nm, QString &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}

bool HolderData::getUpData( const QString &nm, QVariant &da ) const
{
  for( HolderData *p = par; p; p=p->par ) {
    if( p->getData( nm, da, false ) ) {
      return true;
    }
  }
  qWarning() << "fail to find UP name " << nm << NWHE;
  return false;
}



QStringList HolderData::getEnumStrings( const QString &enum_name ) const
{
  QStringList r;
  const QMetaObject *mci = metaObject();
  if( ! mci ) {
    return r;
  };
  int idx = mci->indexOfEnumerator( enum_name.toLocal8Bit().data() );
  if( idx < 0 ) {
    return r;
  }
  QMetaEnum me = mci->enumerator( idx );
  if( !me.isValid() ) {
    return r;
  }

  int n = me.keyCount();
  QString nm, snm, lbl;
  QMetaClassInfo ci;
  for( int i=0; i<n; ++i ) {
    int val = me.value( i ); // now the same as i, but ...
    snm = me.key( i );
    nm = QString( "enum_" ) + enum_name + "_" + QSN( val );
    int ci_idx = mci->indexOfClassInfo( nm.toLocal8Bit().data() );
    if( ci_idx < 0 ) {
      r << enum_name + "_" + QSN( val );
      continue;
    }
    ci = mci->classInfo( ci_idx );
    r << ci.value();
  }

  return r;
}

HolderData* HolderData::addObjToSubP( const QString &subname, const QString &tp, const QString &ob_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return nullptr;
  }
  HolderData *ob = sub->addObjP( tp, ob_name );
  return ob;
}

bool HolderData::addObjToSub( const QString &subname, const QString &tp, const QString &ob_name )
{
  HolderData *ho = addObjToSubP( subname, tp, ob_name );
  return ( ho != nullptr );
}

bool HolderData::delObjFromSub( const QString &subname, const QString &ob_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->delObj( ob_name );
}

bool HolderData::cloneObjInSub( const QString &subname,
       const QString &old_name, const QString &new_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->cloneObj( old_name, new_name );
}

bool HolderData::renameObjInSub( const QString &subname,
       const QString &old_name, const QString &new_name )
{
  HolderData *sub = getObj( subname );
  if( !sub ) {
    // warn
    return false;
  }
  return sub->renameObj( old_name, new_name );
}

QAbstractItemModel* HolderData::getComplModel( const QString &targ, QObject *mdl_par ) const
{
  auto mdl = new QStandardItemModel( mdl_par );
  // qDebug() << "req: target: " << targ << NWHE;
  if( targ == "in" ) {
    fillComplModelForInputs( mdl );
  } else if( targ == "prm" ) {
    fillComplModelForParams( mdl );
  } else if( targ == "out" ) {
    fillComplModelForOuts( mdl );
  }
  return mdl;
}

void HolderData::fillComplModelForInputs( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForInputs( mdl );
  }
}

void HolderData::fillComplModelForParams( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForParams( mdl );
  }
}

void HolderData::fillComplModelForOuts( QStandardItemModel *mdl ) const
{
  if( par ) {
    par->fillComplModelForOuts( mdl );
  }
}

int HolderData::fillComplForInputs( QStandardItem *it0, const QString &prefix ) const
{
  if( !it0 ) { return 0; }
  int n = 0;

  // // debug: special fake name ==Full.name
  // auto it = new QStandardItem( QSL("==") % prefix % getFullName() );
  // it0->appendRow( it );

  for( auto e: children() ) {

    if( auto hd = qobject_cast<HolderDouble*>( e ) ) {
      auto it = new QStandardItem( prefix % hd->objectName() );
      it0->appendRow( it ); ++n;
      continue;
    }

    if( auto hda = qobject_cast<HolderDoubleArray*>( e ) ) {
      auto it = new QStandardItem( prefix % hda->objectName() + "[0]" );
      it0->appendRow( it ); ++n;
      continue;
    }

    if( auto el = qobject_cast<TDataSet*>(e) ) {
      auto it = new QStandardItem( prefix % el->objectName() );
      int na = el->fillComplForInputs( it, prefix );
      if( na > 0 ) { // append row for elem only if at least one final completition
        it0->appendRow( it ); ++n;
      } else {
        delete it;
      }
    }

  }
  return n;
}

void HolderData::dumpStruct() const
{
  static int dump_lev = -1;
  ++dump_lev;
  qDebug() << toString() << NWHE;
}

void HolderData::post_set()
{
  do_post_set();
  for( auto ho: TCHILD(HolderData*) ) { // propagate to childs
    ho->post_set();
  }
}

void HolderData::do_post_set()
{
  qWarning() << ERR_ABS << NWHE;
}



const char* HolderData::helpstr { "Abstract data holder" };


// ---------------- HolderValue ---------
STD_CLASSINFO(HolderValue,clpData|clpPure);

CTOR(HolderValue,HolderData)
{
}

void HolderValue::reset_dfl()
{
  qWarning() << ERR_ABS << NWHE;
}

void HolderValue::do_post_set()
{
  qWarning() << ERR_ABS << NWHE;
}


bool HolderValue::set( const QVariant & /*x*/, int /* idx */ )
{
  qWarning() << ERR_ABS << NWHE;
  return false;
}

QVariant HolderValue::get( int /* idx */ ) const
{
  qWarning() << ERR_ABS << NWHE;
  return QVariant();
}

QString HolderValue::toString() const
{
  qWarning() << ERR_ABS << NWHE;
  return QString();
}

bool HolderValue::fromString( const QString & /*s */ )
{
  qWarning() << ERR_ABS << NWHE;
  return false;
}


QString HolderValue::getTypeV() const
{
  return "value?";
}

const char* HolderValue::helpstr { "Parent of all data" };


DEFAULT_FUNCS_REG(HolderValue);



// ---------------- HolderInt ---------
STD_CLASSINFO_ALIAS(HolderInt,clpData,int);

CTOR(HolderInt,HolderValue) , v(0)
{
  tp=QVariant::Int;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "INT,SIMPLE" );
  }
  post_set();
}

HolderInt::~HolderInt()
{
  v = 0; dyn = 0;
}

void HolderInt::reset_dfl()
{
  auto v0 = v;
  v = 0;
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = QString2IntEx( s );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderInt::set( const QVariant & x, int /* idx */ )
{
  auto v0 = v;
  bool ok;
  if( x.type() == QVariant::Int ) {
    v = x.toInt( &ok );
  } else {
    v = QString2IntEx( x.toString(), &ok );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderInt::get( int /* idx */ ) const
{
  return QVariant( v );
}

void HolderInt::do_post_set()
{
  int v_min { IMIN }, v_max { IMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() )
    v_min = s_min.toInt();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() )
    v_max = s_max.toInt();
  v = qBound( v_min, v, v_max );
}

QString HolderInt::toString() const
{
  return QSN( v );
}

bool HolderInt::fromString( const QString &s )
{
  auto v0 = v;
  bool ok = true;
  int vc = QString2IntEx( s, &ok );
  if( ok ) {
    v = vc;
    post_set();
  }
  if( v != v0 ) {
    setModified();
  }
  return ok;
}


QString HolderInt::getTypeV() const
{
  return "int";
}

const char* HolderInt::helpstr { "Contains integer data" };


DEFAULT_FUNCS_REG(HolderInt);


// ---------------- HolderSwitch ---------
STD_CLASSINFO_ALIAS(HolderSwitch,clpData,switch);

CTOR(HolderSwitch,HolderInt)
{
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,SWITCH" );
  }
  setParm("min","0"); setParm("max","1");
  post_set();
}

HolderSwitch::~HolderSwitch()
{
  // NOP
}

void HolderSwitch::do_post_set()
{
  v = v ? 1 : 0;
}

QString HolderSwitch::getTypeV() const
{
  return "switch";
}

const char* HolderSwitch::helpstr { "Contains integer (bin) data - switch iface" };

DEFAULT_FUNCS_REG(HolderSwitch);


// ---------------- HolderList ---------
STD_CLASSINFO_ALIAS(HolderList,clpData,list);

CTOR(HolderList,HolderInt)
{
  setParm("min","0"); setParm("max","0");
  if( getParm("props") == "INT,SIMPLE" ) {
    setParm( "props", "INT,LIST" );
  }

  // may be overkill, but one cone - one place
  QString enum_name = getParm( "enum" );
  QStringList sl;
  if( ! enum_name.isEmpty() ) {
    if( par ) {
      sl = par->getEnumStrings( enum_name );
    }
  }
  setParm( "max", QSN( sl.size()-1 ) );

  post_set();
}

HolderList::~HolderList()
{
  // NOP
}

void HolderList::do_post_set()
{
  HolderInt::do_post_set();
}


QString HolderList::getTypeV() const
{
  return "list";
}

const char* HolderList::helpstr { "Contains integer data - list iface" };

DEFAULT_FUNCS_REG(HolderList);



// ---------------- HolderDouble ---------
STD_CLASSINFO_ALIAS(HolderDouble,clpData,double);

CTOR(HolderDouble,HolderValue), v(0)
{
  tp=QVariant::Double;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "DOUBLE,SIMPLE" );
  }
}

HolderDouble::~HolderDouble()
{
  v = 0; dyn = 0;
}

void HolderDouble::reset_dfl()
{
  auto v0 = v;
  v = 0;
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = s.toDouble();
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderDouble::set( const QVariant & x, int /* idx */ )
{
  bool ok;
  auto v0 = v;
  v = x.toDouble( &ok );
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderDouble::get( int /* idx */ ) const
{
  return QVariant( v );
}

void HolderDouble::do_post_set()
{
  double v_min { DMIN }, v_max { DMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() )
    v_min = s_min.toDouble();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() )
    v_max = s_max.toDouble();
  v = qBound( v_min, v, v_max );
}

QString HolderDouble::toString() const
{
  return QSN( v, 'g', 18 ); // TODO? format
}

bool HolderDouble::fromString( const QString &s )
{
  auto v0 = v;
  bool ok;
  double vx = s.toDouble( &ok );
  if( ok ) {
    v = vx;
    post_set();
  }
  if( v != v0 ) {
    setModified();
  }
  return ok;
}


QString HolderDouble::getTypeV() const
{
  return "double";
}

const char* HolderDouble::helpstr { "Contains double data" };

DEFAULT_FUNCS_REG(HolderDouble);


// ---------------- HolderString ---------
STD_CLASSINFO_ALIAS(HolderString,clpData,QString);

CTOR(HolderString,HolderValue)
{
  tp=QVariant::String;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "STRING,SIMPLE" );
  }
}

HolderString::~HolderString()
{
  dyn = 0;
}

void HolderString::reset_dfl()
{
  auto v0 = v;
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = s;
  } else {
    v = "";
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
}

bool HolderString::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  v = x.toString();
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderString::get( int /* idx */ ) const
{
  return QVariant( v );
}

void HolderString::do_post_set()
{
  int v_max { IMAX };
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) {
    v_max = s_max.toInt();
  }
  v.truncate( v_max );
}

QString HolderString::toString() const
{
  return v;
}

bool HolderString::fromString( const QString &s )
{
  auto v0 = v;
  v = s;
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}


QString HolderString::getTypeV() const
{
  return "string";
}

const char* HolderString::helpstr { "Contains QString data" };

DEFAULT_FUNCS_REG(HolderString);


// ---------------- HolderColor ---------
STD_CLASSINFO_ALIAS(HolderColor,clpData,QColor);

CTOR(HolderColor,HolderValue)
{
  tp=QVariant::Color;
  post_set();
  if( getParm("props").isEmpty() ) {
    setParm( "props", "COLOR,INT" );
  }
}

HolderColor::~HolderColor()
{
}

void HolderColor::reset_dfl()
{
  auto v0 = v;
  v = QColor();
  QString s = getParm( "def" );
  if( ! s.isEmpty() ) {
    v = QColor( s );
    if( ! v.isValid() ) {
      v = QColor(s.toInt());
    }
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderColor::set( const QVariant & x, int /* idx */  )
{
  auto v0 = v;
  QString s = x.toString();
  v = QColor( s );
  if( ! v.isValid() ) {
    QRgb rgba = x.toInt();
    v.setRgba( rgba );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}

QVariant HolderColor::get( int /* idx */ ) const
{
  return QVariant( (int)(v.rgba()) );
}

void HolderColor::do_post_set()
{
  if( ! v.isValid() ) {
    v = QColor(Qt::red);
  }
}

QString HolderColor::toString() const
{
  //return QSN( (int)(v.rgba()) );
  return v.name();
}

bool HolderColor::fromString( const QString &s )
{
  auto v0 = v;
  v = QColor( s );
  if( ! v.isValid() ) {
    QRgb rgba = s.toInt();
    v.setRgba( rgba );
  }
  post_set();
  if( v != v0 ) {
    setModified();
  }
  return true;
}


QString HolderColor::getTypeV() const
{
  return "color";
}

const char* HolderColor::helpstr { "Contains QColor data" };

DEFAULT_FUNCS_REG(HolderColor);

// ---------------- HolderIntArray ---------
STD_CLASSINFO_ALIAS(HolderIntArray,clpData|clpArray,int[]);

CTOR(HolderIntArray,HolderValue)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_INT" );
  }
  // all done by reset_dfl
}

HolderIntArray::~HolderIntArray()
{
  dyn = 0;
}

void HolderIntArray::reset_dfl()
{
  auto v0 = v;
  int n = 1, v1 = 0;
  QString s = getParm("N");
  if( ! s.isEmpty() ) {
    n = s.toInt();
  }
  s = getParm("def");
  if( ! s.isEmpty() ) {
    v1 = s.toInt();
  }
  v.assign( n, v1 );

  s = getParm("defs");
  if( ! s.isEmpty() ) {
    QStringList sl = s.split( " ", QString::SkipEmptyParts );
    if( sl.size() > (int)v.size() ) {
      v.assign( sl.size(), v1 );
    }

    int i = 0;
    bool ok;
    for( auto cs : sl ) {
      int vc = cs.toInt( &ok, 0 ); // 0 = auto base
      if( ok ) {
        v[i] = vc;
      }
      i++;
    }
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderIntArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( idx < 0 || (unsigned)(idx) >= v.size() ) { // slow, but safe - not for fast code
    return false;
  }
  auto v0 = v[idx];
  v[idx] = x.toInt( &ok );
  post_set();
  if( v[idx] != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderIntArray::get( int idx ) const
{
  if( idx < 0 || (unsigned)idx >= v.size() ) { // slow, but safe - not for fast code
    return QVariant();
  }
  return QVariant( v[idx] );
}

void HolderIntArray::do_post_set()
{
  int v_min { IMIN }, v_max { IMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() )
    v_min = s_min.toInt();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() )
    v_max = s_max.toInt();
  for( int& vc : v ) {
    vc = qBound( v_min, vc, v_max );
  }
}

QString HolderIntArray::toString() const
{
  QString s, sep = "";
  for( int vc : v ) {
    s += sep + QSN( vc );
    sep = " ";
  }
  return s;
}

bool HolderIntArray::fromString( const QString &s )
{
  bool ok;
  QStringList sl = s.split(" ", QString::SkipEmptyParts );
  auto v0 = v;
  v.clear(); v.reserve( sl.size() );

  for( auto s : sl ) {
    int vc = s.toInt( &ok, 0 ); // 0 = auto base
    v.push_back( vc );
  }

  post_set();
  if( v != v0 ) { // slow
    setModified();
  }
  return ok; // ? only last
}


QString HolderIntArray::getTypeV() const
{
  return "int[]";
}

const char* HolderIntArray::helpstr { "Contains vector of integer data" };


DEFAULT_FUNCS_REG(HolderIntArray);


// ---------------- HolderDoubleArray ---------
STD_CLASSINFO_ALIAS(HolderDoubleArray,clpData|clpArray,double[]);

CTOR(HolderDoubleArray,HolderValue)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_DOUBLE" );
  }
}

HolderDoubleArray::~HolderDoubleArray()
{
  dyn = 0;
}

void HolderDoubleArray::reset_dfl()
{
  int n = 1;
  double v1 = 0;
  auto v0 = v;
  QString s = getParm("N");
  if( ! s.isEmpty() ) {
    n = s.toInt();
  }

  s = getParm("def");
  if( ! s.isEmpty() ) {
    v1 = s.toDouble();
  }
  v.assign( n, v1 );

  s = getParm("defs");
  if( ! s.isEmpty() ) {
    QStringList sl = s.split( " ", QString::SkipEmptyParts );
    if( sl.size() > (int)v.size() ) {
      v.assign( sl.size(), v1 );
    }

    int i = 0;
    bool ok;
    for( auto cs : sl ) {
      double vc = cs.toDouble( &ok );
      if( ok ) {
        v[i] = vc;
      }
      i++;
    }
  }

  post_set();
  if( v != v0 ) { // slow
    setModified();
  }
}


bool HolderDoubleArray::set( const QVariant & x, int idx )
{
  bool ok;
  if( idx < 0 || (unsigned)(idx) >= v.size() ) { // slow, but safe - not for fast code
    return false;
  }
  auto v0 = v[idx];
  v[idx] = x.toDouble( &ok );
  post_set();
  if( v[idx] != v0 ) {
    setModified();
  }
  return ok;
}

QVariant HolderDoubleArray::get( int idx ) const
{
  if( idx < 0 || (unsigned)idx >= v.size() ) // slow, but safe - not for fast code
    return QVariant();
  return QVariant( v[idx] );
}

void HolderDoubleArray::do_post_set()
{
  double v_min { DMIN }, v_max { DMAX };
  QString s_min = getParm( "min" );
  if( ! s_min.isEmpty() )
    v_min = s_min.toDouble();
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() )
    v_max = s_max.toDouble();
  for( double& vc : v ) {
    vc = qBound( v_min, vc, v_max );
  }
}

QString HolderDoubleArray::toString() const
{
  QString s, sep = "";
  for( double vc : v ) {
    s += sep + QSN( vc );
    sep = " ";
  }
  return s;
}

bool HolderDoubleArray::fromString( const QString &s )
{
  bool ok;
  QStringList sl = s.split(" ", QString::SkipEmptyParts );
  auto v0 = v;
  v.clear(); v.reserve( sl.size() );

  for( auto s : sl ) {
    double vc = s.toDouble( &ok );
    v.push_back( vc );
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
  return ok; // ? only last
}


QString HolderDoubleArray::getTypeV() const
{
  return "double[]";
}

const char* HolderDoubleArray::helpstr { "Contains vector of double data" };


DEFAULT_FUNCS_REG(HolderDoubleArray);


// ---------------- HolderStringArray ---------
STD_CLASSINFO_ALIAS(HolderStringArray,clpData|clpArray,string[]);

CTOR(HolderStringArray,HolderValue)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "ARRAY_STRING" );
  }
}

HolderStringArray::~HolderStringArray()
{
  dyn = 0;
}

void HolderStringArray::reset_dfl()
{
  int n = 1;
  auto v0 = v;
  QString s = getParm("N");
  if( ! s.isEmpty() ) {
    n = s.toInt();
  }
  QString sd = getParm("def");

  QStringList sl;
  s = getParm("defs");
  if( ! s.isEmpty() ) {
    sl = s.split( '\x01', QString::KeepEmptyParts ); // keep here
    if( sl.size() > n ) {
      n = sl.size();
    }
  }

  v.clear();
  int nds = sl.size();
  for( int i=0; i<n; ++i ) {
    QString vc;
    if( i < nds ) {
      vc = sl[i];
    } else {
      vc = sd;
    }
    v.append( vc );
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
}


bool HolderStringArray::set( const QVariant & x, int idx )
{
  if( idx < 0 || idx >= v.size() )  { // slow, but safe - not for fast code
    return false;
  }
  auto v0 = v[idx];
  v[idx] = x.toString();
  post_set();
  if( v[idx] != v0 ) {
    setModified();
  }
  return ! v[idx].isEmpty();
}

QVariant HolderStringArray::get( int idx ) const
{
  if( idx < 0 || idx >= v.size() ) { // slow, but safe - not for fast code
    return QVariant();
  }
  return QVariant( v[idx] );
}

void HolderStringArray::do_post_set()
{
  int len_max = IMAX;
  QString s_max = getParm( "max" );
  if( ! s_max.isEmpty() ) {
    len_max = s_max.toInt();
    for( QString& vc : v ) {
      vc.truncate(len_max);
    }
  }
}

QString HolderStringArray::toString() const
{
  return v.join( QChar(0x2400) ); // NUL representation in Unicode
}

bool HolderStringArray::fromString( const QString &s )
{
  auto v0 = v;
  if( s.contains( QChar(0x2400) ) ) {
    v = s.split( QChar(0x2400), QString::KeepEmptyParts );
  } else {
    v = s.split('\x01', QString::KeepEmptyParts ); // old strings
  }

  post_set();
  if( v != v0 ) {
    setModified();
  }
  return !v.isEmpty();
}


QString HolderStringArray::getTypeV() const
{
  return "string[]";
}

const char* HolderStringArray::helpstr { "Contains QStringList" };


DEFAULT_FUNCS_REG(HolderStringArray);



// ---------------- TDataSet ------------------------
STD_CLASSINFO(TDataSet,clpSpecial|clpPure);


const char* TDataSet::helpstr =
 "<H1>TDataSet</H1>\nThis is a base element for all other elements\n"
 "Never to be used directly";

CTOR(TDataSet,HolderData)
{
  tp=QVariant::UserType;
  if( getParm("props").isEmpty() ) {
    setParm( "props", "OBJ" );
  }
}

TDataSet::~TDataSet()
{
  state = stateBad;
}


// ----------------------------------


void TDataSet::reset_dfl()
{
  for( auto o : TCHILD(HolderData*) ) {
    o->reset_dfl();
    // it may be a non-holder elements, like model.eng?
  }
}



QString TDataSet::getTypeV() const
{
  return metaObject()->className();
}

DEFAULT_FUNCS_REG(TDataSet);




const double* TDataSet::getDoublePtr( const QString &nm, ltype_t *lt,
              const TDataSet **targ, int lev  ) const
{
  static ltype_t clt;
  ltype_t *plt = lt ? lt : &clt;
  if( nm.isEmpty() ) {
    *plt = LinkNone; return nullptr;
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
    TDataSet *ds= qobject_cast<TDataSet*>(ho);
    if( ds ) {
      return ds->getDoublePtr( "out0", plt, targ, lev+1 ); // defailt output
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
  TDataSet *ds = qobject_cast<TDataSet*>(ho);
  if( !ds ) {
    *plt = LinkBad;
    qWarning() << "Complex name " << nm << " is given for simple object "
               << ho->getFullName() << NWHE;
    return nullptr;
  }
  return ds->getDoublePtr( rest, plt, targ, lev+1 ); // pass to child

}


const double* TDataSet::getSchemeDoublePtr( const QString &nm, ltype_t *lt,
        const TDataSet **src_ob, int lev) const
{
  ltype_t clt = LinkNone;
  ltype_t *plt = ( lt ) ? lt : &clt;  // failsafe link

  if( nm.isEmpty() ) {
    *plt = LinkNone;
    return nullptr;
  }

  TDataSet *ds = qobject_cast<TDataSet*>( par );  // parent-less object or root
  if( !ds ) {
    *plt = LinkBad;
    return nullptr;
  }
  return ds->getSchemeDoublePtr( nm, lt, src_ob, lev );
}

// not const - change param
double* TDataSet::getDoublePrmPtr( const QString &nm, int *flg )
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




bool TDataSet::set( const QVariant & x, int /* idx */  )
{
  bool ok =  fromString( x.toString() );
  post_set();
  return ok;
}

QVariant TDataSet::get( int /* idx */ ) const
{
  return QVariant( this->toString() );
}

QString TDataSet::toString() const
{
  QString buf;
  buf.reserve(4096); // TODO ?
  QTextStream tstr( &buf, QIODevice::WriteOnly );
  QDomDocument dd_tmp;
  QDomElement el_tmp = dd_tmp.createElement("tmp_xxx");
  QDomElement dom = toDom( dd_tmp );
  dom.save( tstr, 4 );

  return buf;
}

bool TDataSet::fromString( const QString &s )
{
  QString errstr;
  int err_line, err_column;
  QDomDocument x_dd;
  if( ! x_dd.setContent( s, false, &errstr, &err_line, &err_column ) ) {
    qWarning() << "fail to parse str: line " << err_line << " col: " << err_column
               << " str: " << errstr << NWHE;
    return false;
  }
  QDomElement domroot = x_dd.documentElement();

  return fromDom( domroot, errstr );
}

QDomElement TDataSet::toDom( QDomDocument &dd ) const
{
  QDomElement de = dd.createElement( "obj" );
  de.setAttribute( "name", objectName() );
  de.setAttribute( "otype", getType() );

  for( auto ho : TCHILD(HolderData*) ) {
    if( ho->getFlags() & efNoSave ) {
      continue;
    }

    const TClassInfo *ci = ho->getClassInfo(); // dont write obsoleted objects
    if( ci->props & clpObsolete ) {
      continue;
    }

    QDomElement cde = ho->toDom( dd );
    de.appendChild( cde );
  }

  return de;
}


void TDataSet::do_post_set()
{
  // do nothing
};


// ------------------------------------ InputAbstract ---------
//
STD_CLASSINFO(InputAbstract,clpInput|clpSpecial|clpPure);

const double InputAbstract::fake_in {0};
const double InputAbstract::one_in {1.0};

CTOR(InputAbstract,TDataSet)
{
}

InputAbstract::~InputAbstract()
{
}


void InputAbstract::do_post_set()
{
  reportStructChanged(); // changed link means changes structure
}

void InputAbstract::do_structChanged()
{
  set_link();
}

void InputAbstract::set_link()
{
  p = &fake_in; src_obj = nullptr; linkType = LinkBad;
  if( source.cval().isEmpty() ) {
    linkType = LinkNone; return;
  }
  if( source.cval() == ":one" ) { // special local case
    linkType = LinkSpec;  p = &one_in;  return;
  }

  ltype_t lt;
  const TDataSet *srct = nullptr;
  const double *cp = getSchemeDoublePtr( source, &lt, &srct, 0 );
  if( lt == LinkElm || lt == LinkSpec ) {
    p = cp;  src_obj = srct; linkType = lt;
  } else {
    qWarning() << "ptr not found for " << source << NWHE;
  }

}

QVariant InputAbstract::dataObj( int col, int role ) const
{
  if( role == Qt::StatusTipRole && col == 0 ) { // used for button labels in dialogs

    QString s = source;
    QChar ac = QChar( 0x274C ); // X
    if( p != &fake_in ) {
      ac = QChar( 0x27BC ); // >>->
      if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
        ac = QChar( 0x2794 ); // ->
      }
    }
    s += ac;
    s += objectName();
    return s;
  }

  return TDataSet::dataObj( col, role );
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
  if( !par ) { // par seems to be InputParams, but may be dangling objects
    return;
  }

  TDataSet *el = qobject_cast<TDataSet*>( par->getParent() ); // grand par may by TMiso, but ...
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
  if( role == Qt::StatusTipRole && col == 0 ) { // used for button labels in dialogs
    QString s = source;
    QChar ac = QChar( 0x274C ); // X
    if( p != &fake_in ) {
      ac = QChar( 0x27BC );     // >>->
      if( src_obj && src_obj->isChildOf( "TMiso" ) ) {
        ac = QChar( 0x2794 );   // ->
      }
    }
    s += ac;

    s += objectName() % "." % tparam.cval();
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


// end of dataset.cpp


