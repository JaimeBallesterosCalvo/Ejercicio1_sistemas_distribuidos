#include <stdio.h>
#include "claves.h"
int main(int argc, char **argv) {
    // Valores para set_value
    int key = 5;
    char *v1 = "ejemplo de valor 1";
    double v2[] = {2.3, 0.5, 23.45};
    struct Coord v3 = {10, 5};  // Inicializar v3

    // Variables para get_value
    char value1[1024] = {0};  // Inicializar value1
    int N_value2 = 0;         // Inicializar N_value2
    double V_value2[10] = {0}; // Inicializar V_value2
    struct Coord value3 = {0, 0}; // Inicializar value3

    printf("CLIENTE: Inicializando sistema...\n");

    // Insertar una tupla con set_value
    int err = set_value(key, v1, 3, v2, v3);
    if (err == -1) {
        printf("Error al insertar la tupla\n");
        return -1;
    }

    // Obtener la tupla con get_value
    err = get_value(key, value1, &N_value2, V_value2, &value3);
    if (err == -1) {
        printf("Error al obtener la clave\n");
        return -1;
    }

    // Mostrar los valores obtenidos
    printf("CLIENTE: Clave %d obtenida correctamente\n", key);
    printf("Value1: %s\n", value1);
    printf("N_value2: %d\n", N_value2);

    // Verificar N_value2 antes de iterar sobre V_value2
    if (N_value2 > 0) {
        for (int i = 0; i < N_value2; i++) {
            printf("V_value2[%d]: %.2f\n", i, V_value2[i]);
        }
    } else {
        printf("No hay valores en V_value2\n");
    }

    printf("Coordenadas: x = %d, y = %d\n", value3.x, value3.y);

    // Prueba de modify_value
    char *nuevo_v1 = "nuevo valor modificado";
    double nuevo_v2[] = {9.99, 8.88};
    struct Coord nuevo_v3 = {20, 15};  // Inicializar nuevo_v3

    err = modify_value(key, nuevo_v1, 2, nuevo_v2, nuevo_v3);
    if (err == -1) {
        printf("Error al modificar la tupla\n");
        return -1;
    }
    printf("CLIENTE: después de modify_value\n");

    // Volver a obtener los datos después de la modificación
    err = get_value(key, value1, &N_value2, V_value2, &value3);
    if (err == -1) {
        printf("Error al obtener la clave después de la modificación\n");
        return -1;
    }

    // Mostrar los valores obtenidos después de la modificación
    printf("Value1: %s\n", value1);
    printf("N_value2: %d\n", N_value2);

    if (N_value2 > 0) {
        for (int i = 0; i < N_value2; i++) {
            printf("V_value2[%d]: %.2f\n", i, V_value2[i]);
        }
    } else {
        printf("No hay valores en V_value2\n");
    }

    printf("Coordenadas: x = %d, y = %d\n", value3.x, value3.y);

    // Prueba de delete_key
    printf("CLIENTE: Intentando eliminar la clave %d...\n", key);
    err = delete_key(key);
    if (err == -1) {
        printf("Error al eliminar la clave\n");
        return -1;
    }
    printf("CLIENTE: Clave %d eliminada correctamente.\n", key);


    // Verificar si la clave fue eliminada
    printf("CLIENTE: Verificando existencia de la clave %d...\n", key);
    err = exist(key);
    if (err == 0) {
        printf("La clave %d no existe.\n", key);
    } else {
        printf("La clave %d sigue existiendo.\n", key);
    }


    return 0;
}

