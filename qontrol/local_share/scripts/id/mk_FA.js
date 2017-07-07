// model.lsf();

// var FA_cfg = {
//   x0: 4, y0: 1, y_dlt: 2, N: 5, p_min: 20, p_max: 60,
//   m_type: "TLorenz", m_prm: "r", m_add:  "\nsigma=lor_sigma!\nb=lor_b!",
//   q_type: "TLinP1Q0", q_in_add: ".x2",
//   F_type:  "TQuality",
//   p_type: "TSearcher"
// };

function mk_ansample1( cfg )
{
  var p_dlt = cfg.p_max - cfg.p_min;
  var p_k = p_dlt / cfg.N;

  main_s.delByNameType( "m_", cfg.m_type );
  main_s.delByNameType( "q_", cfg.q_type );
  main_s.delByNameType( "F_", cfg.F_type );
  main_s.delByNameType( "p_", cfg.p_type );
  main_s.delElem(  "p_o" );
  main_s.delElem(  "obj" );
  main_s.delElem(  "q_o" );
  main_s.delElem(  "p_xx" );
  main_s.delElem(  "e_ge" );
  main_s.delElem(  "e_le" );
  main_s.delElem(  "e_ee" );
  main_s.delElem(  "e_xe" );
  main_s.post_set();

  if( !model.hasObj( "p_o0" )    ) { model.addObjDatas( "double", "p_o0", "30" ); }
  if( !model.hasObj( "U_p" )     ) { model.addObjDatas( "double", "U_p", "15" ); }
  if( !model.hasObj( "omega_p" ) ) { model.addObjDatas( "double", "omega_p", "1.57" ); }
  if( !model.hasObj( "q_gamma" ) ) { model.addObjDatas( "double", "q_gamma", "1" ); }
  if( !model.hasObj( "a_q" )     ) { model.addObjDatas( "double", "a_q", "2" ); }
  if( !model.hasObj( "v_f" )     ) { model.addObjDatas( "double", "v_f", "0.5" ); }
  if( !model.hasObj( "k_g" )     ) { model.addObjDatas( "double", "k_g", "1.0" ); }
  if( !model.hasObj( "k_cl" )    ) { model.addObjDatas( "double", "k_cl", "0.5" ); }
  if( !model.hasObj( "k_ch" )    ) { model.addObjDatas( "double", "k_ch", "0.01" ); }
  if( !model.hasObj( "k_nl" )    ) { model.addObjDatas( "double", "k_nl", "0.5" ); }
  if( !model.hasObj( "k_nh" )    ) { model.addObjDatas( "double", "k_nh", "0.01" ); }

  main_s.addObjDatas( "TSource", "p_o", "vis_x=" + (cfg.x0-3) + "\nvis_y=" + cfg.y0  );
  main_s.p_o.uu.fromString( "U_p!" );
  main_s.p_o.cc.fromString( "p_o0!" );
  main_s.p_o.omega.fromString( "omega_p!" );
  main_s.addObjDatas( cfg.m_type, "obj", "vis_x=" + (cfg.x0-2) + "\nvis_y=" + cfg.y0  );
  main_s.obj[cfg.m_prm].fromString( "p_o" );

  main_s.addObjDatas( "TLinP1Q0", "q_o", "vis_x=" + cfg.x0 + "\nvis_y=" + cfg.y0 );
  main_s.q_o.a.fromString( "a_q!" );
  main_s.q_o.in_u.fromString( "obj" + cfg.q_in_add );

  for( var i=0; i<cfg.N; ++i ) {
    var x_c = cfg.x0;
    var y_c = cfg.y0 + ( cfg.N - i ) * cfg.y_dlt;
    var p_l = cfg.p_min + (i-1) * p_k;
    var p_c = cfg.p_min + i * p_k;
    var p_r = cfg.p_min + (i+1) * p_k;

    print( "Creating at (" + x_c + "," + y_c + ") p= ", p_c, "\n" );
    var m_nm = "m_" + i;
    main_s.addObjDatas( cfg.m_type, m_nm, "vis_x=" + (x_c-2) + "\nvis_y=" + y_c + cfg.m_add );
    main_s.setD( m_nm + "." + cfg.m_prm + ".source", "p_" + i );

    var q_nm = "q_" + i;
    main_s.addObjDatas( cfg.q_type, q_nm, "vis_x=" + x_c + "\nvis_y=" + y_c );
    var t_q = main_s[q_nm];
    t_q.a.fromString( "a_q!" );
    t_q.in_u.fromString( m_nm + cfg.q_in_add );

    var F_nm = "F_" + i;
    main_s.addObjDatas( cfg.F_type, F_nm, "vis_x=" + (x_c+2) + "\nvis_y=" + y_c );
    var t_F = main_s[F_nm];
    t_F.q_gamma.fromString( "q_gamma!"  );
    t_F.q_o.fromString( "q_o"  );
    t_F.q_m.fromString( q_nm  );
    t_F.F_min.fromString( "0"  );
    t_F.F_max.fromString( "1"  );

    var p_nm = "p_" + i;
    main_s.addObjDatas( cfg.p_type, p_nm, "vis_x=" + (x_c+4) + "\nvis_y=" + y_c );
    var t_p = main_s[p_nm];
    t_p.p_c.fromString( p_nm );
    t_p.F_c.fromString( F_nm );
    t_p.out0_init.fromString( p_c );
    t_p.autocalc_minmax.fromString( "0" );
    t_p.p_min.fromString( p_l );
    t_p.p_max.fromString( p_r );
    var p_l_nm = p_l;
    var F_l_nm = "0";
    if( i > 0 ) {
      p_l_nm = "p_" + (i-1);
      F_l_nm = "F_" + (i-1);
    }
    var p_r_nm = p_r;
    var F_r_nm = "0";
    if( i < (cfg.N-1) ) {
      p_r_nm = "p_" + (i+1);
      F_r_nm = "F_" + (i+1);
    }
    t_p.p_l.fromString( p_l_nm );
    t_p.F_l.fromString( F_l_nm );
    t_p.p_r.fromString( p_r_nm );
    t_p.F_r.fromString( F_r_nm );

    t_p.v_f.fromString( "v_f!");
    t_p.k_cl.fromString( "k_cl!");
    t_p.k_ch.fromString( "k_ch!");
    t_p.k_ch2.fromString( "0");
    t_p.k_nl.fromString( "k_nl!");
    t_p.k_nh.fromString( "k_nh!");
    t_p.k_nh2.fromString( "0");
    t_p.k_g.fromString( "k_g!");

  }
  main_s.post_set();

  main_s.addObjDatas( "TMulsumN", "p_xx", "vis_x=" + (cfg.x0+5) + "\nvis_y=" + cfg.y0 );

  for( var i=0; i<cfg.N; ++i ) {
    var lnm = "p_" + i;
    main_s.p_xx.addObjDatas( "InputSimple", lnm,  "source=" + lnm );
    main_s.p_xx[lnm].setD( "channel",  i );
    main_s.p_xx[lnm].setD( "onlyLabel",  1 );
  }
  for( var i=0; i<cfg.N; ++i ) {
    var lnm = "F_" + i;
    main_s.p_xx.addObjDatas( "InputSimple", lnm,  "source=" + lnm );
    main_s.p_xx[lnm].setD( "channel",  i );
    main_s.p_xx[lnm].setD( "subchannel",  1 );
    main_s.p_xx[lnm].setD( "onlyLabel",  1 );
  }
  main_s.p_xx.F_0.setParm( "sep",  "col" );

  main_s.addObjDatas( "TErrMeasure", "e_ge", "vis_x=" + (cfg.x0+6) + "\nvis_y=" + cfg.y0 );
  main_s.e_ge.in_x.fromString( "p_o" );
  main_s.e_ge.in_y.fromString( "p_xx.pge" );
  main_s.e_ge.in_y0.fromString( "p_o0" );
  main_s.addObjDatas( "TErrMeasure", "e_le", "vis_x=" + (cfg.x0+6) + "\nvis_y=" + (cfg.y0+1) );
  main_s.e_le.in_x.fromString( "p_o" );
  main_s.e_le.in_y.fromString( "p_xx.ple" );
  main_s.e_le.in_y0.fromString( "p_o0" );
  main_s.addObjDatas( "TErrMeasure", "e_ee", "vis_x=" + (cfg.x0+6) + "\nvis_y=" + (cfg.y0+2) );
  main_s.e_ee.in_x.fromString( "p_o" );
  main_s.e_ee.in_y.fromString( "p_xx.pee" );
  main_s.e_ee.in_y0.fromString( "p_o0" );

}

mk_ansample1( FA_cfg );
//fun_tmp = mk_ansample1;
//print( "mk_FA.js: " + typeof( mk_ansample1 ) + " fun_tmp: " + typeof( fun_tmp ) + '\n' );

1;
