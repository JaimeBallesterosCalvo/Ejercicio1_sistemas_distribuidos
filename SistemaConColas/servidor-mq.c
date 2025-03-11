#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"
#include <errno.h>

#define MAX 1024

// Definición de estructuras dentro del servidor
struct peticion {
    int op;               // Código de operación (0: Destroy, 1: Set, etc.)
    int key;              // Clave principal
    char value1[MAX];     // Nombre o string asociado a la clave
    int N_value2;         // Número de valores en V_value2
    double V_value2[MAX]; // Array de valores
    struct Coord coord;   // Coordenada asociada
    char q_name[MAX];     // Nombre de la cola de respuesta del cliente
};


struct respuesta {
    int value;
    char status;
};

void tratar_peticion(struct peticion *p);

int main( int argc, char *argv[])
{
    struct peticion p;
    unsigned int prio;

    printf("SERVIDOR: Intentando abrir la cola /SERVIDOR...\n");
    int qs = mq_open("/SERVIDOR", O_CREAT | O_RDONLY, 0700, NULL);
    if (qs == -1) {
        perror("Error al abrir la cola del servidor");
        return -1;
    }

    while (1){
        printf("SERVIDOR: Esperando petición...\n");
        if (mq_receive(qs, (char *)&p, sizeof(p), &prio)){
            perror("Error al recibir la petición");
            continue;
        }
        tratar_peticion(&p);
    }
}

void tratar_peticion ( struct peticion *p )
{
    struct respuesta r ;

    switch (p->op) {
        case 0:  // Destroy
            r.status = destroy();
            if (r.status == -1) {
                perror("Error en destroy()");
            }
            break;
        case 1:  // Set
            r.status = set_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en set_value()\n");
            }
            break;
        case 2:  // Get
            r.status = get_value(p->key, p->value1, &(p->N_value2), p->V_value2, &(p->coord));
            if (r.status == -1) {
                fprintf(stderr, "Error en get()\n");
            }
            break;
        case 3:  // Modify
            r.status = modify_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en modify()\n");
            }
            break;
        case 4:  // Delete
            r.status = delete_key(p->key);
            if (r.status == -1) {
                fprintf(stderr, "Error en delete()\n");
            }
            break;
        case 5:  // Exist
            r.status = exist(p->key);
            if (r.status == -1) {
                fprintf(stderr, "Error en exist()\n");
            }
            break;
        default:
            printf("Operación no reconocida: %d\n", p->op);
            r.status = -1;
            break;
    }
    int qr = mq_open(p->q_name, O_WRONLY);
    if (qr == -1) {
        perror("Error al abrir la cola de respuesta");
        return;
    }

    if (mq_send(qr, (char*)&r, sizeof(struct respuesta), 0) == -1) {
        perror("Error en mq_send");
    }mq_close(qr);
}

