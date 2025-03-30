#include <stdio.h>
#include <mqueue.h>
#include "claves.h"
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "msg.h"

#define MAX 256
#define TRUE 1
#define FALSE 0

int qs;

// Mutex para proteger operaciones en claves.c
pthread_mutex_t claves_mutex = PTHREAD_MUTEX_INITIALIZER;

// Variables globales para sincronización
pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_cond = PTHREAD_COND_INITIALIZER;
int sync_copied = FALSE;  // Indica si la petición ha sido copiada


void limpiar_y_salir(int signum) {
    printf("\nSERVIDOR: Capturada señal SIGINT (Ctrl+C). Cerrando cola del servidor...\n");
    if (mq_close(qs) == -1) {
        perror("Error al cerrar la cola del servidor");
    } else {
        printf("SERVIDOR: Cola del servidor cerrada correctamente.\n");
    }
    if (mq_unlink("/SERVIDOR") == -1) {
        perror("Error al desvincular la cola del servidor");
    } else {
        printf("SERVIDOR: Cola del servidor desvinculada correctamente.\n");
    }

    pthread_mutex_destroy(&claves_mutex);
    printf("SERVIDOR: Mutex destruido.\n");

    exit(0);  // Salir del programa
}

// Función que maneja cada petición en un hilo separado
void *tratar_peticion(void *arg) {
    struct peticion *p = (struct peticion *)arg;
    struct peticion p_local;
    struct respuesta r;

    // Inicializar la respuesta
    r.status = -1;  // Valor por defecto en caso de error
    r.value1[0] = '\0';  // Inicializar value1
    r.N_value2 = 0;  // Inicializar N_value2
    r.coord = (struct Coord){0, 0};  // Inicializar coord

    printf("SERVIDOR: Procesando petición en hilo...\n");
    printf("SERVIDOR: Operación solicitada: %d\n", p->op);

    // Copiar la petición a una variable local
    pthread_mutex_lock(&sync_mutex);
    p_local = *p;
    sync_copied = TRUE;  // Indicar que la copia se ha completado
    pthread_cond_signal(&sync_cond);  // Notificar al hilo principal
    pthread_mutex_unlock(&sync_mutex);

    switch (p_local.op) {
        case 0:  // Destroy
            printf("SERVIDOR: Ejecutando operación DESTROY\n");
            pthread_mutex_lock(&claves_mutex);
            r.status = destroy();
            if (r.status == -1) {
                perror("Error en destroy()");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        case 1:  // Set
            printf("SERVIDOR: Ejecutando operación SET\n");
            printf("SERVIDOR: key = %d, value1 = %s, N_value2 = %d, coord = (%d, %d)\n",
                   p_local.key, p_local.value1, p_local.N_value2, p_local.coord.x, p_local.coord.y);
            pthread_mutex_lock(&claves_mutex);
            r.status = set_value(p_local.key, p_local.value1, p_local.N_value2, p_local.V_value2, p_local.coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en set_value()\n");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        case 2:  // Get
            printf("SERVIDOR: Ejecutando operación GET\n");
            printf("SERVIDOR: key = %d\n", p_local.key);
            pthread_mutex_lock(&claves_mutex);
            r.status = get_value(p_local.key, r.value1, &r.N_value2, r.V_value2, &r.coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en get()\n");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        case 3:  // Modify
            printf("SERVIDOR: Ejecutando operación MODIFY\n");
            printf("SERVIDOR: key = %d, value1 = %s, N_value2 = %d, coord = (%d, %d)\n",
                   p_local.key, p_local.value1, p_local.N_value2, p_local.coord.x, p_local.coord.y);
            pthread_mutex_lock(&claves_mutex);
            r.status = modify_value(p_local.key, p_local.value1, p_local.N_value2, p_local.V_value2, p_local.coord);
            if (r.status == -1) {
                fprintf(stderr, "Error en modify()\n");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        case 4:  // Delete
            printf("SERVIDOR: Ejecutando operación DELETE\n");
            printf("SERVIDOR: key = %d\n", p_local.key);
            pthread_mutex_lock(&claves_mutex);
            r.status = delete_key(p_local.key);
            if (r.status == -1) {
                fprintf(stderr, "Error en delete()\n");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        case 5:  // Exist
            printf("SERVIDOR: Ejecutando operación EXIST\n");
            printf("SERVIDOR: key = %d\n", p_local.key);
            pthread_mutex_lock(&claves_mutex);
            r.status = exist(p_local.key);
            if (r.status == -1) {
                fprintf(stderr, "Error en exist()\n");
            }
            pthread_mutex_unlock(&claves_mutex);
            break;
        default:
            printf("Operación no reconocida: %d\n", p_local.op);
            r.status = -1;
            break;
    }

    printf("SERVIDOR: Resultado de la operación: status = %d\n", r.status);
    printf("SERVIDOR: Enviando respuesta al cliente...\n");

    // Configurar atributos de la cola del cliente
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct respuesta);
    attr.mq_curmsgs = 0;

    // Abrir la cola de respuesta del cliente
    int qr = mq_open(p_local.q_name, O_WRONLY, 0700, &attr);
    if (qr == -1) {
        perror("Error al abrir la cola de respuesta");
        pthread_exit(NULL);
    }

    // Enviar la respuesta al cliente
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

    // Cerrar la cola de respuesta
    mq_close(qr);

    // Finalizar el hilo
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    struct peticion p;
    unsigned int prio;
    pthread_t thid;
    pthread_attr_t attr;

    signal(SIGINT, limpiar_y_salir);


    // Inicializar atributos del hilo
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Configurar atributos de la cola del servidor
    struct mq_attr attr_queue;
    attr_queue.mq_flags = 0;
    attr_queue.mq_maxmsg = 10;
    attr_queue.mq_msgsize = sizeof(struct peticion);
    attr_queue.mq_curmsgs = 0;

    printf("SERVIDOR: Abro la cola del /SERVIDOR...\n");
    int qs = mq_open("/SERVIDOR", O_CREAT | O_RDONLY, 0700, &attr_queue);
    if (qs == -1) {
        perror("Error al abrir la cola del servidor");
        return -1;
    }

    while (1) {
        printf("SERVIDOR: Esperando petición...\n");

        // Recibir la petición
        if (mq_receive(qs, (char *) &p, sizeof(struct peticion), &prio) == -1) {
            perror("Error en mq_receive");
            continue;
        }

        printf("SERVIDOR: Petición recibida\n");
        printf("SERVIDOR: op = %d\n", p.op);
        printf("SERVIDOR: key = %d\n", p.key);

        // Crear un hilo para manejar la petición
        if (pthread_create(&thid, &attr, tratar_peticion, (void *) &p) != 0) {
            perror("Error al crear el hilo");
            continue;
        }

        // Esperar a que el hilo copie la petición
        pthread_mutex_lock(&sync_mutex);
        while (sync_copied == FALSE) {
            pthread_cond_wait(&sync_cond, &sync_mutex);
        }
        sync_copied = FALSE;  // Reiniciar la bandera
        pthread_mutex_unlock(&sync_mutex);
    }

}