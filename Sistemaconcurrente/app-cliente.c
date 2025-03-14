#include <stdio.h>
#include "claves.h"

int main(int argc, char **argv) {
    printf("CLIENTE: Iniciando pruebas...\n\n");

    // Prueba 1: Insertar múltiples claves y verificar su existencia
    printf("=== Prueba 1: Insertar múltiples claves y verificar su existencia ===\n");
    int key1 = 1;
    char *v1_1 = "valor1 para clave1";
    double v2_1[] = {1.1, 2.2};
    struct Coord v3_1 = {1, 1};

    int key2 = 2;
    char *v1_2 = "valor1 para clave2";
    double v2_2[] = {3.3, 4.4};
    struct Coord v3_2 = {2, 2};

    // Insertar primera clave
    int err = set_value(key1, v1_1, 2, v2_1, v3_1);
    if (err == -1) {
        printf("Error al insertar la clave1\n");
    } else {
        printf("Clave1 insertada correctamente.\n");
    }

    // Insertar segunda clave
    err = set_value(key2, v1_2, 2, v2_2, v3_2);
    if (err == -1) {
        printf("Error al insertar la clave2\n");
    } else {
        printf("Clave2 insertada correctamente.\n");
    }

    // Verificar existencia de las claves
    if (exist(key1) == 1) {
        printf("Clave1 existe.\n");
    } else {
        printf("Clave1 no existe.\n");
    }

    if (exist(key2) == 1) {
        printf("Clave2 existe.\n");
    } else {
        printf("Clave2 no existe.\n");
    }
    printf("\n");

    // Prueba 2: Modificar una clave y verificar los cambios
    printf("=== Prueba 2: Modificar una clave y verificar los cambios ===\n");
    int key3 = 3;
    char *v1_3 = "valor original";
    double v2_3[] = {5.5, 6.6};
    struct Coord v3_3 = {3, 3};

    // Insertar clave
    err = set_value(key3, v1_3, 2, v2_3, v3_3);
    if (err == -1) {
        printf("Error al insertar la clave3\n");
    } else {
        printf("Clave3 insertada correctamente.\n");
    }

    // Modificar clave
    char *nuevo_v1 = "valor modificado";
    double nuevo_v2[] = {7.7, 8.8};
    struct Coord nuevo_v3 = {4, 4};

    err = modify_value(key3, nuevo_v1, 2, nuevo_v2, nuevo_v3);
    if (err == -1) {
        printf("Error al modificar la clave3\n");
    } else {
        printf("Clave3 modificada correctamente.\n");
    }

    // Obtener y mostrar los valores modificados
    char value1[1024] = {0};
    int N_value2 = 0;
    double V_value2[10] = {0};
    struct Coord value3 = {0, 0};

    err = get_value(key3, value1, &N_value2, V_value2, &value3);
    if (err == -1) {
        printf("Error al obtener la clave3\n");
    } else {
        printf("Value1: %s\n", value1);
        printf("N_value2: %d\n", N_value2);
        for (int i = 0; i < N_value2; i++) {
            printf("V_value2[%d]: %.2f\n", i, V_value2[i]);
        }
        printf("Coordenadas: x = %d, y = %d\n", value3.x, value3.y);
    }
    printf("\n");

    // Prueba 3: Eliminar una clave y verificar su eliminación
    printf("=== Prueba 3: Eliminar una clave y verificar su eliminación ===\n");
    int key4 = 4;
    char *v1_4 = "valor para clave4";
    double v2_4[] = {9.9, 10.10};
    struct Coord v3_4 = {5, 5};

    // Insertar clave
    err = set_value(key4, v1_4, 2, v2_4, v3_4);
    if (err == -1) {
        printf("Error al insertar la clave4\n");
    } else {
        printf("Clave4 insertada correctamente.\n");
    }

    // Eliminar clave
    err = delete_key(key4);
    if (err == -1) {
        printf("Error al eliminar la clave4\n");
    } else {
        printf("Clave4 eliminada correctamente.\n");
    }

    // Verificar si la clave fue eliminada
    if (exist(key4) == 0) {
        printf("La clave4 fue eliminada correctamente.\n");
    } else {
        printf("La clave4 no fue eliminada.\n");
    }
    printf("\n");

    // Prueba 4: Intentar obtener una clave que no existe
    printf("=== Prueba 4: Intentar obtener una clave que no existe ===\n");
    int key5 = 999;  // Clave que no existe

    char value1_5[1024] = {0};
    int N_value2_5 = 0;
    double V_value2_5[10] = {0};
    struct Coord value3_5 = {0, 0};

    err = get_value(key5, value1_5, &N_value2_5, V_value2_5, &value3_5);
    if (err == -1) {
        printf("Error al obtener la clave5 (como se esperaba, la clave no existe).\n");
    } else {
        printf("La clave5 existe, lo cual no debería ocurrir.\n");
    }
    printf("\n");

    // Prueba 5: Intentar modificar una clave que no existe
    printf("=== Prueba 5: Intentar modificar una clave que no existe ===\n");
    int key6 = 999;  // Clave que no existe
    char *nuevo_v1_6 = "intento de modificación";
    double nuevo_v2_6[] = {11.11, 12.12};
    struct Coord nuevo_v3_6 = {6, 6};

    err = modify_value(key6, nuevo_v1_6, 2, nuevo_v2_6, nuevo_v3_6);
    if (err == -1) {
        printf("Error al modificar la clave6 (como se esperaba, la clave no existe).\n");
    } else {
        printf("La clave6 fue modificada, lo cual no debería ocurrir.\n");
    }
    printf("\n");

    // Prueba 6: Intentar eliminar una clave que no existe
    printf("=== Prueba 6: Intentar eliminar una clave que no existe ===\n");
    int key7 = 999;  // Clave que no existe

    err = delete_key(key7);
    if (err == -1) {
        printf("Error al eliminar la clave7 (como se esperaba, la clave no existe).\n");
    } else {
        printf("La clave7 fue eliminada, lo cual no debería ocurrir.\n");
    }
    printf("\n");

    printf("CLIENTE: Todas las pruebas completadas.\n");
    return 0;
}

