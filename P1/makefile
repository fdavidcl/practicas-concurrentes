## makefile prácticas SCD (CUA, octubre 2013)
##
## compila todos los fuentes C y C++ (.c, .cc y .cpp)
## crea un ejecutable enlazando con 'pthread' y 'rt'
## modificar "ejecutable" para crear un ejecutable con otro nombre
##
## usar:
##
##    make (o make x) - compilar y ejecutar
##    make comp       - compilar (también con make "ejecutable")
##    make clean      - borrar ejecutable y archivos .o



.PHONY: x, clean, comp
.SUFFIXES:
.SUFFIXES: .o .c .cc .cpp

ejecutable:=  ejemplos_se1
params_comp:= -g -Wall -I.
libs:=        -lpthread -lrt
unidades:=    $(wildcard *.cc) $(wildcard *.cpp) $(wildcard *.c) 
fuentes:=     $(unidades) $(wildcard *.h) $(wildcard *.hpp)
unidades_o:=  $(addsuffix .o, $(basename $(unidades))) 


x:
	@echo "--------------------------------------------------------"
	@echo "SCD 13-14. Archivos de seminarios y prácticas (makefile)"
	@echo "--------------------------------------------------------"
	@make --no-print-directory comp
	./$(ejecutable)

comp: $(ejecutable)

$(ejecutable): $(unidades_o)
	g++ -o $(ejecutable) $(unidades_o)  $(libs) 

$(unidades_o): $(fuentes) 

.cc.o:
	g++ $(params_comp) -c $<

.cpp.o:
	g++ $(params_comp) -c $<

.c.o:
	gcc $(params_comp) -c $<

clean:
	rm -rf *.o $(ejecutable)










