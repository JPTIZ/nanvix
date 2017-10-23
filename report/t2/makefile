OUTPUT := nanvix-report
REFS   := references
LATEX  := xelatex

default: $(OUTPUT).tex $(REFS).bib
	$(LATEX) --shell-escape $(OUTPUT)
	bibtex $(OUTPUT)
	$(LATEX) --shell-escape $(OUTPUT)
	$(LATEX) --shell-escape $(OUTPUT)

clean:
	rm -f *.{toc,aux,bbl,out}
	rm -f $(OUTPUT).pdf
