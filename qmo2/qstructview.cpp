/***************************************************************************
                          qstructview.cpp  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2013 by atu
    email                : atu@nmetau.edu.ua
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui>

#include "miscfun.h"
#include "qstructview.h"
#include "qplotview.h"
#include "qdoubletable.h"
#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"
#include "qrunview.h"


  
QStructView::QStructView( QMo2Doc *adoc, QMo2View *mview, QWidget *parent )
            : QWidget( parent )
{
  doc = adoc; mainview = mview;
  model = mainview->getModel(); 
  devTp = 0;
  // grid_sz = 40; 
  grid_sz = 46; 
  lm = tm = 4;  obj_sz = 32;
  // setBackgroundMode( Qt::NoBackground );
  setMaximumSize( grid_sz*MODEL_MX, grid_sz*MODEL_MY );
  setMinimumSize( grid_sz*8, grid_sz*6 );
  setFocusPolicy( Qt::StrongFocus );
  setFocus();
}

QStructView::~QStructView()
{

}

QSize QStructView::getElemsBound() const
{
  int x, y, mx, my, elnu, n_el;
  TMiso *ob;
  int sel_x = mainview->getSelX();
  int sel_y = mainview->getSelY();
  mx = my = 3;
  if( model != 0 ) {
    n_el = model->getNMiso();
    for( elnu=0; elnu<n_el; elnu++ ) {
      ob = model->getMiso( elnu );
      if( ob == 0 ) continue;
      x = y = 0;
      ob->getData( "vis_x", &x );
      ob->getData( "vis_y", &y );
      if( x > mx ) mx = x;
      if( y > my ) my = y;
    };
  };
  if( sel_x > mx ) mx = sel_x;
  if( sel_y > my ) my = sel_y;
  mx+=3; my+=3;
  mx *= grid_sz; my *= grid_sz;
  QSize pas = mainview->svSize();
  if( mx > pas.width() ) pas.setWidth( mx );
  if( my > pas.height() ) pas.setHeight( my );
  return pas;
}  

QSize QStructView::sizeHint() const
{
  return getElemsBound();
}

QPoint QStructView::getSelCoords() const
{
  int sel_x = mainview->getSelX();
  int sel_y = mainview->getSelY();
  return QPoint( sel_x * grid_sz + grid_sz/2, sel_y * grid_sz + grid_sz/2 );
}


void QStructView::update()
{
  resize( getElemsBound() );
  QWidget::update();
}  


void QStructView::paintEvent( QPaintEvent * /*pe*/ )
{
  if( doc == 0 ) return;
  model = doc->getModel();
  devTp = 0;
  QPainter p( this );
  // p.setBackgroundMode( Qt::OpaqueMode );
  p.setBackground( hasFocus() ? Qt::white : QColor( 230, 230, 230 )  );
  p.eraseRect( 2, 2, width()-4, height()-4 );
  drawAll( p );
  p.end();
}

void QStructView::printAll()
{
  QPrinter *pr;
  if( doc == 0 || model == 0 || QMo2Win::qmo2win == 0 ) return;
  pr = QMo2Win::qmo2win->getPrinter();
  if( pr == 0 ) return;
  QPrintDialog pr_dialog( pr, this );
  if( pr_dialog.exec() ) {
    devTp = 1;
    pr->setFullPage( FALSE );
    pr->newPage();
    QPainter p( pr ); 
    drawAll( p );
    p.end();
  };
}

void QStructView::drawAll( QPainter &p )
{
  int i, h, w, nh, nw;
  int n_el, elnu,  n_out, out_nu, out_tp, n_li, line_busy;
  int ob_x, ob_y, ob_ord, ob_lock, ob_first, ob_last;
  int ob_flip, ob_noIcon;
  int li_src_x, li_src_y, li_dst_x, li_dst_y;
  int ob_gx, ob_gy; /* left top point of object icon */
  int ob_gxc; /* x- center of object icon */
  int flip_factor; /* sign of input offsets amd labels */
  int st_y; /* label on elems start y */
  int sel_x, sel_y, /*sel,*/ mark;
  int li_src_xs;
  LinkInfo li[8];
  QString qs, ob_name, target_name;
  TMiso *ob;
  TOutArr *arr;
  Mo2Settings *psett;
  psett = QMo2Win::qmo2win->getSettings();
  int s_icons = psett->showicons;
  const QFont &strf = QMo2Win::qmo2win->getStructFont();
  p.setFont( strf );
  const QFont &smlf = QMo2Win::qmo2win->getSmallFont();
  h = height(); w = width(); nh = 1 + h / grid_sz; nw = 1 + w / grid_sz;
  int el_marg = (grid_sz-obj_sz)/2; // elements margin
  sel_x = mainview->getSelX();
  sel_y = mainview->getSelY();
  //sel = mainview->getSel();
  mark = mainview->getMark();
  if( nh >= MODEL_MY ) nh = MODEL_MY;
  if( nw >= MODEL_MX ) nh = MODEL_MX;
  if( ! model ) {
    p.setBrush( Qt::red );
    p.drawRect( 0, 0, w, 8 );
    return;
  };
  // ---------- draw grid
  if( psett->showgrid ) {
    p.setPen( QPen(QColor(200,220,220), 0, Qt::DotLine ) );
    for( i=0; i<nw; i++ )
      p.drawLine( lm + i*grid_sz, tm, lm+i*grid_sz, h );
    for( i=0; i<nh; i++ )
      p.drawLine( lm, tm+i*grid_sz, w, tm+i*grid_sz );
  };
  n_el = model->getNMiso();
  p.setPen( Qt::black );
  // --------- draw elems ---------------------
  for( elnu=0; elnu<n_el; elnu++ ) {
    ob = model->getMiso( elnu );
    if( !ob ) 
      continue;
    ob_name =  ob->objectName();
    line_busy = 0; ob_x = ob_y = -1; ob_ord = -1; flip_factor = -1;
    ob_lock = ob_first = ob_last = ob_flip = ob_noIcon = 0;
    ob->getData( "vis_x", &ob_x ); 
    ob->getData( "vis_y", &ob_y );
    ob->getData( "ord", &ob_ord );
    ob->getData( "locked", &ob_lock );
    ob->getData( "onlyFirst", &ob_first );
    ob->getData( "onlyLast", &ob_last );
    ob->getData( "flip", &ob_flip );
    ob->getData( "noIcon", &ob_noIcon );
    if( ob_x < 0 || ob_x >=MODEL_MX || ob_y < 0 || ob_y >= MODEL_MY )
      continue;
    // set real object icon origin
    ob_gx = lm + ob_x * grid_sz + el_marg; 
    ob_gy = tm + ob_y * grid_sz + el_marg;
    ob_gxc = ob_gx + obj_sz/2; 
    flip_factor = ob_flip ? 1 : -1;
    p.setPen( Qt::black );

    // --------------------- draw element icon or frame
    if( ob_noIcon || !s_icons ) {
      p.setPen( QPen(Qt::black,2) );  p.setBrush( Qt::NoBrush );
      p.drawRect( ob_gx, ob_gy, obj_sz, obj_sz );
    } else {
      QString iconName = QString( ":icons/elm_" )
	+ QString(ob->getType()).toLower() 
	+ ".png";
      QIcon el_ico(iconName);
      el_ico.paint( &p, ob_gx, ob_gy, obj_sz, obj_sz );
    };
    p.setPen( Qt::black );  p.setBrush( Qt::red );

    // special marks: TODO: icons
    if( ob_lock )
      p.drawRect( ob_gx + 4, ob_gy + 8, 20, 8 );
    if( ob_first )
      p.drawRect( ob_gx + 4, ob_gy + 14, 6, 6 );
    if( ob_last )
      p.drawRect( ob_gx + 20, ob_gy +14, 6, 6 );
    
    // order mark
    if( psett->showord ) {
      if( s_icons && ! ob_noIcon )  {
	p.setPen( Qt::NoPen ); p.setBrush( QColor(240,240,255) );
	p.drawRect( ob_gx, ob_gy, 31,11);
      };
      p.setPen( Qt::black );
      p.drawText( ob_gx + 2, ob_gy + 10, QSN(ob_ord) );
      line_busy++;
    };
    // --------------------- draw element name
    if( psett->shownames || ob_noIcon || !s_icons ) { 
      st_y = ob_gy + line_busy*10;
      if( s_icons && ! ob_noIcon )  {
	p.setPen( Qt::NoPen ); p.setBrush( QColor(255,255,225) );
	p.drawRect( ob_gx, st_y, 31,11);
      };
      p.setPen( Qt::black );
      p.drawText( ob_gx + 2, st_y+10, ob_name );
      line_busy++;
    };
    // TODO: vector icon here
    p.setBrush( Qt::NoBrush ); p.setPen( QPen(Qt::black,1) );
    st_y = ob_gy + line_busy*10; 

    
    if( elnu == mark ) { // red rect around marked element
      p.setBrush( Qt::NoBrush ); p.setPen( Qt::red );
      p.drawRect( lm + ob_x*grid_sz, tm + ob_y*grid_sz, grid_sz, grid_sz );
    };

    // ------------------ draw element links
    if( model->getLinkInfos( elnu, li ) != 0 )
      continue;
    p.setBrush( Qt::red );
    n_li = 0;
    for( i=0; i<4; i++ ) {
      if( li[i].ltype != LinkNone )
	n_li++;
    };
    // old simple inputs: replaced
    
    for( i=4; i<8; i++ ) { // param inputs TODO: replace
      li_dst_x = ob_gx + (i-3)*6;
      li_dst_y = ob_gy + obj_sz-el_marg;
      switch( li[i].ltype ) {
	case LinkNone: continue;
	case LinkBad: 
	      p.setPen( Qt::NoPen ); p.setBrush( Qt::red );
	      p.drawRect( li_dst_x, li_dst_y, 4, 4 );
              break;
	case LinkElm:
	      li_src_x = lm + li[i].x*grid_sz + (li[i].eflip ? el_marg : obj_sz+el_marg); 
	      li_src_y = tm + obj_sz + li[i].y*grid_sz - 8;

	      p.setPen( li[i].pflags ? QPen(Qt::magenta,2) : QPen(Qt::red,2) );
	      
              li_src_xs = li_src_x + (li[i].eflip?-3:3);
	      p.drawLine( li_src_x, li_src_y, li_src_xs, li_src_y );
	      p.drawLine( li_dst_x, li_dst_y, li_dst_x, li_dst_y+4 );
	      p.drawLine( li_src_xs, li_src_y, li_src_xs, li_dst_y+4 );
	      p.drawLine( li_src_xs, li_dst_y+4, li_dst_x, li_dst_y+4 );
	      break;
	case LinkSpec:
	      p.setPen( Qt::NoPen );  p.setBrush( Qt::blue );
	      p.drawRect( li_dst_x, li_dst_y, 4, 4 );
	      break;

      };
    }; // end param inputs loop
    
    //  new input marks
    int n_in = ob->inputsCount();
    int in_sep_sz = obj_sz/(n_in+1);
    p.setFont( smlf ); 
    p.setBrush( Qt::NoBrush );
    
    if( psett->showLinks ) {
      // p.drawText( ob_gx + 16, ob_gy+obj_sz/2, QSN( n_in ) );
      for( int i_in=0; i_in < n_in; ++i_in ) {
        const InputSimple *in = ob->getInput(i_in);
        if( ! in )
          continue;
        ltype_t lt = in->getLinkType();
        const TDataSet* targ = in->getTarget();
        li_dst_x = ob_gxc + flip_factor * (obj_sz/2);
        li_dst_y = ob_gy + (i_in+1)*in_sep_sz;
        int pre_dst_x = li_dst_x + el_marg * flip_factor; // *2 ?
        QString lbl; 
        int line_color, line_width = 1, x_shift = 0, y_shift = 0;
        in->getData("line_w", &line_width );
        in->getData("x_shift", &x_shift );
        in->getData("y_shift", &y_shift );
        if( in->getData("line_color", &line_color ) ) {
          p.setPen( QPen( QColor(QRgb(line_color)), line_width ) );
        } else {
          p.setPen( QPen( Qt::black, line_width ) );
        }
        if( lt == LinkNone ) {
          p.drawEllipse( QPoint(li_dst_x, li_dst_y), el_marg/3, el_marg/3 );
          continue;
        }
        int x_vert = pre_dst_x + x_shift * flip_factor;
        // arrow
        // p.drawLine( pre_dst_x, li_dst_y,   li_dst_x, li_dst_y  );
        p.drawLine( li_dst_x, li_dst_y, x_vert, li_dst_y );
        p.drawLine( li_dst_x +  3*flip_factor, li_dst_y-2, li_dst_x, li_dst_y  );
        p.drawLine( li_dst_x +  3*flip_factor, li_dst_y+2, li_dst_x, li_dst_y  );
        
        in->getData("label", lbl );
        if( ! lbl.isEmpty() ) {
          p.drawText( x_vert-2*flip_factor, li_dst_y-2, lbl );
        }
        
        if( lt == LinkBad ) {
          p.setPen( QPen( Qt::red, 2 ) );
          p.drawLine( x_vert-el_marg/3, li_dst_y-el_marg/3, 
                      x_vert+el_marg/3, li_dst_y+el_marg/3 );
          p.drawLine( x_vert-el_marg/3, li_dst_y+el_marg/3, 
                      x_vert+el_marg/3, li_dst_y-el_marg/3 );
          continue;
        }
        
        if( lt == LinkSpec ) {
          p.setPen( QPen( Qt::magenta, 2 ) );
          p.drawRect( x_vert-el_marg/4, li_dst_y-el_marg/4, el_marg/2, el_marg/2 );
          continue;
        }

        // here must be ordinary targets -> large mark if not so
        const TMiso *etarg = nullptr;
        if( !targ || ( (etarg = qobject_cast<const TMiso*>(targ)) == nullptr ) ) {
          p.setPen( QPen( Qt::red, 4 ) );
          p.drawEllipse( QPoint(x_vert, li_dst_y), el_marg, el_marg );
          continue;
        }

        // get info about source and calc coords
        int so_x = -1, so_y = -1, so_flip = 0, only_lbl = 0;
        int so_flip_factor = 1, li_src_xc = 0;
        QString so;
        etarg->getData( "vis_x", &so_x );
        etarg->getData( "vis_y", &so_y );
        etarg->getData( "flip", &so_flip );
        in->getData( "source", so );
        so_flip_factor = ( so_flip ) ? -1 : 1;

        li_src_xc = lm + so_x*grid_sz + obj_sz/2 + el_marg;
        li_src_x = li_src_xc + obj_sz * so_flip_factor / 2;
        if( ob_y != so_y ) {
          li_src_y = tm + so_y*grid_sz + obj_sz/2 + el_marg - y_shift;
        } else {
          li_src_y = li_dst_y - y_shift; // special case: one line
        }

        if( so.contains(".") ) { // complex - not 'out0' source
          QChar qshc = (so.right(1))[0];
          char shc = qshc.toLatin1();
          li_src_y += 2 + ( shc  & 0x07 );
          p.drawLine( li_src_x + el_marg*so_flip_factor/2, li_src_y, 
                      li_src_x + el_marg*so_flip_factor/2, li_src_y+3 );
        }

        p.drawLine( li_src_x, li_src_y, 
                    li_src_x + el_marg*so_flip_factor, li_src_y );
        
        in->getData( "onlyLabel", &only_lbl );
        if( only_lbl ) {
          if( ! lbl.isEmpty() ) {
            p.drawText( li_src_x+(3+el_marg)*so_flip_factor, li_src_y-2, lbl );
          }
          continue;
        }
        
        p.drawLine( x_vert, li_dst_y, x_vert, li_src_y ); // vertical part
        p.drawLine( x_vert, li_src_y, li_src_x, li_src_y); // horiz. from src

      }
    }
  }; // end loop on elems

  p.setFont( smlf );
  n_out = model->getNOutArr(); p.setPen( Qt::black );
 
  // -------------- output marks
  for( out_nu=0; out_nu < n_out; out_nu++ ) {
    arr = model->getOutArr( out_nu );
    if( ! arr ) 
      continue;
    target_name = ""; out_tp = -1;
    arr->getData( "name", target_name );
    arr->getData( "type", &out_tp );
    ltype_t lt  = LinkBad; 
    const TDataSet *lob = nullptr;
    const double *fp = model->getDoublePtr( target_name, &lt, &lob );
    if( !fp || lt != LinkElm || !lob )
      continue;
    const TMiso *targ = qobject_cast<const TMiso*>(lob);
    if( ! targ ) 
      continue;
    ob_x = ob_y = -1;
    targ->getData( "vis_x", &ob_x ); targ->getData( "vis_y", &ob_y );
    if( ob_x < 0 || ob_x >=MODEL_MX || ob_y < 0 || ob_y >= MODEL_MY )
      continue;
    switch( out_tp ) {
      case 0: p.setBrush( Qt::white ); break;
      case 1: p.setBrush( Qt::green ); break;
      case 2: p.setBrush( Qt::cyan ); break;
      case 3: p.setBrush( Qt::gray ); break;
      default: p.setBrush( Qt::red ); break;
    };
    int omark_x = lm + ob_x*grid_sz + obj_sz - 10 - out_nu;
    int omark_y = tm + ob_y*grid_sz +  1;
    p.drawRect( omark_x, omark_y, 10, 10 );
    if( target_name.contains('.') ) { // inner link mark
      p.setBrush( Qt::red );
      p.drawRect( omark_x, omark_y+9, 10, 2 );
    }
    p.drawText( omark_x+2, omark_y+9,  QSN( out_nu ) );
  }; // end loop on outputs
  
  // ----------- draw selection
  if( devTp != 1 ) {
    QPainter::CompositionMode old_op = p.compositionMode();
    p.setCompositionMode( QPainter::RasterOp_SourceXorDestination );
    p.setPen( Qt::NoPen ); 
    p.setBrush( QColor(64,64,32) );
    p.drawRect( lm + sel_x*grid_sz, tm + sel_y*grid_sz, grid_sz, grid_sz );
    p.setCompositionMode( old_op );
  };

}


void QStructView::mousePressEvent( QMouseEvent *me )
{
  int h, w, nh, nw, ex, ey, x, y;
  QMenu *menu;
  TMiso *ob = 0;
  QString elmname( "?bad?" );
  double outval = 0;
  QString title;
  h = height(); w = width(); nh = h / grid_sz - 1; nw = w / grid_sz - 1;
  x = me->x(); y = me->y(); 
  ex = ( x - lm ) / grid_sz; ey = ( y - tm ) / grid_sz;
  if( ex >= 0 && ex <= nw && ey >=0 && ey <= nh ) {
    mainview->changeSel( ex, ey, 0 );
    ob = model->xy2Miso( ex, ey );
    if( ob ) {
      elmname = ob->objectName();
      ob->getData( "out0", &outval );
      if( elmname.isEmpty() )
	elmname = "?unknown?";
    };
    title = elmname;
    if( model->getState() > 1 ) {
      title += QString( "(" ) + QString::number( outval ) + QString( ")" );
    }
    switch( me->button() ) {
      case Qt::LeftButton:  break;
      case Qt::RightButton:  
	    menu = new QMenu( this ); 
	    QAction *act;
	    if( ob != 0 ) {
	      // menu->setTitle( title );
	      (void) menu->addAction( title ); // only a title
	      menu->addSeparator();
	      act = menu->addAction( "&Edit" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(editElm() ) );
	      act = menu->addAction( "&Delete" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(delElm() ) );
	      menu->addSeparator();
	      act = menu->addAction( "&Link" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(linkElm() ) );
	      act = menu->addAction( "&Reorder" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(ordElm() ) );
	      menu->addSeparator();
	    } else {
	      act = menu->addAction( "&New" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(newElm() ) );
	      if( mainview->getMark() >= 0 ) {
		act = menu->addAction( "&Move to" );
		connect( act, SIGNAL( activated() ), mainview, SLOT(moveElm() ) );
              }
	    };
	    menu->addSeparator();
	    act = menu->addAction( "New outp&ut" );
	    connect( act, SIGNAL( activated() ), mainview, SLOT(newOut() ) );
	    menu->addSeparator();
	    act = menu->addAction( "Edit model" );
	    connect( act, SIGNAL( activated() ), mainview, SLOT(editModel() ) );
	    act = menu->addAction( "Print model" );
	    connect( act, SIGNAL( activated() ), mainview, SLOT(print() ) );
	    
	    menu->exec( mapToGlobal(QPoint( x, y )) );
	    delete menu;
            break;
      case Qt::MidButton:   mainview->editElm();  break;
      default: break;// none
    };
  };
}

void QStructView::mouseDoubleClickEvent( QMouseEvent * /*me*/ )
{
  mainview->editElm();
}


void QStructView::keyPressEvent( QKeyEvent *ke )
{
  int k, /*h, w, nh, nw,*/ st, btnShift, /*btnCtrl,*/ xy_delta;
  k = ke->key(); st = ke->modifiers(); 
  btnShift = ( ( st & Qt::ShiftModifier ) != 0 );
  // btnCtrl = ( ( st & Qt::ControlModifier ) != 0 );
  // h = height(); 
  // w = width(); 
  // nh = h / grid_sz - 1; 
  // nw = w / grid_sz - 1;
  xy_delta = btnShift ? 5 : 1;
  switch( k ) {
    case Qt::Key_Return: mainview->editElm(); break; // to catch both keys
    case Qt::Key_Home:  emit sig_changeSel( 0, 0, 0 ); break;
    case Qt::Key_Left:  emit sig_changeSel( -xy_delta, 0, 1 );  break;
    case Qt::Key_Right: emit sig_changeSel( xy_delta, 0, 1 );  break;
    case Qt::Key_Up:    emit sig_changeSel( 0, -xy_delta, 1 );  break;
    case Qt::Key_Down:  emit sig_changeSel( 0, xy_delta, 1 );  break;
    case Qt::Key_Tab:   emit sig_changeSel( 1, 0, 2 ); break;
    case Qt::Key_0:     emit sig_changeLevel(0); break;
    case Qt::Key_1:     emit sig_changeLevel(1); break;
    case Qt::Key_2:     emit sig_changeLevel(2); break;
    case Qt::Key_3:     emit sig_changeLevel(3); break;
    case Qt::Key_4:     emit sig_changeLevel(4); break;
    case Qt::Key_5:     emit sig_changeLevel(5); break;
    case Qt::Key_6:     emit sig_changeLevel(6); break;
    case Qt::Key_7:     emit sig_changeLevel(7); break;
    case Qt::Key_8:     emit sig_changeLevel(8); break;
    case Qt::Key_9:     emit sig_changeLevel(9); break;
    default: ke->ignore();
  };
}

// end of qstructview.cpp


