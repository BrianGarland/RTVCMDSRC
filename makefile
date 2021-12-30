NAME=Retrieve Command Source
BIN_LIB=RTVCMDSRC
DBGVIEW=*SOURCE
TGTRLS=V7R1M0
SHELL=/QOpenSys/usr/bin/qsh

#----------

all: $(BIN_LIB).lib rtvmsg.c rtvcmd.c rtvcmdm.mi rtvcmdr.rpg rtvcmdc.clp rtvmsgc.clp rtvcmdu.pnlgrp rtvcmdsrc.cmd rtvcmdmsg.cmd  
	@echo "Built all"

#----------

%.c:
	system "CRTBNDC PGM($(BIN_LIB)/$*) SRCSTMF('QSOURCE/$*.c') OUTPUT(*PRINT) DBGVIEW($(DBGVIEW)) TGTRLS($(TGTRLS)) INCDIR('QSOURCE')"	
	
rtvcmdm.mi:
	liblist -a CRTMIPGM;\
	system "CRTMIPGM SRC('QSOURCE/rtvcmdm.mi') PGM('RTVCMDM   $(BIN_LIB)') TXT('$(NAME)')"
		
%.pnlgrp:
	system "CPYFRMSTMF FROMSTMF('QSOURCE/$*.pnlgrp') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSOURCE.file/$*.mbr') MBROPT(*REPLACE)"
	system "CRTPNLGRP PNLGRP($(BIN_LIB)/$*) SRCFILE($(BIN_LIB)/QSOURCE) SRCMBR($*) REPLACE(*YES) TEXT('$(NAME)')"	
	
%.rpg:
	system "CPYFRMSTMF FROMSTMF('QSOURCE/$*.rpg') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSOURCE.file/$*.mbr') MBROPT(*REPLACE)"
	system "CRTRPGPGM PGM($(BIN_LIB)/$*) SRCFILE($(BIN_LIB)/QSOURCE) SRCMBR($*) REPLACE(*YES) TEXT('$(NAME)') TGTRLS($(TGTRLS))"	
	
%.rpgle:
	system "CRTBNDRPG PGM($(BIN_LIB)/$*) SRCSTMF('QSOURCE/$*.rpgle') TEXT('$(NAME)') REPLACE(*YES) DBGVIEW($(DBGVIEW)) TGTRLS($(TGTRLS))"
	
%.clp:
	system "CPYFRMSTMF FROMSTMF('QSOURCE/$*.clp') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSOURCE.file/$*.mbr') MBROPT(*REPLACE)"
	system "CRTCLPGM PGM($(BIN_LIB)/$*) SRCFILE($(BIN_LIB)/QSOURCE) SRCMBR($*) REPLACE(*YES) TEXT('$(NAME)') TGTRLS($(TGTRLS))"	
	
rtvcmdsrc.cmd:
	system "CPYFRMSTMF FROMSTMF('QSOURCE/rtvcmdsrc.cmd') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSOURCE.file/RTVCMDSRC.mbr') MBROPT(*REPLACE)"
	system "CRTCMD CMD($(BIN_LIB)/RTVCMDSRC) PGM($(BIN_LIB)/RTVCMDC) SRCFILE($(BIN_LIB)/QSOURCE) SRCMBR($*) TEXT('$(NAME)')"
	
rtvcmdmsg.cmd:
	system "CPYFRMSTMF FROMSTMF('QSOURCE/rtvcmdmsg.cmd') TOMBR('/QSYS.lib/$(BIN_LIB).lib/QSOURCE.file/RTVCMDMSG.mbr') MBROPT(*REPLACE)"
	system "CRTCMD CMD($(BIN_LIB)/RTVCMDMSG) PGM($(BIN_LIB)/RTVMSGC) SRCFILE($(BIN_LIB)/QSOURCE) SRCMBR($*) TEXT('$(NAME)')"
	
%.lib:
	-system -qi "CRTLIB LIB($(BIN_LIB)) TEXT('$(NAME)')"
	-system -qi "CRTSRCPF FILE($(BIN_LIB)/QSOURCE) MBR($*) RCDLEN(198)"
	system "CRTSRCPF FILE($(BIN_LIB)/RTVCMDS) RCDLEN(112)"
	system "ADDPFM FILE($(BIN_LIB)/RTVCMDS) MBR(CMD) TEXT('Prototype for CMD src mbr') SRCTYPE(CMD)"
	system "ADDPFM FILE($(BIN_LIB)/RTVCMDS) MBR(CLP) TEXT('Prototype for CLP src mbr') SRCTYPE(CLP)"

clean:
	system "CLRLIB LIB($(BIN_LIB))"

erase:
	-system -qi "DLTLIB LIB($(BIN_LIB))"	
