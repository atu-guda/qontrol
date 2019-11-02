QT += core widgets xml script printsupport multimedia
TEMPLATE = app
TARGET = qontrol
CONFIG += c++17 release warn_on qt thread
#CONFIG += c++14 debug warn_on qt thread
CONFIG += no-keywords
CONFIG += qscintilla2
QMAKE_CXXFLAGS += -g3  -std=gnu++17
#QMAKE_CXXFLAGS += -g3  -std=gnu++17 -DQT_NO_CAST_FROM_ASCII=1
#QMAKE_CXXFLAGS += -fgnu-keywords -fext-numeric-literals # -fPIC
#CONFIG += debug warn_on qt thread
OBJECTS_DIR=.obj
MOC_DIR=.moc

RESOURCES = qontrol.qrc

INCLUDEPATH += .. ../elems

LIBS += -lgsl -lgslcblas
LIBS += -lmgl
LIBS += -lfftw3
#LIBS += -lboost_thread -lboost_chrono

SOURCES += \
  addelemdia.cpp \
  circbuf.cpp \
  cmdview.cpp \
  cmdlistview.cpp \
  commonsubwin.cpp \
  contgraph.cpp \
  contout.cpp \
  datapool.cpp \
  dataset.cpp \
  elemfact.cpp \
  elemparams.cpp \
  linkedobj.cpp \
  main.cpp \
  miscfun.cpp \
  colorbtn.cpp \
  doubletable.cpp \
  fontbtn.cpp \
  graphdataview.cpp \
  labodoc.cpp \
  laboview.cpp \
  labowin.cpp \
  linkcompleter.cpp \
  longvalidator.cpp \
  mglview.cpp \
  outdataview.cpp \
  paramsdialog.cpp \
  pastedialog.cpp \
  runview.cpp \
  statusmodel.cpp \
  structview.cpp \
  rootdata.cpp \
  scaledata.cpp \
  scheme.cpp \
  schemeview.cpp \
  scriptfuncs.cpp \
  scriptdialog.cpp \
  settingsdata.cpp \
  simul.cpp \
  simulview.cpp \
  tmiso.cpp \
  tmodel.cpp \
  datawidget.cpp \
  tex2label.cpp \
  tgraph.cpp \
  toutarr.cpp \
  rand_gen.cpp \
  ../elems/tadjgen.cpp \
  ../elems/tbarrier.cpp \
  ../elems/tbjt.cpp \
  ../elems/tcorralalysis.cpp \
  ../elems/tcounter.cpp \
  ../elems/tcriterion.cpp \
  ../elems/tdelay.cpp \
  ../elems/tdiffin.cpp \
  ../elems/tdyn3d.cpp \
  ../elems/tdynlink3d.cpp \
  ../elems/textrlatch.cpp \
  ../elems/terrmeasure.cpp \
  ../elems/textrtest.cpp \
  ../elems/tfriction.cpp \
  ../elems/tfilesimple.cpp \
  ../elems/tfilesource.cpp \
  ../elems/tfuncmisc.cpp \
  ../elems/tfuncpoly.cpp \
  ../elems/tfunctrans.cpp \
  ../elems/thyst.cpp \
  ../elems/tiir7.cpp \
  ../elems/tintegrator.cpp \
  ../elems/tlatch.cpp \
  ../elems/tlinear.cpp \
  ../elems/tlinp1q0.cpp \
  ../elems/tlogic.cpp \
  ../elems/tlorenz.cpp \
  ../elems/trossler.cpp \
  ../elems/tmatrix34.cpp \
  ../elems/tmaxnum.cpp \
  ../elems/tmovingcorr.cpp \
  ../elems/tmultipl.cpp \
  ../elems/tmulsumn.cpp \
  ../elems/tpid.cpp \
  ../elems/tquadextr.cpp \
  ../elems/tquality.cpp \
  ../elems/tqsearcher.cpp \
  ../elems/trand.cpp \
  ../elems/trandtrigg.cpp \
  ../elems/trelaxgen.cpp \
  ../elems/tsource.cpp \
  ../elems/tsearcher.cpp \
  ../elems/tsinsum.cpp \
  ../elems/tsourcelin.cpp \
  ../elems/tsubscheme.cpp \
  ../elems/tsum8.cpp \
  ../elems/tsumn.cpp \
  ../elems/tswitch.cpp \
  ../elems/tthrum.cpp \
  ../elems/ttrigger.cpp \
  ../elems/tvibro.cpp

HEADERS += \
  addelemdia.h \
  askedparam.h \
  autoact.h \
  circbuf.h \
  cmdview.h \
  cmdlistview.h \
  commonsubwin.h \
  contgraph.h \
  contout.h \
  datapool.h \
  dataset.h \
  defs.h \
  elemparams.h \
  linkedobj.h \
  miscfun.h \
  colorbtn.h \
  doubletable.h \
  fontbtn.h \
  graphdataview.h \
  labodoc.h \
  laboview.h \
  labowin.h \
  linkcompleter.h \
  longvalidator.h \
  mglview.h \
  outdataview.h \
  paramsdialog.h \
  pastedialog.h \
  runview.h \
  statusmodel.h \
  structview.h \
  rootdata.h \
  scaledata.h \
  scheme.h \
  schemeview.h \
  scriptdialog.h \
  scriptfuncs.h \
  settingsdata.h \
  simul.h \
  simulview.h \
  tmiso.h \
  tmodel.h \
  datawidget.h \
  tgraph.h \
  toutarr.h \
  rand_gen.h \
  ../elems/tadjgen.h \
  ../elems/tbarrier.h \
  ../elems/tbjt.h \
  ../elems/tcorralalysis.h \
  ../elems/tcounter.h \
  ../elems/tcriterion.h \
  ../elems/tdelay.h \
  ../elems/tdiffin.h \
  ../elems/tdyn3d.h \
  ../elems/tdynlink3d.h \
  ../elems/terrmeasure.h \
  ../elems/textrlatch.h \
  ../elems/textrtest.h \
  ../elems/tfilesimple.h \
  ../elems/tfilesource.h \
  ../elems/tfriction.h \
  ../elems/tfuncmisc.h \
  ../elems/tfuncpoly.h \
  ../elems/tfunctrans.h \
  ../elems/thyst.h \
  ../elems/tiir7.h \
  ../elems/tintegrator.h \
  ../elems/tlatch.h \
  ../elems/tlinear.h \
  ../elems/tlinp1q0.h \
  ../elems/tlogic.h \
  ../elems/tlorenz.h \
  ../elems/trossler.h \
  ../elems/tmatrix34.h \
  ../elems/tmaxnum.h \
  ../elems/tmovingcorr.h \
  ../elems/tmultipl.h \
  ../elems/tmulsumn.h \
  ../elems/tpid.h \
  ../elems/tquadextr.h \
  ../elems/tquality.h \
  ../elems/tqsearcher.h \
  ../elems/trand.h \
  ../elems/trandtrigg.h \
  ../elems/trelaxgen.h \
  ../elems/tsearcher.h \
  ../elems/tsinsum.h \
  ../elems/tsource.h \
  ../elems/tsourcelin.h \
  ../elems/tsubscheme.h \
  ../elems/tsum8.h \
  ../elems/tsumn.h \
  ../elems/tswitch.h \
  ../elems/tthrum.h \
  ../elems/ttrigger.h \
  ../elems/tvibro.h


