#
# despachadora de calculadora
#

all: clean despachador child

clean:	
	rm -rf despachador child

despachador: despachador.c
child: child.c
