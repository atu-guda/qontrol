/***************************************************************************
                          qmo2view.h  -  description
                             -------------------
    begin                : Mon Jul 31 16:51:57 EEST 2000
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

#ifndef QMO2VIEW_H
#define QMO2VIEW_H

// include files for Qt
#include <QWidget>
class QResizeEvent;
class QCloseEvent;

class QMo2Doc;
class QStructView;
class QOutView;
class QGraphView;
class TDataSet;
class TRootData;
class TModel;
class QStatusModel;
class QScrollArea;
class QTreeView;

class HolderModel;
class TMiso;

/**
 * This class provides an base for QMo2Doc view.
 */
class QMo2View : public QWidget
{
   Q_OBJECT
   friend class QMo2Doc;
 public:
   /** enumeration for chate checking */
   enum CheckType { validCheck = 0, selCheck, linkToCheck,
                     noselCheck, moveCheck, doneCheck
   };
   /** Constructor for the view
     * @param pDoc  your document instance that the view represents. 
     * Create a document before calling the constructor
     * or connect an already existing document to a new MDI child widget.*/
   QMo2View( QMo2Doc* pDoc, QWidget* parent );
   /** Destructor for the main view */
   ~QMo2View();
   /** returns a pointer to the document connected to the view*/
   QMo2Doc *getDocument() const;
   /** gets called to redraw the document contents if it has been modified */
   void update(QMo2View* pSender);
   // /** implement preferred size */
   // QSize sizeHint();
   /** inner size for structure view */
   QSize svSize() const;

   const QString& currentFile() const;

 signals:
   void viewChanged();

 public slots: 
   /** called whan we need to update views */
   void updateViews();
   /** change selection rel: 0-abs 1-rel 2-next elm */
   void changeSel( int x, int y, int rel );
   /** change level for selecting aux objects */
   void changeLevel( int lev );
   /** contains the implementation for printing functionality  */
   void print();
   // element related
   void newElm();
   void delElm();
   void editElm();
   void linkElm();
   void qlinkElm();
   void qplinkElm();
   void unlinkElm();
   void lockElm();
   void ordElm();
   void markElm();
   void moveElm();
   void infoElm();
   void testElm1();
   void testElm2();
   void cutElm();
   void copyElm();
   void pasteElm();
   
   // outs related
   void newOut();
   void delOut();
   void editOut();
   void editOut2();
   void showOutData();
   void exportOut();
  
   // graphs related
   void newGraph();
   void delGraph();
   void editGraph();
   void editGraph2();
   void showGraph();
   void showGraphData();
   void exportGraphData();
   void gnuplotGraph();
  
   // model related
   void editModel();
   void showVars();
   void editModel2();
  
   // runs
   void runRun();
   void runPrm();
   void runPrm2();
   void resetModel();
   
   // misc
   void showHelp();

 public: 
   /** returns document rootdata */
   TRootData* getRoot(void);
   /** returns document model */
   TModel* getModel(void);
   /** returns x-coordinate for selected cell */
   int getSelX(void) const { return sel_x; }
   /** returns y-coordinate for selected cell */
   int getSelY(void) const { return sel_y; }
   /** returns number of selected element or -1 if none */
   int getSel(void) const { return sel; }
   /** returns ptr to selected element or nullptr if none */
   TMiso* getSelObj(void) const { return selObj; }
   /** returns number of marked element or -1 if none */
   int getMark(void) const { return mark; }
   /** returns level for aux objects selection */
   int getLevel(void) const { return level; }
 protected:
   virtual void closeEvent( QCloseEvent* );
   virtual void resizeEvent( QResizeEvent* );
   int checkState( CheckType ctp );
 protected:  
   QScrollArea *scrollArea;
   QStructView *sview;
   QOutView *oview;
   QGraphView *gview;
   QStatusModel *stam;
   QTreeView *treeView;
   QMo2Doc *doc;
   TRootData *root;
   TModel *model;
   HolderModel *ho_mo;
   int sel, sel_x, sel_y, mark, level;
   /** prt to selected object or nullptr */
   TMiso *selObj;
   /** prt to marked object or nullptr */
   TMiso *markObj;
   /** name of marked element */
   QString markName;
   /** help string */
   static const char helpstr[];
};
typedef QMo2View *PQMo2View;

#endif

// end of qmo2view.h

