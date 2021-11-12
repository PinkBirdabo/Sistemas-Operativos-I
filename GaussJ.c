#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

/**************Declaración de funciones**********/
void crea_matriz(long double **,int,int);
void libera_matriz(long double **);
void imprime_matriz(long double *,int,int);
void llena_matriz(long double *,int, int,FILE*);
short int gauss_jordan(long double *,int,int);
short int pgauss_jordan(long double *,int,int,int *);


/**************Código princiapal**********/
int main()
{
   //Declaracion de variables
   int r,c,max;
   int *no;
   int no_hilos;
   double tsec,tpa;
   no=&no_hilos;
   struct timespec s1,s2,p1,p2;
   long double *ap_matriz,*ap_matriz2;
   FILE *archivo;
   
   //Se lee del archivo los renglones y columnas que tendrán las matrices
   archivo=fopen("Datos.txt","r");
   fscanf(archivo,"%d",&r);
   fscanf(archivo,"%d",&c);
   fclose(archivo);
   ap_matriz=NULL;

  //Se llama a la función para crear las matrices de manera dinámica
  crea_matriz(&ap_matriz,r,c);
  crea_matriz(&ap_matriz2,r,c);
  
  //Se llama a la función para llenar a las matrices a partir del archivo de entrada
  llena_matriz(ap_matriz,r,c,archivo);
  llena_matriz(ap_matriz2,r,c,archivo);
  
  //Impresion de matrices iniciales
  //printf("\nMatriz Secuencial:\n");
  //imprime_matriz(ap_matriz,r,c);
   //printf("\nMatriz Paralela:\n");
  //imprime_matriz(ap_matriz2,r,c);
  
  //Se calcula el tiempo de ejecución de Gauss-Jordan secuencial
  clock_gettime(CLOCK_MONOTONIC,&s1);
  gauss_jordan(ap_matriz,r,c);//Se llama a la función que resuleve Gauss-Jordan
  clock_gettime(CLOCK_MONOTONIC,&s2);
  //Se imprime la matriz final de Gauss-Jordan secuencial
   printf("\nSecuencial\n");
  //imprime_matriz(ap_matriz,r,c);
  libera_matriz(&ap_matriz);//Se llama a la función para liberar el apuntador a la matriz
  
  //Se calcula el tiempo de ejecución de Gauss-Jordan paralelo 
  clock_gettime(CLOCK_MONOTONIC,&p1);
  pgauss_jordan(ap_matriz2,r,c,no);//Se llama a la función que resuleve Gauss-Jordan de manera paralela
  clock_gettime(CLOCK_MONOTONIC,&p2);
  //Se imprime la matriz final de Gauss-Jordan paralelo
  printf("\nParalelo\n");
  //imprime_matriz(ap_matriz2,r,c);
  libera_matriz(&ap_matriz2);//Se llama a la función para liberar el apuntador a la matriz
  
  //Se realiza la impresión de los tiempos de ejecución, el número de hilo usados y el máximo de hilos
  printf("\nTamaño de la matriz:%d\n",r);
  printf("\nNumero de hilos:%d\n",no_hilos);
  max=omp_get_max_threads();
  printf("\nNumero maximo de hilos: %d\n",max);
  tsec=(double)(s2.tv_sec-s1.tv_sec)+((double)(s2.tv_nsec-s1.tv_nsec)/1000000000L);
  printf("\nTiempo secuencial: %lf \n",tsec);
   tpa=(double)(p2.tv_sec-p1.tv_sec)+((double)(p2.tv_nsec-p1.tv_nsec)/1000000000L);
  printf("\nTiempo paralelo: %lf\n",tpa);
  
   return 0;
   
}

void crea_matriz(long double **ap_matriz,int r,int c)
/*Crea la matriz de manera dinámica
Recibe: doble apuntador a la matriz, el número de renglones y de columnas
Envía:void*/
{
  *ap_matriz=(long double *)malloc((r*c)*sizeof(long double));//Utilización de malloc para la creación de la matriz
}



short int pgauss_jordan(long double *ap_matriz,int r,int c,int *no)
/*Realiza el algoritmo de Gauss-Jordan de manera paralela
Recibe: apuntador a la matriz, el número de renglones y de columnas y un apuntador para el obtener el número de hilos empleados
Envía:short int para ya no despelgar las raices en caso de que el sistema de ecuaciones no tenga solucion*/
{
        
        int k, i,j;
        long double cons,pib;
       
       //Ciclo que asigna el pibote 
        for ( i = 0;i < r;i++ )
        {
            
            //Se divide al renglón entre el pibote para que este sea 1
            pib=*(ap_matriz+(i+(c*i)));
            for( j = 0;j < c;j++ )
            {
            	*(ap_matriz+(j+(c*i)))=*(ap_matriz+(j+(c*i))) / pib;
            }
            
            //Ciclo que controla los renglones
            //Se establece que el for sea paralelo, el número de hilos a crear, se vuelve privadas las variables j,k y cons para evitar
            //problemas debido a la concurrencia
            #pragma omp parallel for num_threads(4) private(j) private (k) private(cons)
            for ( j = 0;j < r;j++ )
            {
                cons=*(ap_matriz+(i+(c*j)));//Se establece la constante por la cual se va a multiplicar el pibote 
                //Ciclo que controla las columnas
                for ( k= 0;k < c;k++ )
                {
                    if ( j != i )//En caso de ser el renglón que contiene el pibote no se realiza la operación
                    {
                     	//Se realiza la resta del reglón actual con el renglón del pibote
                        *(ap_matriz+(k+(c*j)))=((*(ap_matriz+(i+(c*i))))*(*(ap_matriz+(k+(c*j)))))-(cons*(*(ap_matriz+(k+(c*i)))));
                        *no=omp_get_num_threads();//Se obtiene el número de hilos que se utilizaron
                   
                    }
                   
                }
            }
             //imprime_matriz(ap_matriz,r,c);
        }

	//Impresión de las raíces 
        printf("\nSolucion Paralela:" );
        
        //Ciclo que controla los renglones
        for(i=0;i<r;i++)
        {
        	//Ciclo que controla las columnas
        	for(j=0;j<c;j++)
        	{
        		
        		//Se realiza la validicación para saber si estamos en un pibote y si este en cero
        		if(i==j&&(*(ap_matriz+(j+(c*i))))==0)
        		{
        			//Se realiza la validación para saber si la última columna del renglón de ese pibote es cero
        			if(*(ap_matriz+(c-1+(c*i)))==0)
        			{
        				printf("\nEl sistema tiene infinitas soluciones\n");
        				return 0;
        			}
        			else
        			{
        				printf("\nEl sistema no tiene solucion\n");
        				return 0;
        			}
        		}
        	}
        }
        
        //Ciclo que controla el número de raíces 

        for ( i = 0;i < r;i++ )
        {
            printf( "\nEl valor de x%d es :%Lf\n", i + 1, ((*(ap_matriz+(r+(c*i))))/(*(ap_matriz+(i+(c*i))))));
            //Se realiza la división de la última columna con el pibote
        }
        return 1;

}
short int gauss_jordan(long double *ap_matriz,int r,int c)
/*Realiza el algoritmo de Gauss-Jordan de manera secuencial 
Recibe: apuntador a la matriz, el número de renglones y de columnas
Envía:short int para ya no despelgar las raices en caso de que el sistema de ecuaciones no tenga solucion*/
{
	int k, i,j;//O(1)
	long double pib,cons;//O(1)

         //Ciclo que asigna el pibote 
        for ( i = 0;i < r;i++ )//O(r)
        {
            //Se divide al renglón entre el pibote para que este sea 1
            pib=*(ap_matriz+(i+(c*i)));
            for( j = 0;j < c;j++ )
            {
            	*(ap_matriz+(j+(c*i)))=*(ap_matriz+(j+(c*i))) / pib;//O(r*c)
            }
            
           //Ciclo que controla los renglones
            for ( j = 0;j < r;j++ )//O(r2)
            {
                cons=*(ap_matriz+(i+(c*j)));//Se establece la constante por la cual se va a multiplicar el pibote 
              
                
                //Ciclo que controla las columnas
                for ( k= 0;k < c;k++ )//O(r2*c)
                {
                    if ( j != i )//En caso de ser el renglón que contiene el pibote no se realiza la operación
                    {
                   	//Se realiza la resta del reglón actual con el renglón del pibote 
                        *(ap_matriz+(k+(c*j)))=((*(ap_matriz+(i+(c*i))))*(*(ap_matriz+(k+(c*j)))))-(cons*(*(ap_matriz+(k+(c*i)))));//O(r2*c-1)
                        
                   
                    }
                   
                }
            }
             //imprime_matriz(ap_matriz,r,c);
        }
        
        //O(r)+O(r*c)+O(r2)+O(r2*c)+O(r2*c-1)
        
       
	//Impresión de las raíces 
        printf("\nSolucion Secuencial:" );
        
        
        //Ciclo que controla los renglones
        for(i=0;i<r;i++)//O(r)
        {
        	//Ciclo que controla las columnas
        	for(j=0;j<c;j++)//O(r*c)
        	{
        		//Se realiza la validicación para saber si estamos en un pibote y si este en cero
        		if(i==j&&(*(ap_matriz+(j+(c*i))))==0)
        		{
        			//Se realiza la validación para saber si la última columna del renglón de ese pibote es cero
        			if(*(ap_matriz+(c-1+(c*i)))==0)
        			{
        				printf("\nEl sistema tiene infinitas soluciones\n");
        				return 0;
        			}
        			else
        			{
        				printf("\nEl sistema no tiene solucion\n");
        				return 0;
        			}
        		}
        	}
        }
       
        //Ciclo que controla el número de raíces 
        for ( i = 0;i < r;i++ )//O(r)
        {
            printf( "\nEl valor de x%d es :%Lf\n", i + 1, ((*(ap_matriz+(r+(c*i))))/(*(ap_matriz+(i+(c*i))))));
            //Se realiza la división de la última columna con el pibote
        }
        
        
        
        //O(r)+O(r*c)+O(r2)+O(r2*c)+O(r2*c-1)
        //O(r*c)+O(r)
        //O(r2*c)
        //O(r2*(r+1))
        //O(r3)
       
        return 1;
}
void llena_matriz(long double*ap_matriz,int r, int c,FILE *ap)
/*Realiza el llenado de la matriz
Recibe: apuntador a la matriz, el número de renglones y de columnas y el apuntador al archivo de datos de entrada
Envía:void*/
{
	int auxc,auxr,x,y;
	ap=fopen("Datos.txt","r");
	fscanf(ap,"%d",&x);
	fscanf(ap,"%d",&y);
	
	//Ciclo que controla los renglones
	for(auxr=0;auxr<r;auxr++)
	{
	//Ciclo que controla las columnas
	for(auxc=0;auxc<c;auxc++)
	{
	//Se realiza la inserción
  	fscanf(ap,"%Lf",(ap_matriz+(auxc+(c*auxr))));
 
	}
	}
fclose(ap);
}
void imprime_matriz(long double *ap_matriz,int r, int c)
{
/*Realiza la impresión de la matriz
Recibe: apuntador a la matriz, el número de renglones y de columnas
Envía:void*/
int aux,i;
aux=r*c;

//Ciclo que recorre la matriz
for(i=0;i<aux;i++)
{
if(i%c==0)//Valida si ya se termino el renglón para realizar un salto
{
printf("\n");
}
printf("|%.2Lf|",*(ap_matriz+i));//Impresión de la información
}
printf("\n");
}
void libera_matriz(long double **ap_matriz)
{
/*Realiza la liberación del apuntador a la matriz
Recibe: double apuntador a la matriz
Envía:void*/
   
   //Libera al apuntador y hace a apunte a nulo
    free(*ap_matriz);
    *ap_matriz=NULL;

}


