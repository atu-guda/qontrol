QT += core widgets xml script printsupport
TEMPLATE = app
TARGET = qontrol
CONFIG += c++14 release warn_on qt thread
CONFIG += no-keywords
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
  dataset.cpp \
  elemfact.cpp \
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
  mglview.cpp \
  outdataview.cpp \
  runview.cpp \
  statusmodel.cpp \
  structview.cpp \
  rootdata.cpp \
  scaledata.cpp \
  scheme.cpp \
  schemeview.cpp \
  scriptfuncs.cpp \
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
  ../elems/tcorralalysis.cpp \
  ../elems/tcounter.cpp \
  ../elems/tcriterion.cpp \
  ../elems/tdelay.cpp \
  ../elems/tdiffin.cpp \
  ../elems/textrlatch.cpp \
  ../elems/tfriction.cpp \
  ../elems/tfilesimple.cpp \
  ../elems/tfilesource.cpp \
  ../elems/tfuncmisc.cpp \
  ../elems/tfuncpoly.cpp \
  ../elems/tfunctrans.cpp \
  ../elems/thyst.cpp \
  ../elems/tintegrator.cpp \
  ../elems/tlatch.cpp \
  ../elems/tlinear.cpp \
  ../elems/tlinp1q0.cpp \
  ../elems/tlogic.cpp \
  ../elems/tlorenz.cpp \
  ../elems/trossler.cpp \
  ../elems/tmaxnum.cpp \
  ../elems/tmultipl.cpp \
  ../elems/tmulsumn.cpp \
  ../elems/tpid.cpp \
  ../elems/tquadextr.cpp \
  ../elems/tquality.cpp \
  ../elems/trand.cpp \
  ../elems/trandtrigg.cpp \
  ../elems/trelaxgen.cpp \
  ../elems/tsource.cpp \
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
  autoact.h \
  circbuf.h \
  cmdview.h \
  cmdlistview.h \
  commonsubwin.h \
  contgraph.h \
  contout.h \
  dataset.h \
  defs.h \
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
  mglview.h \
  outdataview.h \
  runview.h \
  statusmodel.h \
  structview.h \
  rootdata.h \
  scaledata.h \
  scheme.h \
  schemeview.h \
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
  ../elems/tcorralalysis.h \
  ../elems/tcounter.h \
  ../elems/tcriterion.h \
  ../elems/tdelay.h \
  ../elems/tdiffin.h \
  ../elems/textrlatch.h \
  ../elems/tfilesimple.h \
  ../elems/tfilesource.h \
  ../elems/tfriction.h \
  ../elems/tfuncmisc.h \
  ../elems/tfuncpoly.h \
  ../elems/tfunctrans.h \
  ../elems/thyst.h \
  ../elems/tintegrator.h \
  ../elems/tlatch.h \
  ../elems/tlinear.h \
  ../elems/tlinp1q0.h \
  ../elems/tlogic.h \
  ../elems/tlorenz.h \
  ../elems/trossler.h \
  ../elems/tmaxnum.h \
  ../elems/tmultipl.h \
  ../elems/tmulsumn.h \
  ../elems/tpid.h \
  ../elems/tquadextr.h \
  ../elems/tquality.h \
  ../elems/trand.h \
  ../elems/trandtrigg.h \
  ../elems/trelaxgen.h \
  ../elems/tsource.h \
  ../elems/tsourcelin.h \
  ../elems/tsubscheme.h \
  ../elems/tsum8.h \
  ../elems/tsumn.h \
  ../elems/tswitch.h \
  ../elems/tthrum.h \
  ../elems/ttrigger.h \
  ../elems/tvibro.h


