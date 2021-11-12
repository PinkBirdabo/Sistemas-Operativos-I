#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


/**************Código princiapal**********/
int main()
{
    //Declracion de variables
    int n, i;
    FILE *archie;
    srand(time(NULL));//Para asignar valores diferentes 
	

    printf("renglones de la matriz: ");
    scanf("%d", &n);

    archie=fopen("Datos.txt", "r");//Se abre el archivo para leerlo
    if(!archie)//Validiacion de si no existe el archivo para crearlo
    {
    	  system("touch Datos.txt");
    	   fclose(archie);
    }
     
    sleep(2);
    archie=fopen("Datos.txt", "w");//Se abre el archivo para escribir
    if(archie){
        fprintf(archie, "%d\n", n);
        fprintf(archie, "%d\n", n+1);
        for(i=0;i<(n*(n+1));i++)
        {
            fprintf(archie, "%d\n", (rand()%5)+1);//Se asignan los valores 
        }
    }
    fclose(archie);//Se cierra el archivo
    
   return 0;
}

   
