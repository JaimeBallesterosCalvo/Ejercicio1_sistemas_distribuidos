#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"
#include <stdlib.h>
#include <unistd.h>


int destroy(void);
{
    /*todo esto es para poder ver si está bien lo que estamos haciendo*/
    /*se puede de hacer de otras maneras, pero con ficheros y un cat siempre funciona*/
    system("rm -fr ./KV");

    int ret = mkdir("KV", 0700);
    if (ret <0){
        perror("mkdir");
        return -1;
    }

    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
{
    char fname[1024];

    sprintf(fname, "./KV/%d", key);

    fd =fopen(fname, "w+");
    if (NULL ==fd){ /**hay que poner toda la comprobación de errores que se puedan**/
        printf("ERROR: fopen de %\n", fname)
    }

    fprintf(fd, "%s\n", value1);
    fprintf(fd, "%d\n", N_value2);
    for (int i=0; i<N_value2; i++)
        fprintf(fd, "%le\n", V_value2[i]);
    }
    fprintf(fd, "%d\n", value3.x);
    fprintf(fd, "%d\n", value3.y);

    fclose(fd);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);
{
    /* en este caso, es parecido al de set value, pero esta vez cogera datos*/
    /* es por ello que hace scanf en vez de printf*/

    char fname[1024];
    FILE *fd;

    sprintf(fname, "./KV/%d", key);

    fd =fopen(fname, "r+");
    if (NULL ==fd){ /**hay que poner toda la comprobación de errores que se puedan**/
        printf("ERROR: fopen de %\n", fname)
    }

    fscan(fd, "%s\n", value1);
    fscan(fd, "%1023[^\n]\n", value1); /*esto es para leer varias palabras, significa que lee hasta un enter*/
    fscan(fd, "%d\n", *N_value2);
    for (int i=0; i<*N_value2; i++){
        fscan(fd, "%le\n", &(V_value2[i]));
    }
    fscan(fd, "%d\n", &(value3->x) ); /*quiero acceder a este puntero al campo X*/
    fscan(fd, "%d\n", &((*value3).y) ); /*es lo mismo pero en otra notación*/

    fclose(fd);
    }

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char fname[1024];
    FILE *fd;

    /*Rango de N_value 2*/
    if (N_value2 < 1 || N_value2 > 32) {
        return -1;  /*Error, N_value 2 está fuera de rango*/
    }

    sprintf(fname, "./KV/%d", key);

    fd = fopen(fname, "w"); 
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

    // Si el archivo se abrió correctamente, la clave existe
    fclose(fd); // Cerrar el archivo después de verificar
    return 1; // La clave existe
}

