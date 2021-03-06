/* include file for building of extension libs using GNU-Win32 toolkit,
   which is based on the Cygnus Cygwin32 API.  This file is included by
   the extension dlls when they are built.  Global vars defined in perl
   exe are referenced by the extension module dll by using __imp_varName,
   where varName is the name of the global variable in perl.exe.
   GNU-Win32 has no equivalent to MSVC's __declspec(dllimport) keyword to
   define a imported global, so we have to use this approach to access
   globals exported by perl.exe.
    -jc 4/1/97
*/

#define  impure_setupptr   (*__imp_impure_setupptr)
#define  Perl_reall_srchlen   (*__imp_Perl_reall_srchlen)
#define  Perl_yychar   (*__imp_Perl_yychar)
#define  Perl_yycheck   (*__imp_Perl_yycheck)
#define  Perl_yydebug   (*__imp_Perl_yydebug)
#define  Perl_yydefred   (*__imp_Perl_yydefred)
#define  Perl_yydgoto   (*__imp_Perl_yydgoto)
#define  Perl_yyerrflag   (*__imp_Perl_yyerrflag)
#define  Perl_yygindex   (*__imp_Perl_yygindex)
#define  Perl_yylen   (*__imp_Perl_yylen)
#define  Perl_yylhs   (*__imp_Perl_yylhs)
#define  Perl_yylval   (*__imp_Perl_yylval)
#define  Perl_yynerrs   (*__imp_Perl_yynerrs)
#define  Perl_yyrindex   (*__imp_Perl_yyrindex)
#define  Perl_yysindex   (*__imp_Perl_yysindex)
#define  Perl_yytable   (*__imp_Perl_yytable)
#define  Perl_yyval   (*__imp_Perl_yyval)
#define  Perl_regarglen   (*__imp_Perl_regarglen)
#define  Perl_regdummy   (*__imp_Perl_regdummy)
#define  Perl_regkind   (*__imp_Perl_regkind)
#define  Perl_simple   (*__imp_Perl_simple)
#define  Perl_varies   (*__imp_Perl_varies)
#define  Perl_watchaddr   (*__imp_Perl_watchaddr)
#define  Perl_watchok   (*__imp_Perl_watchok)
#define  Argv   (*__imp_Argv)
#define  Cmd   (*__imp_Cmd)
#define  DBgv   (*__imp_DBgv)
#define  DBline   (*__imp_DBline)
#define  DBsignal   (*__imp_DBsignal)
#define  DBsingle   (*__imp_DBsingle)
#define  DBsub   (*__imp_DBsub)
#define  DBtrace   (*__imp_DBtrace)
#define  Error   (*__imp_Error)
#define  Perl_AMG_names   (*__imp_Perl_AMG_names)
#define  Perl_No   (*__imp_Perl_No)
#define  Perl_Sv   (*__imp_Perl_Sv)
#define  Perl_Xpv   (*__imp_Perl_Xpv)
#define  Perl_Yes   (*__imp_Perl_Yes)
#define  Perl_amagic_generation   (*__imp_Perl_amagic_generation)
#define  Perl_an   (*__imp_Perl_an)
#define  Perl_buf   (*__imp_Perl_buf)
#define  Perl_bufend   (*__imp_Perl_bufend)
#define  Perl_bufptr   (*__imp_Perl_bufptr)
#define  Perl_check   (*__imp_Perl_check)
#define  Perl_collation_ix   (*__imp_Perl_collation_ix)
#define  Perl_collation_name   (*__imp_Perl_collation_name)
#define  Perl_collation_standard   (*__imp_Perl_collation_standard)
#define  Perl_collxfrm_base   (*__imp_Perl_collxfrm_base)
#define  Perl_collxfrm_mult   (*__imp_Perl_collxfrm_mult)
#define  Perl_compcv   (*__imp_Perl_compcv)
#define  Perl_compiling   (*__imp_Perl_compiling)
#define  Perl_comppad   (*__imp_Perl_comppad)
#define  Perl_comppad_name   (*__imp_Perl_comppad_name)
#define  Perl_comppad_name_fill   (*__imp_Perl_comppad_name_fill)
#define  Perl_cop_seqmax   (*__imp_Perl_cop_seqmax)
#define  Perl_curcop   (*__imp_Perl_curcop)
#define  Perl_curcopdb   (*__imp_Perl_curcopdb)
#define  Perl_curinterp   (*__imp_Perl_curinterp)
#define  Perl_curpad   (*__imp_Perl_curpad)
#define  Perl_dc   (*__imp_Perl_dc)
#define  Perl_di   (*__imp_Perl_di)
#define  Perl_ds   (*__imp_Perl_ds)
#define  Perl_egid   (*__imp_Perl_egid)
#define  Perl_envgv   (*__imp_Perl_envgv)
#define  Perl_error_count   (*__imp_Perl_error_count)
#define  Perl_euid   (*__imp_Perl_euid)
#define  Perl_evalseq   (*__imp_Perl_evalseq)
#define  Perl_expect   (*__imp_Perl_expect)
#define  Perl_fold_locale   (*__imp_Perl_fold_locale)
#define  Perl_gid   (*__imp_Perl_gid)
#define  Perl_he_root   (*__imp_Perl_he_root)
#define  Perl_hexdigit   (*__imp_Perl_hexdigit)
#define  Perl_hints   (*__imp_Perl_hints)
#define  Perl_in_my   (*__imp_Perl_in_my)
#define  Perl_last_lop   (*__imp_Perl_last_lop)
#define  Perl_last_lop_op   (*__imp_Perl_last_lop_op)
#define  Perl_last_uni   (*__imp_Perl_last_uni)
#define  Perl_lex_brackets   (*__imp_Perl_lex_brackets)
#define  Perl_lex_brackstack   (*__imp_Perl_lex_brackstack)
#define  Perl_lex_casemods   (*__imp_Perl_lex_casemods)
#define  Perl_lex_casestack   (*__imp_Perl_lex_casestack)
#define  Perl_lex_defer   (*__imp_Perl_lex_defer)
#define  Perl_lex_dojoin   (*__imp_Perl_lex_dojoin)
#define  Perl_lex_expect   (*__imp_Perl_lex_expect)
#define  Perl_lex_fakebrack   (*__imp_Perl_lex_fakebrack)
#define  Perl_lex_formbrack   (*__imp_Perl_lex_formbrack)
#define  Perl_lex_inpat   (*__imp_Perl_lex_inpat)
#define  Perl_lex_inwhat   (*__imp_Perl_lex_inwhat)
#define  Perl_lex_op   (*__imp_Perl_lex_op)
#define  Perl_lex_repl   (*__imp_Perl_lex_repl)
#define  Perl_lex_starts   (*__imp_Perl_lex_starts)
#define  Perl_lex_state   (*__imp_Perl_lex_state)
#define  Perl_lex_stuff   (*__imp_Perl_lex_stuff)
#define  Perl_linestr   (*__imp_Perl_linestr)
#define  Perl_markstack   (*__imp_Perl_markstack)
#define  Perl_markstack_max   (*__imp_Perl_markstack_max)
#define  Perl_markstack_ptr   (*__imp_Perl_markstack_ptr)
#define  Perl_max_intro_pending   (*__imp_Perl_max_intro_pending)
#define  Perl_maxo   (*__imp_Perl_maxo)
#define  Perl_min_intro_pending   (*__imp_Perl_min_intro_pending)
#define  Perl_multi_close   (*__imp_Perl_multi_close)
#define  Perl_multi_end   (*__imp_Perl_multi_end)
#define  Perl_multi_open   (*__imp_Perl_multi_open)
#define  Perl_multi_start   (*__imp_Perl_multi_start)
#define  Perl_na   (*__imp_Perl_na)
#define  Perl_nexttoke   (*__imp_Perl_nexttoke)
#define  Perl_nexttype   (*__imp_Perl_nexttype)
#define  Perl_nextval   (*__imp_Perl_nextval)
#define  Perl_nomemok   (*__imp_Perl_nomemok)
#define  Perl_numeric_local   (*__imp_Perl_numeric_local)
#define  Perl_numeric_name   (*__imp_Perl_numeric_name)
#define  Perl_numeric_standard   (*__imp_Perl_numeric_standard)
#define  Perl_oldbufptr   (*__imp_Perl_oldbufptr)
#define  Perl_oldoldbufptr   (*__imp_Perl_oldoldbufptr)
#define  Perl_op   (*__imp_Perl_op)
#define  Perl_op_desc   (*__imp_Perl_op_desc)
#define  Perl_op_name   (*__imp_Perl_op_name)
#define  Perl_op_seqmax   (*__imp_Perl_op_seqmax)
#define  Perl_opargs   (*__imp_Perl_opargs)
#define  Perl_origalen   (*__imp_Perl_origalen)
#define  Perl_origenviron   (*__imp_Perl_origenviron)
#define  Perl_osname   (*__imp_Perl_osname)
#define  Perl_padix   (*__imp_Perl_padix)
#define  Perl_patleave   (*__imp_Perl_patleave)
#define  Perl_pidstatus   (*__imp_Perl_pidstatus)
#define  Perl_ppaddr   (*__imp_Perl_ppaddr)
#define  Perl_profiledata   (*__imp_Perl_profiledata)
#define  Perl_psig_name   (*__imp_Perl_psig_name)
#define  Perl_psig_ptr   (*__imp_Perl_psig_ptr)
#define  Perl_regbol   (*__imp_Perl_regbol)
#define  Perl_regcode   (*__imp_Perl_regcode)
#define  Perl_regendp   (*__imp_Perl_regendp)
#define  Perl_regeol   (*__imp_Perl_regeol)
#define  Perl_reginput   (*__imp_Perl_reginput)
#define  Perl_reglastparen   (*__imp_Perl_reglastparen)
#define  Perl_regnaughty   (*__imp_Perl_regnaughty)
#define  Perl_regnpar   (*__imp_Perl_regnpar)
#define  Perl_regparse   (*__imp_Perl_regparse)
#define  Perl_regprecomp   (*__imp_Perl_regprecomp)
#define  Perl_regprev   (*__imp_Perl_regprev)
#define  Perl_regsawback   (*__imp_Perl_regsawback)
#define  Perl_regsize   (*__imp_Perl_regsize)
#define  Perl_regstartp   (*__imp_Perl_regstartp)
#define  Perl_regtill   (*__imp_Perl_regtill)
#define  Perl_regxend   (*__imp_Perl_regxend)
#define  Perl_retstack   (*__imp_Perl_retstack)
#define  Perl_retstack_ix   (*__imp_Perl_retstack_ix)
#define  Perl_retstack_max   (*__imp_Perl_retstack_max)
#define  Perl_rsfp   (*__imp_Perl_rsfp)
#define  Perl_rsfp_filters   (*__imp_Perl_rsfp_filters)
#define  Perl_savestack   (*__imp_Perl_savestack)
#define  Perl_savestack_ix   (*__imp_Perl_savestack_ix)
#define  Perl_savestack_max   (*__imp_Perl_savestack_max)
#define  Perl_scopestack   (*__imp_Perl_scopestack)
#define  Perl_scopestack_ix   (*__imp_Perl_scopestack_ix)
#define  Perl_scopestack_max   (*__imp_Perl_scopestack_max)
#define  Perl_scrgv   (*__imp_Perl_scrgv)
#define  Perl_sh_path   (*__imp_Perl_sh_path)
#define  Perl_sig_name   (*__imp_Perl_sig_name)
#define  Perl_sig_num   (*__imp_Perl_sig_num)
#define  Perl_siggv   (*__imp_Perl_siggv)
#define  Perl_stack_base   (*__imp_Perl_stack_base)
#define  Perl_stack_max   (*__imp_Perl_stack_max)
#define  Perl_stack_sp   (*__imp_Perl_stack_sp)
#define  Perl_statbuf   (*__imp_Perl_statbuf)
#define  Perl_sub_generation   (*__imp_Perl_sub_generation)
#define  Perl_subline   (*__imp_Perl_subline)
#define  Perl_subname   (*__imp_Perl_subname)
#define  Perl_sv_no   (*__imp_Perl_sv_no)
#define  Perl_sv_undef   (*__imp_Perl_sv_undef)
#define  Perl_sv_yes   (*__imp_Perl_sv_yes)
#define  Perl_tainting   (*__imp_Perl_tainting)
#define  Perl_thisexpr   (*__imp_Perl_thisexpr)
#define  Perl_timesbuf   (*__imp_Perl_timesbuf)
#define  Perl_tokenbuf   (*__imp_Perl_tokenbuf)
#define  Perl_uid   (*__imp_Perl_uid)
#define  Perl_vert   (*__imp_Perl_vert)
#define  Perl_vtbl_amagic   (*__imp_Perl_vtbl_amagic)
#define  Perl_vtbl_amagicelem   (*__imp_Perl_vtbl_amagicelem)
#define  Perl_vtbl_arylen   (*__imp_Perl_vtbl_arylen)
#define  Perl_vtbl_bm   (*__imp_Perl_vtbl_bm)
#define  Perl_vtbl_collxfrm   (*__imp_Perl_vtbl_collxfrm)
#define  Perl_vtbl_dbline   (*__imp_Perl_vtbl_dbline)
#define  Perl_vtbl_env   (*__imp_Perl_vtbl_env)
#define  Perl_vtbl_envelem   (*__imp_Perl_vtbl_envelem)
#define  Perl_vtbl_fm   (*__imp_Perl_vtbl_fm)
#define  Perl_vtbl_glob   (*__imp_Perl_vtbl_glob)
#define  Perl_vtbl_isa   (*__imp_Perl_vtbl_isa)
#define  Perl_vtbl_isaelem   (*__imp_Perl_vtbl_isaelem)
#define  Perl_vtbl_itervar   (*__imp_Perl_vtbl_itervar)
#define  Perl_vtbl_mglob   (*__imp_Perl_vtbl_mglob)
#define  Perl_vtbl_nkeys   (*__imp_Perl_vtbl_nkeys)
#define  Perl_vtbl_pack   (*__imp_Perl_vtbl_pack)
#define  Perl_vtbl_packelem   (*__imp_Perl_vtbl_packelem)
#define  Perl_vtbl_pos   (*__imp_Perl_vtbl_pos)
#define  Perl_vtbl_sig   (*__imp_Perl_vtbl_sig)
#define  Perl_vtbl_sigelem   (*__imp_Perl_vtbl_sigelem)
#define  Perl_vtbl_substr   (*__imp_Perl_vtbl_substr)
#define  Perl_vtbl_sv   (*__imp_Perl_vtbl_sv)
#define  Perl_vtbl_taint   (*__imp_Perl_vtbl_taint)
#define  Perl_vtbl_uvar   (*__imp_Perl_vtbl_uvar)
#define  Perl_vtbl_vec   (*__imp_Perl_vtbl_vec)
#define  Perl_xiv_arenaroot   (*__imp_Perl_xiv_arenaroot)
#define  Perl_xiv_root   (*__imp_Perl_xiv_root)
#define  Perl_xnv_root   (*__imp_Perl_xnv_root)
#define  Perl_xpv_root   (*__imp_Perl_xpv_root)
#define  Perl_xrv_root   (*__imp_Perl_xrv_root)
#define  ampergv   (*__imp_ampergv)
#define  argvgv   (*__imp_argvgv)
#define  argvoutgv   (*__imp_argvoutgv)
#define  basetime   (*__imp_basetime)
#define  beginav   (*__imp_beginav)
#define  bodytarget   (*__imp_bodytarget)
#define  cddir   (*__imp_cddir)
#define  chopset   (*__imp_chopset)
#define  comppad_name_floor   (*__imp_comppad_name_floor)
#define  copline   (*__imp_copline)
#define  curpm   (*__imp_curpm)
#define  curstack   (*__imp_curstack)
#define  curstash   (*__imp_curstash)
#define  curstname   (*__imp_curstname)
#define  cxstack   (*__imp_cxstack)
#define  cxstack_ix   (*__imp_cxstack_ix)
#define  cxstack_max   (*__imp_cxstack_max)
#define  dbargs   (*__imp_dbargs)
#define  debdelim   (*__imp_debdelim)
#define  debname   (*__imp_debname)
#define  debstash   (*__imp_debstash)
#define  debug   (*__imp_debug)
#define  defgv   (*__imp_defgv)
#define  defoutgv   (*__imp_defoutgv)
#define  defstash   (*__imp_defstash)
#define  delaymagic   (*__imp_delaymagic)
#define  diehook   (*__imp_diehook)
#define  dirty   (*__imp_dirty)
#define  dlevel   (*__imp_dlevel)
#define  dlmax   (*__imp_dlmax)
#define  do_undump   (*__imp_do_undump)
#define  doextract   (*__imp_doextract)
#define  doswitches   (*__imp_doswitches)
#define  dowarn   (*__imp_dowarn)
#define  dumplvl   (*__imp_dumplvl)
#define  e_fp   (*__imp_e_fp)
#define  e_tmpname   (*__imp_e_tmpname)
#define  endav   (*__imp_endav)
#define  errgv   (*__imp_errgv)
#define  eval_root   (*__imp_eval_root)
#define  eval_start   (*__imp_eval_start)
#define  fdpid   (*__imp_fdpid)
#define  filemode   (*__imp_filemode)
#define  firstgv   (*__imp_firstgv)
#define  forkprocess   (*__imp_forkprocess)
#define  formfeed   (*__imp_formfeed)
#define  formtarget   (*__imp_formtarget)
#define  gensym   (*__imp_gensym)
#define  in_eval   (*__imp_in_eval)
#define  incgv   (*__imp_incgv)
#define  inplace   (*__imp_inplace)
#define  last_in_gv   (*__imp_last_in_gv)
#define  lastfd   (*__imp_lastfd)
#define  lastscream   (*__imp_lastscream)
#define  lastsize   (*__imp_lastsize)
#define  lastspbase   (*__imp_lastspbase)
#define  laststatval   (*__imp_laststatval)
#define  laststype   (*__imp_laststype)
#define  leftgv   (*__imp_leftgv)
#define  lineary   (*__imp_lineary)
#define  localizing   (*__imp_localizing)
#define  localpatches   (*__imp_localpatches)
#define  main_cv   (*__imp_main_cv)
#define  main_root   (*__imp_main_root)
#define  main_start   (*__imp_main_start)
#define  mainstack   (*__imp_mainstack)
#define  maxscream   (*__imp_maxscream)
#define  maxsysfd   (*__imp_maxsysfd)
#define  minus_F   (*__imp_minus_F)
#define  minus_a   (*__imp_minus_a)
#define  minus_c   (*__imp_minus_c)
#define  minus_l   (*__imp_minus_l)
#define  minus_n   (*__imp_minus_n)
#define  minus_p   (*__imp_minus_p)
#define  multiline   (*__imp_multiline)
#define  mystack_base   (*__imp_mystack_base)
#define  mystack_max   (*__imp_mystack_max)
#define  mystack_sp   (*__imp_mystack_sp)
#define  mystrk   (*__imp_mystrk)
#define  nice_chunk   (*__imp_nice_chunk)
#define  nice_chunk_size   (*__imp_nice_chunk_size)
#define  nrs   (*__imp_nrs)
#define  ofmt   (*__imp_ofmt)
#define  ofs   (*__imp_ofs)
#define  ofslen   (*__imp_ofslen)
#define  oldlastpm   (*__imp_oldlastpm)
#define  oldname   (*__imp_oldname)
#define  op_mask   (*__imp_op_mask)
#define  origargc   (*__imp_origargc)
#define  origargv   (*__imp_origargv)
#define  origfilename   (*__imp_origfilename)
#define  ors   (*__imp_ors)
#define  orslen   (*__imp_orslen)
#define  pad_reset_pending   (*__imp_pad_reset_pending)
#define  padix_floor   (*__imp_padix_floor)
#define  parsehook   (*__imp_parsehook)
#define  patchlevel   (*__imp_patchlevel)
#define  perl_destruct_level   (*__imp_perl_destruct_level)
#define  perldb   (*__imp_perldb)
#define  preambleav   (*__imp_preambleav)
#define  preambled   (*__imp_preambled)
#define  preprocess   (*__imp_preprocess)
#define  regflags   (*__imp_regflags)
#define  restartop   (*__imp_restartop)
#define  rightgv   (*__imp_rightgv)
#define  rs   (*__imp_rs)
#define  runlevel   (*__imp_runlevel)
#define  sawampersand   (*__imp_sawampersand)
#define  sawstudy   (*__imp_sawstudy)
#define  sawvec   (*__imp_sawvec)
#define  screamfirst   (*__imp_screamfirst)
#define  screamnext   (*__imp_screamnext)
#define  secondgv   (*__imp_secondgv)
#define  signalstack   (*__imp_signalstack)
#define  sortcop   (*__imp_sortcop)
#define  sortstack   (*__imp_sortstack)
#define  sortstash   (*__imp_sortstash)
#define  splitstr   (*__imp_splitstr)
#define  statcache   (*__imp_statcache)
#define  statgv   (*__imp_statgv)
#define  statname   (*__imp_statname)
#define  statusvalue   (*__imp_statusvalue)
#define  stdingv   (*__imp_stdingv)
#define  strchop   (*__imp_strchop)
#define  strtab   (*__imp_strtab)
#define  sv_arenaroot   (*__imp_sv_arenaroot)
#define  sv_count   (*__imp_sv_count)
#define  sv_objcount   (*__imp_sv_objcount)
#define  sv_root   (*__imp_sv_root)
#define  tainted   (*__imp_tainted)
#define  tmps_floor   (*__imp_tmps_floor)
#define  tmps_ix   (*__imp_tmps_ix)
#define  tmps_max   (*__imp_tmps_max)
#define  tmps_stack   (*__imp_tmps_stack)
#define  top_env   (*__imp_top_env)
#define  toptarget   (*__imp_toptarget)
#define  unsafe   (*__imp_unsafe)
#define  warnhook   (*__imp_warnhook)
