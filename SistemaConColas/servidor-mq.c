#include <stdio.h>
#include <mqueue.h>
#include "claves.h"
#include <errno.h>

#define MAX 256

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


struct respuesta { //TODO tengo que poner las cosas de manera más específica o como?
    int value;
    char status;
};

void tratar_peticion(struct peticion *p);

int main( int argc, char *argv[])
{
    printf("SERVIDOR: Tamaño de peticion: %ld\n", sizeof(struct peticion));
    struct peticion p;
    unsigned int prio;

    struct mq_attr attr;
    // Configurar atributos de la cola del cliente
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(p);
    attr.mq_curmsgs = 0;

    printf("SERVIDOR: Intentando abrir la cola /SERVIDOR...\n");
    int qs = mq_open("/SERVIDOR", O_CREAT | O_RDONLY, 0700, &attr);
    if (qs == -1) {
        perror("Error al abrir la cola del servidor");
        return -1;
    }

    while(1){
        printf("SERVIDOR: Esperando petición...\n");
        mq_receive(qs, (char *)&p, sizeof(p), &prio);

        printf("SERVIDOR: Petición recibida\n");
        printf("SERVIDOR: op = %d\n", p.op);
        printf("SERVIDOR: key = %d\n", p.key);
        printf("SERVIDOR: value1 = %s\n", p.value1);
        printf("SERVIDOR: N_value2 = %d\n", p.N_value2);
        printf("SERVIDOR: q_name = %s\n", p.q_name);

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

    printf("SERVIDOR: Nombre de la cola de respuesta: %s\n", p->q_name);
    struct mq_attr attr;
    // Configurar atributos de la cola del cliente
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct respuesta);
    attr.mq_curmsgs = 0;

    int qr = mq_open(p->q_name, O_WRONLY, &attr);
    if (qr == -1) {
        perror("Error al abrir la cola de respuesta");
        return;
    }

    if (mq_send(qr, (char*)&r, sizeof(struct respuesta), 0) == -1) {
        perror("Error en mq_send");
    }mq_close(qr);
}

