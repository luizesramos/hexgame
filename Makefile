PROG = hex
FLAG = -std=c++0x

all: ${PROG}

${PROG}: 
	g++ ${FLAG} ${PROG}.cpp -o p${PROG}

mst:
	g++ ${FLAG} mst.cpp -o pmst

gen:
	g++ gen.cpp -o pgen

graph:
	g++ graph.cpp -o pgraph

clean:
	rm -f c${PROG} p${PROG} *~ pgraph pgen pmst
