LATEX=latex
PDFLATEX=pdflatex

TARGET=guidelines_en

default : $(TARGET).pdf

%.pdf : %.tex
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null 
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null

clean :
	-rm -f *.aux *.toc *.log *.out *.pdf *.dvi *.bbl *.blg *.haux *.htoc 

