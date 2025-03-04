#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


int destroy(void)
{
   system("rm -fr ./KV");

    int ret = mkdir("KV", 0700);
    if (ret <0){
        perror("mkdir");
        return -1;
    }

    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3)
{
    char fname[1024];
    sprintf(fname, "./KV/%d", key); // Nombre del archivo = clave

    FILE *fd = fopen(fname, "w+");
    if (NULL == fd){
        printf("ERROR: fopen de %s\n", fname);
        return -1;
    }

    // Escribe los valores en el archivo
    fprintf(fd, "%s\n", value1);
    fprintf(fd, "%d\n", N_value2);
    for (int i = 0; i < N_value2; i++){
        fprintf(fd, "%le\n", V_value2[i]);
    }
    fprintf(fd, "%d\n", value3.x);
    fprintf(fd, "%d\n", value3.y);

    fclose(fd);
    return 0;
}


int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3)
{
    char fname[1024];
    sprintf(fname, "./KV/%d", key);

    FILE *fd = fopen(fname, "r"); // Abre el archivo en modo lectura
    if (NULL == fd){
        printf("ERROR: fopen de %s\n", fname);
        return -1;
    }

    fscanf(fd, "%1023[^\n]\n", value1); // Lee la cadena hasta el salto de línea
    fscanf(fd, "%d\n", N_value2); // Lee el número de elementos del vector
    for (int i = 0; i < *N_value2; i++){
        fscanf(fd, "%le\n", &(V_value2[i]));
    }
    fscanf(fd, "%d\n", &(value3->x));
    fscanf(fd, "%d\n", &(value3->y));

    fclose(fd);
    return 0;
}


int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char fname[1024];
    FILE *fd;

    /*Rango de N_value 2*/
    if (N_value2 < 1 || N_value2 > 32) {
        return -1;  /*Error, N_value 2 está fuera de rango*/
    }

    sprintf(fname, "./KV/%d", key);

    fd = fopen(fname, "r+"); 
    if (fd == NULL) {
        return -1; /*Error, no existe un elemento con dicha clave*/
    }

    fprintf(fd, "%s\n", value1);  
    fprintf(fd, "%d\n", N_value2);  
    
    for (int i = 0; i < N_value2; i++) {
        fprintf(fd, "%le\n", V_value2[i]);  
    }

    fprintf(fd, "%d\n", value3.x);  
    fprintf(fd, "%d\n", value3.y);  

    
    fclose(fd);

    return 0;  

}

int delete_key(int key) {
    char fname[1024];

    // Construir el nombre del archivo correspondiente a la clave
    sprintf(fname, "./KV/%d", key);

    // Intentar eliminar el archivo
    if (remove(fname) == 0) {
        // Si se eliminó correctamente, devolver 0
        return 0;
    } else {
        // Si no se pudo eliminar (por ejemplo, si el archivo no existe), devolver -1
        return -1;
    }
}

int exist(int key) {
    char fname[1024];
    FILE *fd;

    // Construir el nombre del archivo correspondiente a la clave
    sprintf(fname, "./KV/%d", key);

    // Intentar abrir el archivo en modo lectura ("r")
    fd = fopen(fname, "r");
    if (fd == NULL) {
        // Si el archivo no existe, devolver 0 (la clave no existe)
        return 0;
    }

    fclose(fd);
    return 1; 
}

/*hola*/
