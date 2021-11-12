#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>


int main()
{

    int n, i;
    FILE *archie;
    srand(time(NULL));
    printf("renglones de la matriz: ");
    scanf("%d", &n);

    archie=fopen("Datos.txt", "r");
    if(!archie)
    {
    	  system("touch Datos.txt");
    	  fclose(archie);
    }
    fclose(archie);
     
    sleep(5);
    archie=fopen("Datos.txt", "w");
    if(archie){
        fprintf(archie, "%d\n", n);
        fprintf(archie, "%d\n", n+1);
        for(i=0;i<(n*(n+1));i++)
        {
            fprintf(archie, "%d\n", (rand()%5)+1);
        }
    }
    fclose(archie);
    
   return 0;
}

   
