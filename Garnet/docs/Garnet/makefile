NAME=Garnet

DEPENDS=\
	Algorithm.txt\
	ReferenceManual.txt\
	GenerationLog.txt

PDF=$(NAME).pdf
TEX=$(NAME).tex
HTML=$(NAME).html
SOURCE=$(NAME).txt
PREPROCESSED=$(NAME).txt.i
PANDOC=pandoc
PDFTEX=lualatex
CPP=gcc -E -P

all: $(PDF)

pdf: $(PDF)

html: $(HTML)

$(PDF): $(TEX)
	$(PDFTEX) $(TEX)
#	$(PANDOC) --latex-engine=lualatex --from=markdown-raw_html --self-contained --toc -o $(PDF) $(SOURCE)
	
$(TEX): $(SOURCE)
	$(PANDOC) --latex-engine=lualatex --from=markdown-raw_html --variable=documentclass:ltjltxdoc --to=latex --self-contained --toc -o $(TEX) $(SOURCE)

$(HTML): $(SOURCE)
	$(PANDOC) --from=markdown-raw_html --to=html --self-contained --toc -o $(HTML) $(SOURCE)

$(SOURCE): $(DEPENDS)
	cat $(DEPENDS) > $(SOURCE)

clean:
	rm $(SOURCE)
	rm $(PDF)
	rm $(TEX)
	