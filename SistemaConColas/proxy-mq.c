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

// Función principal del proxy
int proxy_operacion(int op, int key, char *value1, int *N_value2, double *V_value2, struct Coord *coord, int *ret_value) {
    printf("PROXY: Preparando petición para operación %d (key = %d)\n", op, key);
    struct peticion p;
    struct respuesta r;
    char qr_name[MAX];
    unsigned int prio = 0;

    // Generar nombre único para la cola de respuesta
    printf("PROXY: Generando nombre para la cola de respuesta...\n");
    sprintf(qr_name, "/CLIENTE_%d", getpid());
    printf("PROXY: Nombre de la cola de respuesta: %s\n", qr_name);

    struct mq_attr attr;

    // Configurar atributos para la cola del servidor
    printf("PROXY: Configurando atributos para la cola del servidor...\n");
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct peticion);
    attr.mq_curmsgs = 0;

    // Abrir cola del servidor
    printf("PROXY: Abriendo cola del servidor...\n");
    int qs = mq_open("/SERVIDOR", O_CREAT | O_WRONLY, 0700, &attr);
    if (qs == -1) {
        printf("Error al abrir la cola del servidor: %s\n", strerror(errno));
        return -1;
    }
    printf("PROXY: Cola del servidor abierta correctamente.\n");

    // Reinicializar attr para la cola de respuesta
    printf("PROXY: Configurando atributos para la cola de respuesta...\n");
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct respuesta);
    attr.mq_curmsgs = 0;

    // Abrir cola de respuesta
    printf("PROXY: Abriendo cola de respuesta...\n");
    int qr = mq_open(qr_name, O_CREAT | O_RDONLY, 0700, &attr);
    if (qr == -1) {
        printf("Error al abrir la cola de respuesta: %s\n", strerror(errno));
        mq_close(qs);
        return -1;
    }
    printf("PROXY: Cola de respuesta abierta correctamente.\n");

    // Preparar petición
    printf("PROXY: Preparando petición...\n");
    p.op = op;
    p.key = key;
    printf("PROXY: Copiando value1...\n");
    strcpy(p.value1, value1);
    printf("PROXY: Copiando N_value2...\n");
    p.N_value2 = *N_value2;
    printf("PROXY: Copiando V_value2...\n");
    memcpy(p.V_value2, V_value2, sizeof(double) * (*N_value2));
    printf("PROXY: Copiando coord...\n");
    p.coord = *coord;
    printf("PROXY: Copiando q_name...\n");
    strcpy(p.q_name, qr_name);

    printf("PROXY: Enviando petición para operación %d (key = %d)\n", op, key);
    // Enviar petición
    if (mq_send(qs, (char *)&p, sizeof(p), 0) == -1) {
        printf("Error al enviar petición: %s\n", strerror(errno));
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    // Recibir respuesta
    if (mq_receive(qr, (char *)&r, sizeof(struct respuesta), &prio) == -1) {
        printf("Error al recibir la respuesta: %s\n", strerror(errno));
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    // Manejar la respuesta según la operación
    if (r.status == -1) {
        printf("CLIENTE: Error en la operación\n");
    } else {
        printf("CLIENTE: Operación exitosa\n");
        printf("CLIENTE: status = %d\n", r.status);
        printf("CLIENTE: value1 = %s\n", r.value1);
        printf("CLIENTE: N_value2 = %d\n", r.N_value2);

        // Mostrar los valores de V_value2
        if (r.N_value2 > 0) {
            printf("CLIENTE: Valores de V_value2:\n");
            for (int i = 0; i < r.N_value2; i++) {
                printf("CLIENTE: V_value2[%d] = %f\n", i, r.V_value2[i]);
            }
        } else {
            printf("CLIENTE: No hay valores en V_value2\n");
        }

        printf("CLIENTE: coord = (%d, %d)\n", r.coord.x, r.coord.y);
    }

    // Manejar la respuesta según la operación
    *ret_value = r.status;
    if (op == 2) {  // Operación GET
        strcpy(value1, r.value1);
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

// Función auxiliar para operaciones sin datos adicionales
int proxy_operacion_simple(int op, int key) {
    printf("PROXY: Preparando petición simple para operación %d (key = %d)\n", op, key);
    struct peticion p;
    struct respuesta r;
    char qr_name[MAX];
    unsigned int prio = 0;

    // Generar nombre único para la cola de respuesta
    sprintf(qr_name, "/CLIENTE_%d", getpid());

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct peticion);
    attr.mq_curmsgs = 0;

    int qs = mq_open("/SERVIDOR", O_CREAT | O_WRONLY, 0700, &attr);
    if (qs == -1) {
        printf("Error al abrir la cola del servidor: %s\n", strerror(errno));
        return -1;
    }

    attr.mq_msgsize = sizeof(struct respuesta);
    int qr = mq_open(qr_name, O_CREAT | O_RDONLY, 0700, &attr);
    if (qr == -1) {
        printf("Error al abrir la cola de respuesta: %s\n", strerror(errno));
        mq_close(qs);
        return -1;
    }

    p.op = op;
    p.key = key;
    strcpy(p.value1, "");
    p.N_value2 = 0;
    p.coord.x = 0;
    p.coord.y = 0;
    strcpy(p.q_name, qr_name);

    if (mq_send(qs, (char *)&p, sizeof(p), 0) == -1) {
        printf("Error al enviar petición: %s\n", strerror(errno));
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    if (mq_receive(qr, (char *)&r, sizeof(struct respuesta), &prio) == -1) {
        printf("Error al recibir la respuesta: %s\n", strerror(errno));
        mq_close(qs);
        mq_close(qr);
        mq_unlink(qr_name);
        return -1;
    }

    mq_close(qs);
    mq_close(qr);
    mq_unlink(qr_name);

    return r.status;
}


int destroy() {
    printf("PROXY: destroy() llamada\n");
    return proxy_operacion_simple(0, 0);
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return proxy_operacion(1, key, value1, &N_value2, V_value2, &coord, &ret_value);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *coord) {
    int ret_value;
    int status = proxy_operacion(2, key, value1, N_value2, V_value2, coord, &ret_value);
    return status;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord coord) {
    int ret_value;
    return proxy_operacion(3, key, value1, &N_value2, V_value2, &coord, &ret_value);
}

int delete_key(int key) {
    printf("PROXY: delete_key() llamada\n");
    return proxy_operacion_simple(4, key);
}

int exist(int key) {
    printf("PROXY: exist() llamada\n");
    return proxy_operacion_simple(5, key);
}