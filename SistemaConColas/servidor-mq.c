#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"

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

    int qs = mq_open("/SERVIDOR", O_CREAT | O_RDONLY, 0700, NULL);
    if (qs == -1){
        return -1;
    }
    while (1){
        mq_receive(qs, &p, sizeof(p), &prio);
        tratar_peticion(&p);
    }
}

void tratar_peticion ( struct peticion *p )
{
    struct respuesta r ;

    switch (p->op) {
        case 0:  // Destroy
            r.status = destroy();
            break;
        case 1:  // Set
            r.status = set_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            break;
        case 2:  // Get
            r.status = get_value(p->key, p->value1, &(p->N_value2), p->V_value2, &(p->coord));
            break;
        case 3:  // Modify
            r.status = modify_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            break;
        case 4:  // Delete
            r.status = delete_key(p->key);
            break;
        case 5:  // Exist
            r.status = exist(p->key);
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

