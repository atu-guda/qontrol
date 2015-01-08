var n = 20;
var res;
// var a[];
for( var i=0; i<n; ++i ) {
  var rr = 1.1 * i;
  main_s.lor.setD( "r", rr );
  model.run_bg();
  res = main_s.x2.getDataD( "out0", 0.0 );
  print( rr, ' ', res,  '\n' );
}
