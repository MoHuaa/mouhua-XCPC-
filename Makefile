RM = del

PANDOC = pandoc
PYTHON = python
PFLAGS = --pdf-engine=xelatex --pdf-engine-opt=-shell-escape --toc -V mainfont="Times New Roman" -V CJKmainfont="SimSun" --listings

TARGET = output.pdf
MARKDOWN_TARGET = .\output.md
INTER_TARGET = .\output_tmp.md

GEN = .\NewGenerater.py
CONFIG = .\config.json

.PHONY: clean all

default: all

${INTER_TARGET}:
	${PYTHON} ${GEN} -f ${CONFIG} -t ${INTER_TARGET} -o ${MARKDOWN_TARGET}

${TARGET}: ${INTER_TARGET}
	${PANDOC} ${INTER_TARGET} -o ${TARGET} ${PFLAGS}
	${RM} ${INTER_TARGET}

clean:
	${RM} ${INTER_TARGET}
	${RM} ${TARGET}

all: clean ${INTER_TARGET} ${TARGET}
