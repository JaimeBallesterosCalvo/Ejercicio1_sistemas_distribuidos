#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"
#include <errno.h>

#define MAX 1024

struct peticion {
    int op;               // Código de operación
    int key;              // Clave
    char value1[MAX];     // Valor 1
    int N_value2;         // Número de valores en V_value2
    double V_value2[MAX]; // Array de valores 2
    struct Coord coord;   // Coordenada
    char q_name[MAX];     // Nombre de la cola de respuesta
};

struct respuesta {
    int value;            // Valor de retorno
    int status;           // Estado de la operación
};

int proxy_operacion(int op, int key, char *value1, int N_value2, double *V_value2, struct Coord coord, int *ret_value) {
    struct peticion p;
    struct respuesta r;
    char qr_name[MAX];
    unsigned int prio = 0;

    // Generar nombre único para la cola de respuesta
    sprintf(qr_name, "/CLIENTE_%d", getpid());

    // Abrir colas de mensajes
    int qs = mq_open("/SERVIDOR", O_CREAT | O_WRONLY, 0700, NULL);
    if (qs == -1) {
        printf("Error al abrir la cola del servidor: %s\n", strerror(errno)); // Imprimir mensaje de error
        return -1;
    }
    printf("PROXY: /SERVIDOR abierto correctamente\n");

    int qr = mq_open(qr_name, O_CREAT | O_RDONLY, 0700, NULL);
    if (qr == -1) {
        printf("Error al abrir la cola de respuesta: %s\n", strerror(errno)); // Imprimir mensaje de error
        mq_close(qs);
        return -1;
    }


    // Preparar petición
    p.op = op;
    p.key = key;
    strcpy(p.value1, value1);
    p.N_value2 = N_value2;
    memcpy(p.V_value2, V_value2, sizeof(double) * N_value2);
    p.coord = coord;
    strcpy(p.q_name, qr_name);

    // Enviar petición
    if (mq_send(qs, (char *)&p, sizeof(struct peticion), 0) == -1) {
        printf("PROXY: Error al enviar petición: %s\n", strerror(errno));
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1; // Error al enviar la petición
    }
    printf("PROXY: Petición enviada correctamente\n");

    // Recibir respuesta
    if (mq_receive(qr, (char *)&r, sizeof(struct respuesta), &prio) == -1) {
        printf("Error al recibir la respuesta: %s\n", strerror(errno)); // Imprimir mensaje de error
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    // Cerrar y eliminar colas
    mq_close(qs);
    mq_close(qr);
    mq_unlink(qr_name);

    // Devolver resultados
    *ret_value = r.value;
    return r.status;
}

// Funciones proxy específicas
int destroy() {
    printf("PROXY: destroy() llamada\n");
    int ret_value;
    int result = proxy_operacion(0, 0, "", 0, NULL, (struct Coord){0, 0}, &ret_value);
    printf("PROXY: destroy() retorna %d\n", result);
    return result;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return proxy_operacion(1, key, value1, N_value2, V_value2, coord, &ret_value);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *coord) {
    int ret_value;
    int status = proxy_operacion(2, key, value1, *N_value2, V_value2, *coord, &ret_value);
    *N_value2 = ret_value; // Actualizar N_value2 con el valor de retorno
    return status;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return proxy_operacion(3, key, value1, N_value2, V_value2, coord, &ret_value);
}

int delete_key(int key) {
    int ret_value;
    return proxy_operacion(4, key, "", 0, NULL, (struct Coord){0, 0}, &ret_value);
}

int exist(int key) {
    int ret_value;
    return proxy_operacion(5, key, "", 0, NULL, (struct Coord){0, 0}, &ret_value);
}