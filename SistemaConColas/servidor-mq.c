#include <stdio.h>
#include <stdlib.h>
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


struct respuesta {
    int status;           // Estado de la operación
    char value1[MAX];     // Valor 1
    int N_value2;         // Número de valores en V_value2
    double V_value2[MAX]; // Array de valores 2
    struct Coord coord;   // Coordenada
};

void tratar_peticion(struct peticion *p);

int main( int argc, char *argv[])
{
    struct peticion p;
    unsigned int prio;

    struct mq_attr attr;
    // Configurar atributos de la cola del cliente
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(p);
    attr.mq_curmsgs = 0;

    printf("SERVIDOR: Abro la cola del /SERVIDOR...\n");
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

        tratar_peticion(&p);
    }
}

void tratar_peticion ( struct peticion *p )
{
    struct respuesta r ;
    // Inicializar la respuesta
    r.status = -1;  // Valor por defecto en caso de error
    r.value1[0] = '\0';  // Inicializar value1
    r.N_value2 = 0;  // Inicializar N_value2
    r.coord = (struct Coord){0, 0};  // Inicializar coord
    printf("SERVIDOR: Procesando petición...\n");
    printf("SERVIDOR: Operación solicitada: %d\n", p->op);

    switch (p->op) {
        case 0:  // Destroy
            printf("SERVIDOR: Ejecutando operación DESTROY\n");
            r.status = destroy();
            if (r.status == -1) {
                perror("Error en destroy()");
            }
            break;
        case 1:  // Set
            printf("SERVIDOR: Ejecutando operación SET\n");
            printf("SERVIDOR: key = %d, value1 = %s, N_value2 = %d, coord = (%d, %d)\n",
                   p->key, p->value1, p->N_value2, p->coord.x, p->coord.y);
            r.status = set_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en set_value()\n");
            }
            break;
        case 2:  // Get
            printf("SERVIDOR: Ejecutando operación GET\n");
            printf("SERVIDOR: key = %d\n", p->key);
            r.status = get_value(p->key, r.value1, &r.N_value2, r.V_value2, &r.coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en get()\n");
            }
            break;
        case 3:  // Modify
            printf("SERVIDOR: Ejecutando operación MODIFY\n");
            printf("SERVIDOR: key = %d, value1 = %s, N_value2 = %d, coord = (%d, %d)\n",
                   p->key, p->value1, p->N_value2, p->coord.x, p->coord.y);
            r.status = modify_value(p->key, p->value1, p->N_value2, p->V_value2, p->coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en modify()\n");
            }
            break;
        case 4:  // Delete
            printf("SERVIDOR: Ejecutando operación DELETE\n");
            printf("SERVIDOR: key = %d\n", p->key);
            r.status = delete_key(p->key);
            if (r.status == -1) {
                fprintf(stderr, "Error en delete()\n");
            }
            break;
        case 5:  // Exist
            printf("SERVIDOR: Ejecutando operación EXIST\n");
            printf("SERVIDOR: key = %d\n", p->key);
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

    printf("SERVIDOR: Resultado de la operación: status = %d\n", r.status);
    printf("SERVIDOR: Enviando respuesta al cliente...\n");

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
    }
    else {
        printf("SERVIDOR: Respuesta enviada correctamente\n");
        printf("SERVIDOR: Contenido de la respuesta enviada:\n");
        printf("SERVIDOR: r.status = %d\n", r.status);
        printf("SERVIDOR: r.value1 = %s\n", r.value1);
        printf("SERVIDOR: r.N_value2 = %d\n", r.N_value2);
        for (int i = 0; i < r.N_value2; i++) {
            printf("SERVIDOR: r.V_value2[%d] = %f\n", i, r.V_value2[i]);
        }
        printf("SERVIDOR: r.coord = (%d, %d)\n", r.coord.x, r.coord.y);
    }

    mq_close(qr);
}

