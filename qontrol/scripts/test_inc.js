// funcion to test include in JS

print( "file scripts/test_inc.js loading... \n" );

test_inc = function()
{
  print( "Function test_inc called\n" );
}

print( "file scripts/test_inc.js loaded.\n" );
print( "test_inc is ", typeof test_inc, "\n" );

