#Garcia Lugo Maria Fernanda
#Mendez Larios Emilio
#Rivas Gutierrez Carlos Andres
#Villegas Aguilar Carlos

#Definicion de la macros

CC=gcc#Compilador
CFLAGS= -Wall -fopenmp#Banderas del compilador
LIBS=-fopenmp#Librerias

#Reglas de inferencia

Proyecto:GaussJ.o
	$(CC) $(LIBS) -o Proyecto GaussJ.o


%.o:%.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm *.o Proyecto
