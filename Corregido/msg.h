#ifndef MSG_H  // Guarda para evitar inclusiones múltiples
#define MSG_H

#include "claves.h"  // Para usar struct Coord
#include <mqueue.h>  // Para tipos de colas de mensajes

#define MAX 256      // Tamaño máximo para cadenas/arrays

// Estructura para PETICIONES (cliente -> servidor)
struct peticion {
    int op;               // Código de operación (0: destroy, 1: set_value, etc.)
    int key;              // Clave (número entero)
    char value1[MAX];     // Cadena de texto (valor1)
    int N_value2;         // Tamaño del array V_value2
    double V_value2[32];  // Array de doubles (valor2) - Tamaño FIJO 32
    struct Coord coord;   // Estructura con x,y (valor3)
    char q_name[MAX];     // Nombre de la cola de respuesta del cliente
};

// Estructura para RESPUESTAS (servidor -> cliente)
struct respuesta {
    int status;           // Resultado (0: éxito, -1: error, -2: error comunicación)
    char value1[MAX];     // Valor1 devuelto (para get_value)
    int N_value2;         // Tamaño del array V_value2 devuelto
    double V_value2[32];  // Array de doubles devuelto
    struct Coord coord;   // Estructura devuelta
};

#endif // MSG_H