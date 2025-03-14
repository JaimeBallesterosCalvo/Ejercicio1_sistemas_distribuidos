#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"
#include <errno.h>

#define MAX 256

// Definición de estructuras
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
    int status;           // Estado de la operación
    char value1[MAX];     // Valor 1
    int N_value2;         // Número de valores en V_value2
    double V_value2[MAX]; // Array de valores 2
    struct Coord coord;   // Coordenada
};

// Función auxiliar para abrir una cola de mensajes
mqd_t abrir_cola(const char *nombre, int flags, struct mq_attr *attr) {
    mqd_t cola = mq_open(nombre, flags, 0700, attr);
    if (cola == -1) {
        printf("Error al abrir la cola %s: %s\n", nombre, strerror(errno));
    }
    return cola;
}

// Función auxiliar para enviar una petición
int enviar_peticion(mqd_t cola, struct peticion *p) {
    if (mq_send(cola, (char *)p, sizeof(struct peticion), 0) == -1) {
        printf("Error al enviar petición: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

// Función auxiliar para recibir una respuesta
int recibir_respuesta(mqd_t cola, struct respuesta *r) {
    unsigned int prio = 0;
    if (mq_receive(cola, (char *)r, sizeof(struct respuesta), &prio) == -1) {
        printf("Error al recibir la respuesta: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

// Función auxiliar para manejar la lógica común de las operaciones
int operacion_comun(int op, int key, char *value1, int *N_value2, double *V_value2, struct Coord *coord, int *ret_value) {
    struct peticion p;
    struct respuesta r;
    char qr_name[MAX];
    struct mq_attr attr = {0, 10, sizeof(struct peticion), 0};

    // Generar nombre único para la cola de respuesta
    sprintf(qr_name, "/CLIENTE_%d", getpid());

    // Abrir cola del servidor
    mqd_t qs = abrir_cola("/SERVIDOR", O_CREAT | O_WRONLY, &attr);
    if (qs == -1) return -1;

    // Abrir cola de respuesta
    attr.mq_msgsize = sizeof(struct respuesta);
    mqd_t qr = abrir_cola(qr_name, O_CREAT | O_RDONLY, &attr);
    if (qr == -1) {
        mq_close(qs);
        return -1;
    }

    // Preparar petición
    p.op = op;
    p.key = key;
    strncpy(p.value1, value1, MAX);
    p.N_value2 = *N_value2;
    memcpy(p.V_value2, V_value2, sizeof(double) * (*N_value2));
    p.coord = *coord;
    strncpy(p.q_name, qr_name, MAX);

    // Enviar petición
    if (enviar_peticion(qs, &p) == -1) {
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    // Recibir respuesta
    if (recibir_respuesta(qr, &r) == -1) {
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    // Manejar la respuesta
    *ret_value = r.status;
    if (op == 2) {  // Operación GET
        strncpy(value1, r.value1, MAX);
        *N_value2 = r.N_value2;
        if (V_value2 != NULL) {
            memcpy(V_value2, r.V_value2, sizeof(double) * r.N_value2);
        }
        *coord = r.coord;
    }

    // Cerrar y eliminar colas
    mq_close(qs);
    mq_close(qr);
    mq_unlink(qr_name);

    return r.status;
}

// Función auxiliar para operaciones simples
// lo que hace es dar valores a N_value, V_value2 y las coordenadas, porque si hacemos todas las operaciones juntas
// nos daba error, asi que esta es la solución que hemos ideado
int operacion_simple(int op, int key) {
    char value1[MAX] = "";
    int N_value2 = 0;
    double V_value2[MAX] = {0};
    struct Coord coord = {0, 0};
    int ret_value;

    return operacion_comun(op, key, value1, &N_value2, V_value2, &coord, &ret_value);
}

// Funciones de la API
int destroy() {
    return operacion_simple(0, 0);
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return operacion_comun(1, key, value1, &N_value2, V_value2, &coord, &ret_value);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *coord) {
    int ret_value;
    return operacion_comun(2, key, value1, N_value2, V_value2, coord, &ret_value);
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return operacion_comun(3, key, value1, &N_value2, V_value2, &coord, &ret_value);
}

int delete_key(int key) {
    return operacion_simple(4, key);
}

int exist(int key) {
    return operacion_simple(5, key);
}