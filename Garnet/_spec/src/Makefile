
TARGETS=Garnet.pdf PicturePerfectEngine.pdf

all: $(TARGETS)

.SUFFIXES: .tex .pdf
.tex.pdf:
	platex $*
	dvipdfmx ${*:.tex=.dvi}

clean:
	rm -f *.pdf
	rm -f *.aux
	rm -f *.log
	rm -f *.pbm
