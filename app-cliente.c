#include <stdio.h>
#include "claves.h"
int main (int argc, char **argv)
{
    //estos son los valores que se introducirán como parámetros en el set
    int key = 5;
    char *v1 = "ejemplo de valor 1";
    double v2[] = {2.3, 0.5, 23.45};
    struct Coord v3;
    v3.x = 10;
    v3.y = 5;

    //los siguientes son los parámetros necesarios para obtener get
    char value1[1024];
    int N_value2;
    double V_value2[10];
    struct Coord value3;

    printf("CLIENTE: Inicializando sistema...\n");

    if (destroy() == -1) {
        printf("ERROR: No se pudo inicializar el sistema correctamente.\n");
        return -1; // Salimos con error
    }
    printf("CLIENTE: Sistema inicializado correctamente.\n");

    int err = set_value(key, v1, 3, v2, v3);
    if (err == -1) {
        printf("Error al insertar la tupla\n");
        return -1;
    }

    err = get_value(key, value1, &N_value2, V_value2, &value3);
    if (err == -1) {
        printf("Error al obtener la clave\n");
        return -1;
    }

    printf("CLIENTE: Clave %d obtenida correctamente\n", key);
    printf("Value1: %s\n", value1);
    printf("N_value2: %d\n", N_value2);
    for (int i = 0; i < N_value2; i++) {
        printf("V_value2[%d]: %.2f\n", i, V_value2[i]);
    }
    printf("Coordenadas: x = %d, y = %d\n", value3.x, value3.y);


     /* Prueba de modify_value */
    char *nuevo_v1 = "nuevo valor modificado";
    double nuevo_v2[] = {9.99, 8.88}; 
    struct Coord nuevo_v3;
    nuevo_v3.x = 20;
    nuevo_v3.y = 15;

    err = modify_value(key, nuevo_v1, 2, nuevo_v2, nuevo_v3);
    if (err == -1) {
        printf("Error al modificar la tupla\n");
        return -1;
    }
    printf("CLIENTE: después de modify_value");

    return 0;
}

