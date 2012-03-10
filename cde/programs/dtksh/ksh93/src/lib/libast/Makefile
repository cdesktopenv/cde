/* $XConsortium: Makefile /main/3 1996/05/10 17:20:03 drk $ */
/*
 * ast library
 */

ID = ast
VERSION = 4.0
CCFLAGS = -O $(CC.PIC)

:PACKAGE: $(ID)

.SOURCE : obsolete comp dir disc hash man misc path port \
		preroot re sfio stdio string tm vec vmalloc

.SOURCE.h : include

PARANOID == "make"
DEBUG ==

HEADERSRC = ast.h ast_dir.h ast_std.h debug.h error.h find.h \
	ftwalk.h fs3d.h hash.h hashpart.h ls.h magic.h \
	modecanon.h modex.h namval.h option.h proc.h re.h sfio.h \
	sfdisc.h sfstr.h stack.h stak.h stdio.h stk.h swap.h tar.h \
	times.h tm.h tok.h vdb.h vecargs.h vmalloc.h wait.h

HEADERGEN = align.h int.h limits.h preroot.h sig.h \
	$(ID)_botch.h $(ID)_fcntl.h $(ID)_fs.h $(ID)_hdr.h \
	$(ID)_lib.h $(ID)_mode.h $(ID)_param.h $(ID)_time.h \
	$(ID)_tty.h $(ID)_types.h $(ID)_unistd.h $(ID)_wait.h

$(ID) $(VERSION) :LIBRARY: README RELEASE \
	LIBAST.3 ast.3 chr.3 compatibility.3 error.3 find.3 fmt.3 \
	fmtls.3 fs3d.3 ftwalk.3 getcwd.3 hash.3 iblocks.3 int.3 \
	magic.3 mem.3 modecanon.3 optget.3 path.3 preroot.3 proc.3 \
	re.3 setenviron.3 sfdisc.3 sfio.3 sig.3 spawnveg.3 stak.3 \
	stk.3 strcopy.3 strdup.3 strelapsed.3 strerror.3 stresc.3 \
	streval.3 strgid.3 strmatch.3 stropt.3 strperm.3 strsignal.3 \
	strsort.3 strtape.3 strton.3 struid.3 swap.3 tab.3 tm.3 \
	tok.3 touch.3 vecargs.3 vmalloc.3 \
	$(HEADERSRC) $(HEADEROLD) \
	state.c \
	dirlib.h opendir.c readdir.c rewinddir.c seekdir.c telldir.c \
	getcwd.c fastfind.c \
	hashlib.h hashalloc.c hashdump.c hashfree.c hashlast.c \
	hashlook.c hashscan.c hashsize.c hashwalk.c \
	memhash.c memsum.c strhash.c strsum.c \
	mematoe.c memetoa.c \
	chresc.c chrtoi.c streval.c strexpr.c strmatch.c strcopy.c \
	modelib.h modei.c modex.c strmode.c strlook.c \
	stresc.c stropt.c strtape.c tok.c tokline.c tokscan.c \
	pathaccess.c pathcat.c pathcanon.c pathcheck.c pathpath.c \
	pathkey.c pathprobe.c pathrepl.c pathtemp.c pathstat.c \
	pathgetlink.c pathsetlink.c pathbin.c pathshell.c pathcd.c \
	fs3d.c ftwalk.c ftwflags.c setenviron.c \
	optget.c optjoin.c strsort.c \
	astcopy.c astconf.c astquery.c astwinsize.c \
	magic.c signal.c systrace.c \
	error.c liberror.c \
	fmtbase.c fmtdev.c fmtelapsed.c fmterror.c fmtesc.c fmtfs.c \
	fmtls.c fmtmatch.c fmtmode.c fmtperm.c fmtre.c \
	fmttime.c fmtuid.c fmtgid.c fmtsignal.c \
	strelapsed.c strperm.c struid.c strgid.c stack.c stk.c \
	swapget.c swapmem.c swapop.c swapput.c \
	sigdata.c sigcrit.c sigunblock.c \
	proclib.h procopen.c procclose.c procrun.c procfree.c \
	relib.h recomp.c reerror.c reexec.c resub.c ressub.c \
	tmdate.c tmfix.c tmfmt.c tmform.c tmgoff.c \
	tminit.c tmleap.c tmlex.c tmmake.c tmpoff.c \
	tmtime.c tmtype.c tmword.c tmzone.c \
	vecargs.c vecfile.c vecfree.c vecload.c vecstring.c \
	vmhdr.h vmbest.c vmclear.c vmclose.c vmdcheap.c \
	vmdcsbrk.c vmdebug.c vmdisc.c vmexit.c vmlast.c vmopen.c \
	vmpool.c vmprivate.c vmprofile.c vmregion.c vmsegment.c \
	vmset.c vmstat.c vmstrdup.c vmtrace.c \
	univlib.h univdata.c touch.c \
	memccpy.c memchr.c memcmp.c memcpy.c memdup.c memmove.c memset.c \
	mkdir.c mkfifo.c mknod.c rmdir.c remove.c rename.c link.c unlink.c \
	strdup.c strchr.c strrchr.c strtod.c strtol.c strtoul.c strton.c \
	setlocale.c mktemp.c tmpnam.c \
	execve.c spawnve.c spawnveg.c vfork.c killpg.c \
	lstat.c readlink.c statvfs.c symlink.c \
	getpgrp.c setpgid.c setsid.c waitpid.c \
	confmap.c conftab.c confstr.c pathconf.c sysconf.c \
	fcntl.c open.c \
	atexit.c getdents.c getwd.c dup2.c errno.c \
	getpreroot.c ispreroot.c realopen.c setpreroot.c \
	getgroups.c mount.c system.c iblocks.c \
	modedata.c tmdata.c \
	sfkeyprintf.c sfslowio.c \
	/* sfio */ \
	_sfhdr.h sfhdr.h \
	sfclose.c sfclrlock.c sfdisc.c sfdlen.c sfexcept.c \
	sfcvt.c sfecvt.c sffcvt.c \
	sfextern.c sffilbuf.c sfflsbuf.c sfprints.c sfgetd.c sfgetl.c \
	sfgetr.c sfgetu.c sfllen.c sfmode.c sfmove.c sfnew.c \
	sfpkrd.c sfnotify.c sfnputc.c sfopen.c sfpeek.c sfpoll.c \
	sfpool.c sfpopen.c sfprintf.c sfputd.c sfputl.c sfputr.c \
	sfputu.c sfrd.c sfread.c sfreserve.c sfscanf.c sfseek.c sfset.c \
	sfsetbuf.c sfsetfd.c sfsize.c sfsk.c sfstack.c sfstrtod.c sfsync.c \
	sfswap.c sftable.c sftell.c sftmp.c sfungetc.c sfvprintf.c \
	sfvscanf.c sfwr.c sfwrite.c sfpurge.c \
	_sfclrerr.c _sfeof.c _sferror.c _sffileno.c \
	_sfgetc.c _sfgetl.c _sfgetu.c _sfstacked.c _sfputc.c _sfputd.c \
	_sfputl.c _sfputu.c _sfslen.c _sfulen.c \
	stdgets.c stdprintf.c stdscanf.c stdvbuf.c stdsprnt.c \
	stdvsprnt.c stdvsnprnt.c stdvsscn.c stdopen.c \
	/* obsolete */ \
	fdcopy.c getpath.c getshell.c getsymlink.c getuniv.c \
	getwinsize.c hsort.c lpstat.c putsymlink.c query.c \
	setcwd.c setenv.c setuniv.c slowio.c strerror.c \
	strsignal.c tablook.c astfeature.c

:INSTALLDIR: conf

conf :: conf.sh

conflib.h conflim.h confmap.h conftab.h confuni.h \
confmap.c conftab.c confstr.c pathconf.c sysconf.c :JOINT: \
		conf conf.tab
	: generate : $(<:N=*.h)
	: generate : $(<:N=*.c)
	$(*) $(CC) $(CCFLAGS:N!=-[DIU]*)

/* NOTE: sun4 runtime link botches ro data so advertized sig_info is rw */

:READONLY: confmap.c conftab.c modedata.c sftable.c \
	/*sigdata.c*/ tmdata.c univdata.c

$(INCLUDEDIR) :INSTALLPROTO: $(HEADERSRC) $(HEADERGEN)

$(HEADERGEN) :COPY: FEATURE/$$(<:B:/$(ID)_//)

$(LIBDIR)/file/magic :INSTALL: magic.tab

/* dirent.h is problematic */

:INSTALL: $(INCLUDEDIR)/dirent.h

$(INCLUDEDIR)/dirent.h : dirstd.h dirnsys.h dirsys.h ast_lib.h
	if	grep _hdr_dirent $(*:N=*ast_lib.h) >/dev/null 2>&1
	then	:
	elif	grep _hdr_ndir $(*:N=*ast_lib.h) >/dev/null 2>&1 && grep _lib_opendir $(*:N=*ast_lib.h) >/dev/null 2>&1
	then	$(CP) $(*:N=*dirnsys.h) $(<)
	elif	grep _sys_dir $(*:N=*ast_lib.h) >/dev/null 2>&1 && grep _lib_opendir $(*:N=*ast_lib.h) >/dev/null 2>&1
	then	$(CP) $(*:N=*dirsys.h) $(<)
	else	$(CP) $(*:N=*dirstd.h) $(<)
	fi

if _just_the_std

std $(VERSION) :LIBRARY: \
	state.c \
	opendir.c readdir.c rewinddir.c seekdir.c telldir.c \
	getcwd.c setcwd.c \
	memccpy.c memchr.c memcmp.c memcpy.c memdup.c memmove.c memset.c \
	mkdir.c mkfifo.c mknod.c rmdir.c remove.c rename.c link.c unlink.c \
	strchr.c strrchr.c strtod.c strtol.c strtoul.c \
	setlocale.c signal.c \
	mktemp.c tmpnam.c \
	execve.c spawnve.c vfork.c killpg.c \
	lstat.c readlink.c statvfs.c symlink.c \
	getpgrp.c setpgid.c setsid.c waitpid.c \
	pathconf.c sysconf.c fcntl.c open.c \
	atexit.c getdents.c dup2.c errno.c \
	getgroups.c mount.c system.c

std.h :COPY: ast_std.h

$(INCLUDEDIR) :INSTALLPROTO: std.h

end
