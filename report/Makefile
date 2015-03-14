type = report

all: $(type).tex
	(cd graphics; make all)
	pdflatex $(type)
	#bibtex $(type)
	pdflatex $(type)
	pdflatex $(type)
	cp $(type).pdf ~/Desktop

clean:
	-rm -f $(type).aux $(type).dvi $(type).log $(type).pdf  $(type).ps $(type).bbl $(type).blg

