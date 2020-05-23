ifeq (${TASM_BIN},)
 $(error Environment variable TASM_BIN is not defined)
endif

inpthex.asm:

INPTHEX.EXE: make.conf inpthex.asm
	dosbox -conf ./make.conf
	cat ./TASM.LOG
	cat ./TLINK.LOG

make.conf:
	../scripts/generate_make_conf.sh ${TASM_BIN} ./inpthex.asm ./make.conf

debug.conf:
	../scripts/generate_debug_conf.sh ${TASM_BIN} ./INPTHEX.EXE ./debug.conf

run.conf:
	../scripts/generate_run_conf.sh ./inpthex.exe ./run.conf

debug: debug.conf INPTHEX.EXE
	dosbox -conf ./debug.conf

run: run.conf INPTHEX.EXE
	dosbox -conf ./run.conf

clean:
	rm ./*.conf ./*.EXE ./*.MAP ./*.OBJ ./*.LOG
