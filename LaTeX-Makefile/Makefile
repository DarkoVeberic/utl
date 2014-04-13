# Makefile for building latex documents (c) Darko Veberic

# pdflatex or latex?
USEPDFLATEX := 1
# needs bibtex?
USEBIBTEX := 0

# with a single .tex file in the current directory no need to specify target
# in case of multiple .tex files explicitly name the target (but without the .tex extension)
TARGET := $(basename $(wildcard *.tex))
# add other dependencies in various places
DEPENDENCIES := $(wildcard figs/*)

# don't modify below this line
##########################################################

LATEX := $(if $(filter 1, $(USEPDFLATEX)), pdflatex, latex)

COLORIZE := awk 'BEGIN{red="\033[31;1m";yel="\033[34m";byel="\033[34;1m";end="\033[0m"}/Rerun /||/rerunfilecheck[^.]/{e=e$$0"\n";print red""$$0""end;next}/^LaTeX Warning:/||/natbib Warning:/{w=w$$0"\n";print yel""$$0""end;next}{print}END{if(w||e)print "\n"red"Warning/error summary:\n"byel""w""end;if(e){print red""e""end;exit 1}}'

define basic_latex_rule
  for i in 1 2 3 ; do $(LATEX) $< | $(COLORIZE) && break ; done
endef

ifeq ($(USEBIBTEX), 1)
define latex_rule
  $(LATEX) $< ; bibtex $*
  $(call basic_latex_rule)
endef
else
define latex_rule
  $(call basic_latex_rule)
endef
endif

.PHONY: all clean

all: $(TARGET).pdf

# dependencies
$(TARGET).pdf: $(DEPENDENCIES)

clean:
	- for e in .tex~ .ps .pdf .dvi .log .aux .toc .nav .out .snm .bbl .blg .spl .lof Notes.bib .maf .fff .lot .ttt ; do rm $(TARGET)$$e ; done

# general rules
ifeq ($(USEPDFLATEX), 1)
%.pdf %.dvi: %.tex $(DEPENDENCIES)
	$(call latex_rule)

%.ps: %.pdf
	pdf2ps $<
else
%.dvi: %.tex $(DEPENDENCIES)
	$(call latex_rule)

%.pdf: %.ps
	ps2pdf $<

%.ps: %.dvi
	dvips $< -o
endif

# embed pdf fonts; needed by some journals for pdf-only submissions
%.embedded_fonts.pdf: %.pdf
	gs -dSAFER -dNOPLATFONTS -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sPAPERSIZE=a4 -dCompatibilityLevel=1.4 -dPDFSETTINGS=/printer -dCompatibilityLevel=1.4 -dMaxSubsetPct=100 -dSubsetFonts=true -dEmbedAllFonts=true -sOutputFile=$@ -f $<
