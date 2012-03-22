/***************************************************************************
                          qstructview.cpp  -  description
                             -------------------
    begin                : Sat Aug 12 2000
    copyright            : (C) 2000-2012 by atu
    email                : atu@dmeti.dp.ua
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

#include "resource.h"
#include "miscfun.h"
#include "qanydial.h"
#include "qstructview.h"
#include "qplotview.h"
#include "qdoubletable.h"
#include "qmo2win.h"
#include "qmo2doc.h"
#include "qmo2view.h"
#include "qrunview.h"


  
QStructView::QStructView( QMo2Doc *adoc, QMo2View *mview,  
                          QWidget *parent, const char * /*name*/ )
            : QWidget( parent )
{
  doc = adoc; mainview = mview;
  model = mainview->getModel(); 
  devTp = 0;
  grid_sz = 40; lm = tm = 4;
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
      ob->getDataSI( "vis_x", &x, 0 );
      ob->getDataSI( "vis_y", &y, 0 );
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
  p.setBackgroundMode( Qt::OpaqueMode );
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
  int i, ci, h, w, nh, nw;
  int n_el, elnu,  n_out, out_nu, out_tp, n_li, line_busy;
  int ob_x, ob_y, ob_ord, ob_lock, ob_noauto, ob_first, ob_last;
  int ob_flip, ob_noIcon;
  int li_mid_y, li_src_x, li_src_y, li_dst_x, li_dst_y;
  int ob_gx, ob_gy, st_y, ic_sz;
  int sel_x, sel_y, /*sel,*/ mark;
  int li_dst_xs, li_src_xs, li_dst_xa;
  int s_ord, s_grid, s_names, s_icons;
  LinkInfo li[8];
  char ob_descr[96];
  QString qs, ob_name;
  TMiso *ob;
  TOutArr *arr;
  Mo2Settings *psett;
  static int xl_diff[4] = { 16, 10, 8, 6 };
  psett = QMo2Win::qmo2win->getSettings();
  s_ord =   psett->showord;
  s_grid =  psett->showgrid;
  s_names = psett->shownames;
  s_icons = psett->showicons;
  QFont strf;
  strf.fromString( psett->structFont );
  //strf.fromString( psett->smallFont );
  p.setFont( strf );
  h = height(); w = width(); nh = 1 + h / grid_sz; nw = 1 + w / grid_sz;
  sel_x = mainview->getSelX();
  sel_y = mainview->getSelY();
  //sel = mainview->getSel();
  mark = mainview->getMark();
  if( nh >= MODEL_MY ) nh = MODEL_MY;
  if( nw >= MODEL_MX ) nh = MODEL_MX;
  if( model == 0 ) {
    p.setBrush( Qt::red );
    p.drawRect( 0, 0, w, 8 );
    return;
  };
  // ---------- draw grid
  if( s_grid ) {
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
    if( ob == 0 ) continue;
    ob_name =  QString::fromLocal8Bit( ob->getName() );
    line_busy = 0; ob_x = ob_y = -1; ob_ord = -1;
    ob_lock = ob_noauto = ob_first = ob_last = ob_flip = ob_noIcon = 0;
    ob->getDataSI( "vis_x", &ob_x, 0 ); ob->getDataSI( "vis_y", &ob_y, 0 );
    ob->getDataSI( "ord", &ob_ord, 0 );
    ob->getDataSI( "links.locked", &ob_lock, 0 );
    ob->getDataSI( "links.noauto", &ob_noauto, 0 );
    ob->getDataSI( "links.onlyFirst", &ob_first, 0 );
    ob->getDataSI( "links.onlyLast", &ob_last, 0 );
    ob->getDataSI( "links.flip", &ob_flip, 0 );
    ob->getDataSI( "links.noIcon", &ob_noIcon, 0 );
    if( ob_x < 0 || ob_x >=MODEL_MX || ob_y < 0 || ob_y >= MODEL_MY )
      continue;
    // set real object icon origin
    ob_gx = lm + ob_x * grid_sz + 4; ob_gy = tm + ob_y * grid_sz + 4;
    p.setPen( Qt::black );

    // --------------------- draw element icon or frame
    if( ob_noIcon || !s_icons ) {
      p.setPen( QPen(Qt::black,2) );  p.setBrush( Qt::NoBrush );
      p.drawRect( ob_gx, ob_gy, 32, 32 );
    } else {
      QPixmap pix( ob->getIcon() );
      p.drawPixmap( ob_gx, ob_gy, pix, 0, 0, grid_sz, grid_sz );
    };
    p.setPen( Qt::black );  p.setBrush( Qt::red );

    // special marks: TODO: icons
    if( ob_lock )
      p.drawRect( ob_gx + 4, ob_gy + 10, 20, 8 );
    if( ob_first )
      p.drawRect( ob_gx + 4, ob_gy, 6, 6 );
    if( ob_last )
      p.drawRect( ob_gx + 20, ob_gy, 6, 6 );
    if( ob_noauto )
      p.drawRect( ob_gx + 30, ob_gy + 20, 6, 6 );
    
    // order mark
    if( s_ord ) {
      if( s_icons && ! ob_noIcon )  {
	p.setPen( Qt::NoPen ); p.setBrush( QColor(240,240,255) );
	p.drawRect( ob_gx, ob_gy, 31,11);
      };
      p.setPen( Qt::black );
      p.drawText( ob_gx + 2, ob_gy + 10, QString::number(ob_ord) );
      line_busy++;
    };
    // --------------------- draw element name
    if( s_names || ob_noIcon || !s_icons ) { 
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
    ic_sz = 32 - line_busy * 10; ++ic_sz; ic_sz--; // TODO: fake
    //p.drawRect( ob_gx + 32 - ic_sz, st_y, ic_sz, ic_sz);

    
    if( elnu == mark ) { // red rect around marked element
      p.setBrush( Qt::NoBrush ); p.setPen( Qt::red );
      p.drawRect( lm + ob_x*grid_sz, tm + ob_y*grid_sz, grid_sz, grid_sz );
      p.drawRect( lm - 1 + ob_x*grid_sz, tm - 1 + ob_y*grid_sz, 42, 42 );
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
    for( ci=0,i=0; i<4 && n_li > 0 ; i++ ) { // inputs
      li_dst_x = lm+4 + ob_x*grid_sz + ( ob_flip ? 32 : 0 );
      li_dst_y = tm+4 + ob_y*grid_sz + (ci+1)*xl_diff[n_li-1];
      switch( li[i].ltype ) {
	case LinkNone: continue;
	case LinkBad: 
	      p.setPen( Qt::NoPen ); p.setBrush( Qt::red );
	      p.drawRect( li_dst_x-2, li_dst_y-1, 4, 4 );
	      ci++; break;
	case LinkElm:
	      li_src_x = lm + 36 + li[i].x*grid_sz - (li[i].eflip ? 32 : 0); 
	      li_src_y = tm + 20 + li[i].y*grid_sz;
	      if( ob_y == li[i].y  &&  ob_flip == li[i].eflip ) {
		li_src_y = li_dst_y;
	      };
	      if( ((li_dst_x + 4> li_src_x) || li[i].eflip  ) )
	         li_mid_y = li_src_y;
	      else  
	         li_mid_y = li_dst_y;
	         //li_mid_y = ( li_src_y + li_dst_y ) / 2 ;

	      p.setPen( QPen(Qt::black,1) );
	      li_dst_xs = li_dst_x - (8-ci*2) * (ob_flip?-1:1);
	      li_dst_xa = li_dst_x + (ob_flip?3:-3);
	      li_src_xs = li_src_x + (li[i].eflip?-4:4);
              
	      if( ( li[i].x < ob_x ) ^ (li_src_xs < li_dst_xs ) ) {
		li_src_xs = li_dst_xs = ( li_src_xs + li_dst_xs ) / 2;
	      };

	      p.drawLine( li_src_x, li_src_y, li_src_xs, li_src_y ); // start-
	      p.drawLine( li_dst_x, li_dst_y, li_dst_xs, li_dst_y ); // -end
	      p.drawLine( li_src_xs, li_mid_y, li_dst_xs, li_mid_y );// --mid-- 
	      p.drawLine( li_src_xs, li_src_y, li_src_xs, li_mid_y );//1st vert
	      p.drawLine( li_dst_xs, li_dst_y, li_dst_xs, li_mid_y );//2nd vert
	      //  arrow
	      p.drawLine( li_dst_x, li_dst_y, li_dst_xa, li_dst_y+2 );
	      p.drawLine( li_dst_x, li_dst_y, li_dst_xa, li_dst_y-2 );
	      ci++; break;
	case LinkSpec:
	      p.setPen( Qt::NoPen );  p.setBrush( Qt::blue );
	      p.drawRect( li_dst_x-2, li_dst_y-1, 4, 4 );
	      ci++; break;

      };
    };
    for( i=4; i<8; i++ ) { // param inputs
      li_dst_x = lm+6 + ob_x*grid_sz + (i-4)*6;
      li_dst_y = tm+34+ob_y*grid_sz;
      switch( li[i].ltype ) {
	case LinkNone: continue;
	case LinkBad: 
	      p.setPen( Qt::NoPen ); p.setBrush( Qt::red );
	      p.drawRect( li_dst_x, li_dst_y, 4, 4 );
              break;
	case LinkElm:
	      li_src_x = lm + 34 + li[i].x*grid_sz - (li[i].eflip ? 32 : 0); 
	      li_src_y = tm + 20 + li[i].y*grid_sz;
	      if( li_dst_x > li_src_x )
		li_mid_y = li_src_y;
	      else  
		li_mid_y = ( li_src_y + li_dst_y ) / 2 ;

	      if( devTp != 1 ) {
		p.setPen( li[i].pflags ? QPen(Qt::magenta,2) : QPen(Qt::red,2) );
	      } else {
		p.setPen( QPen( Qt::black, 2 ) );
	      };
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
  }; // end loop on elems

  QFont smlf;
  smlf.fromString( psett->smallFont );
  p.setFont( smlf );
  n_out = model->getNOutArr(); p.setPen( Qt::black );
 
  // -------------- output marks
  for( out_nu=0; out_nu < n_out; out_nu++ ) {
    arr = model->getOutArr( out_nu );
    if( arr == 0 ) continue;
    ob_descr[0] = 0; out_tp = -1;
    arr->getDataSS( "name", ob_descr, 96, 0 );
    arr->getDataSI( "type", &out_tp, 0 );
    elnu = model->oname2elnu( ob_descr );
    if( elnu < 0 ) continue;
    ob = model->getMiso( elnu );
    if( ob == 0 ) continue;
    ob_x = ob_y = -1;
    ob->getDataSI( "vis_x", &ob_x, 0 ); ob->getDataSI( "vis_y", &ob_y, 0 );
    if( ob_x < 0 || ob_x >=MODEL_MX || ob_y < 0 || ob_y >= MODEL_MY )
      continue;
    switch( out_tp ) {
      case 0: p.setBrush( Qt::white ); break;
      case 1: p.setBrush( Qt::green ); break;
      case 2: p.setBrush( Qt::cyan ); break;
      case 3: p.setBrush( Qt::gray ); break;
      default: p.setBrush( Qt::red ); break;
    };
    p.drawRect( lm+22 + ob_x*grid_sz, tm+1 + ob_y*grid_sz, 10, 10 );
    p.drawText( lm+24 + ob_x*grid_sz, tm+9 + ob_y*grid_sz, 
	        QString::number( out_nu ) );
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
  int h, w, nh, nw, ex, ey, x, y, elnu;
  QMenu *menu;
  TMiso *ob = 0;
  const char *elmname = "?bad?";
  double outval;
  QString title;
  h = height(); w = width(); nh = h / grid_sz - 1; nw = w / grid_sz - 1;
  x = me->x(); y = me->y(); 
  ex = ( x - lm ) / grid_sz; ey = ( y - tm ) / grid_sz;
  if( ex >= 0 && ex <= nw && ey >=0 && ey <= nh ) {
    mainview->changeSel( ex, ey, 0 );
    elnu = model->xy2elnu( ex, ey );
    if( elnu >=0 ) {
      ob = model->getMiso( elnu );
      if( ob != 0 ) {
	elmname = ob->getName();
	if( elmname == 0 || elmname[0] == 0 )
	  elmname = "?unknown?";
      };
      title = elmname;
      if( model->getState() > 1 ) {
	outval = model->xout( elnu );
	title += QString( "(" ) + QString::number( outval ) + QString( ")" );
      }
    };
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
	    } else {
	      act = menu->addAction( "&New" );
	      connect( act, SIGNAL( activated() ), mainview, SLOT(newElm() ) );
	      if( mainview->getMark() >= 0 )
		act = menu->addAction( "&Move to" );
		connect( act, SIGNAL( activated() ), mainview, SLOT(moveElm() ) );
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


