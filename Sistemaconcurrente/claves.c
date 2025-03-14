#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"
#include <sys/stat.h>

char *warehouse_name = "KV";
int initialized = 0;

int aux_init(void) {
    int ret;

    ret = mkdir("KV", 0700);
    if (ret < 0) {
        perror("mkdir: ");
        return -1;
    }

    return 0;
}

int aux_get_file_name(char *fname, int key) {
    sprintf(fname, "%s/%d", warehouse_name, key);
    return 0;
}

int destroy(void) {
    system("rm -fr ./KV");

    int ret = mkdir("KV", 0700);
    if (ret < 0) {
        perror("mkdir");
        return -1;
    }

    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char fname[1024];
    int ret;

    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    ret = exist(key);
    if (ret == 1){
        fprintf(stderr, "ERROR: la key %d ya existe\n", key);
        return -1;
    }

    if ((N_value2 <1) || (N_value2 >32)){
        fprintf(stderr, "ERROR: N_value2 %d está fuera de rango\n", N_value2);
        return -1;
    }


    sprintf(fname, "./KV/%d", key); // Nombre del archivo = clave
    aux_get_file_name(fname, key);
    FILE *fd = fopen(fname, "w+");
    if (NULL == fd) {
        printf("ERROR: fopen de %s\n", fname);
        return -1;
    }

    // Escribe los valores en el archivo
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

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    char fname[1024];

    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    //TODO hacer lo de los gets

    sprintf(fname, "./KV/%d", key);
    aux_get_file_name(fname, key);
    FILE *fd = fopen(fname, "r"); // Abre el archivo en modo lectura
    if (NULL == fd) {
        printf("ERROR: fopen de %s\n", fname);
        return -1;
    }

    fscanf(fd, "%1023[^\n]\n", value1); // Lee la cadena hasta el salto de línea
    fscanf(fd, "%d\n", N_value2); // Lee el número de elementos del vector
    for (int i = 0; i < *N_value2; i++) {
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
    int ret;

    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    ret = exist(key);
    if (ret == 0){
        fprintf(stderr, "ERROR: la key %d no existe\n", key);
        return -1;
    }

    if ((N_value2 <1) || (N_value2 >32)){
        fprintf(stderr, "ERROR: N_value2 %d está fuera de rango\n", N_value2);
        return -1;
    }



    sprintf(fname, "./KV/%d", key);
    aux_get_file_name(fname, key);
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

    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    sprintf(fname, "./KV/%d", key);
    aux_get_file_name(fname, key);

    printf("Intentando eliminar el archivo: %s\n", fname);  // Mensaje de depuración

    if (remove(fname) == 0) {
        printf("Archivo eliminado correctamente: %s\n", fname);  // Mensaje de depuración
        return 0;
    } else {
        perror("Error al eliminar el archivo");  // Mensaje de depuración
        return -1;
    }
}

int exist(int key) {
    char fname[1024];
    FILE *fd;

    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    sprintf(fname, "./KV/%d", key);
    aux_get_file_name(fname, key);

    printf("Intentando abrir el archivo: %s\n", fname);  // Mensaje de depuración

    // Intentar abrir el archivo en modo lectura ("r")
    fd = fopen(fname, "r");
    if (fd == NULL) {
        printf("El archivo no existe: %s\n", fname);  // Mensaje de depuración
        return 0;
    }

    fclose(fd);
    printf("El archivo existe: %s\n", fname);  // Mensaje de depuración
    return 1;
}


