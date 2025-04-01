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

    if (strlen(value1) >= 256) {
        fprintf(stderr, "Error: value1 excede 255 caracteres\n");
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
    // Verifica si el directorio KV existe, si no, créalo
    if (initialized == 0) {
        if (aux_init() == -1) {
            return -1;  // Error al crear el directorio
        }
        initialized = 1;  // Marca como inicializado
    }

    // Validaciones adicionales
    if (exist(key) != 1) {
        return -1;  // Clave no existe
    }
    if (strlen(value1) >= 256 || N_value2 < 1 || N_value2 > 32) {
        return -1;
    }

    // Primero eliminamos la clave existente
    if (delete_key(key) != 0) {
        return -1;  // Error al eliminar la clave
    }

    // Luego creamos una nueva con los mismos valores (modificados)
    return set_value(key, value1, N_value2, V_value2, value3);
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

    if (exist(key) != 1) {
        return -1;  // Clave no existe
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


