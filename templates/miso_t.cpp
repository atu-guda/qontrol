
const char* ZZZZ::helpstr = "<H1>ZZZZ</H1>\n"
 "Do: <br>\n"
 "Parameters:<br>\n"
 " - <b>parm1</b> -- descr;<br>\n"
 " - <b>parm1</b> -- descr;<br>\n"
 " - <b>parm1</b> -- descr.<br>\n"
 "Have 5 parameters: <b>a0 .. a3, b</b>, each can be changed at any time.";

TClassInfo ZZZZ::class_info = {
  CLASS_ID_ZZZZ, "ZZZZ", ZZZZ::create,
  &TMiso::class_info, helpstr };

TDataInfo ZZZZ::tzzzz_d_i[20] = {
// tp      subtp       l    dx   dy   dw   dh  fl  min  max hv dy  name        descr  list_d
 { dtpDial,       0,   0,    0,   0, 380, 300, 0,  0.0, 0.0, 0, 0, "Zzz", "", "Dialog for ZZZZ"},
 { dtpInt,        0,   0,   10,  10,  70,  20, 8,  0.0, 1e6, 0, 0, "ord", "order", ""},
 { dtpStr,        0,  60,   90,  10, 280,  20, 0,  0.0, 0.0, 0, 0, "descr", "Object description",""},
 { dtpLabel,      0,   0,   20,  50,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpInt, dtpsList,   2,   10,  70, 120,  20, 8,  0.0, 0.0, 0, 0, "type", "type", ""},
 { dtpLabel,      0,   0,   20,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "l_type",   "", "Type"},
 { dtpDou,        0,   0,   30, 110, 120,  20, 0,  -1e300, 1e300, 0, 0, "a0", "a0", ""},
 { dtpLabel,      0,   0,   90,  90,  50,  20, 0,  0.0, 0.0, 0, 0, "la1",   "", "a1="},
 { dtpDou,        0,   0,  150,  90, 120,  20, 0,  -1e300, 1e300, 0, 0, "a1", "a1", ""},
 { dtpLabel,      0,   0,   90, 120,  50,  20, 0,  0.0, 0.0, 0, 0, "la2",   "", "a2="},
 { dtpDou,        0,   0,  150, 120, 120,  20, 0,  -1e300, 1e300, 0, 0, "a2", "a2", ""},
 { dtpLabel,      0,   0,   90, 150,  50,  20, 0,  0.0, 0.0, 0, 0, "la3",   "", "a3="},
 { dtpDou,        0,   0,  150, 150, 120,  20, 0,  -1e300, 1e300, 0, 0, "a3", "a3", ""},
 { dtpLabel,      0,   0,   90, 180,  50,  20, 0,  0.0, 0.0, 0, 0, "lb",   "", "b="},
 { dtpDou,        0,   0,  150, 180, 120,  20, 0,  -1e300, 1e300, 0, 0, "b", "b", ""},
 { dtpButton,     0,   0,   20, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_ok", "", "OK"},
 { dtpButton,     1,   0,  140, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_can", "", "Cancel"},
 { dtpButton,     2,   0,  260, 240,  90,  30, 0,  0.0, 0.0, 0, 0, "btn_help", "", "Help"},
 { dtpObj, CLASS_ID_TElmLink,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "links", "Object links", "" },
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_x", "X coord in scheme", ""},
 { dtpInt,        0,   0,    0,   0,   0,   0, 6,  0.0, 0.0, 0, 0, "vis_y", "Y coord in scheme", ""},
 { dtpEnd,        0,   0,    0,   0,   0,   0, 0, 0.0, 0.0, 0, 0, "", "", "" }
};



ZZZZ::ZZZZ( PTDataSet aparent )
        :TMiso( aparent )
{
  int i;
  a[0] = 1; a[1] = -1; a[2] = a[3] = 0; b = 0;
  d_i = tzzzz_d_i;
  initHash();
  for( i=0; i<nelm; i++ ) {
    ptrs.push_back( 0 );
  };
  ptrs[1] = &ord; ptrs[2] = descr;  // from TMiso
  ptrs[4] = &type; ptrs[6] = &a[1]; ptrs[8] = &a[2];
  ptrs[10] = &a[3]; ptrs[12] = &b;
  // from TMiso 
  ptrs[16] = links;
  ptrs[17] = &vis_x; ptrs[18] = &vis_y;
}

ZZZZ::~ZZZZ()
{
}

PTDataSet ZZZZ::create( PTDataSet apar )
{
  return new ZZZZ( apar );
}

int ZZZZ::getClassId(void) const 
{
  return CLASS_ID_ZZZZ;
}

const char* ZZZZ::getClassName(void) const 
{
  return "ZZZZ";
}

const TClassInfo* ZZZZ::getClassInfo(void) const
{
  return &class_info;
}

const char *ZZZZ::getHelp(void) const
{
  return helpstr;
}

const char** ZZZZ::getIcon(void) const
{
  return icon;
}

int ZZZZ::preRun( int run_tp, int an, int anx, int any, double adt )
{
  int rc;
  rc = TMiso::preRun( run_tp, an, anx, any, adt );
  // here

  return rc;
}

int ZZZZ::postRun( int good )
{
  TMiso::postRun( good );
  return 0;
}

int ZZZZ::startLoop( int acnx, int acny )
{
  int rc = TMiso::startLoop( acnx, acny );

  return rc;
}

int ZZZZ::startLoop2( int acny )
{
  return 0;
}

int ZZZZ::endLoop(void)
{
  return 0;
}

int ZZZZ::endLoop2(void)
{
  return 0;
}

double ZZZZ::f( const double* u, double /* t */ )
{
  double v;
  return v;
}

/* XPM */
const char* ZZZZ::icon[] = {
/* width height num_colors chars_per_pixel */
"    32    32       16            1",
/* colors */
"` c #000000",
". c #800000",
"# c #008000",
"a c #808000",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #c0c0c0",
"f c #808080",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
/* pixels */
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeeeee`eeeeeeeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeee``eeeeee``eeeeeeeeeeeeee",
"eeeeeee`eeeeeeeeee`eeeeeeeeeeeee",
"eeeeee```eeeeeeee```eeeeeeeeeeee",
"eeeee`e```eeeeee```e`eeeeeeeeeee",
"eeeee`ee```eeee```ee`eeeeeeeeeee",
"eeee`eeee```ee```eeee`eeeeeeeeee",
"eeee`eeeee``````eeeee`ee``eeeeee",
"eeee`eeeeee````eeeeee`eeee``eeee",
"`````eeeeee````eeeeee````````eee",
"eeee`eeeee``````eeeee`eeee``eeee",
"eeee`eeee````````eeee`ee``eeeeee",
"eeeee`ee``````````ee`eeeeeeeeeee",
"eeeee`e````````````e`eeeeeeeeeee",
"eeeeee``````````````eeeeeeeeeeee",
"eeeeeee````````````eeeeeeeeeeeee",
"eeeeeeee``````````eeeeeeeeeeeeee",
"eeeeeeeeee``````eeeeeeeeeeeeeeee",
"eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"
};


