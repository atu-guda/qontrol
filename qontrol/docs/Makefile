LATEX=lualatex

all : struct0.pdf

%.pdf : %.tex
	$(LATEX) $^
	$(LATEX) $^

clean:
	rm -f *.aux *.log *.toc
allclean: clean
	rm -f *.pdf

.PHONY: clean allclean

