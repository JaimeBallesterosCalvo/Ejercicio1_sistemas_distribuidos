#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"
#include <stdlib.h>
#include <unistd.h>


int destroy(void);
{
    /*todo esto es para poder ver si está bien lo que estamos haciendo*/
    /*se puede de hacer de otras maneras, pero con ficheros y un cat siempre funciona*/
    system("rm -fr ./KV");

    int ret = mkdir("KV", 0700);
    if (ret <0){
        perror("mkdir");
        return -1;
    }

    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
{
    char fname[1024];

    sprintf(fname, "./KV/%d", key);

    fd =fopen(fname, "w+");
    if (NULL ==fd){ /**hay que poner toda la comprobación de errores que se puedan**/
        printf("ERROR: fopen de %\n", fname)
    }

    fprintf(fd, "%s\n", value1);
    fprintf(fd, "%d\n", N_value2);
    for (int i=0; i<N_value2; i++)
        fprintf(fd, "%le\n", V_value2[i]);
    }
    fprintf(fd, "%d\n", value3.x);
    fprintf(fd, "%d\n", value3.y);

    fclose(fd);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);
{
    /* en este caso, es parecido al de set value, pero esta vez cogera datos*/
    /* es por ello que hace scanf en vez de printf*/

    char fname[1024];
    FILE *fd;

    sprintf(fname, "./KV/%d", key);

    fd =fopen(fname, "r+");
    if (NULL ==fd){ /**hay que poner toda la comprobación de errores que se puedan**/
        printf("ERROR: fopen de %\n", fname)
    }

    fscan(fd, "%s\n", value1);
    fscan(fd, "%1023[^\n]\n", value1); /*esto es para leer varias palabras, significa que lee hasta un enter*/
    fscan(fd, "%d\n", *N_value2);
    for (int i=0; i<*N_value2; i++){
        fscan(fd, "%le\n", &(V_value2[i]));
    }
    fscan(fd, "%d\n", &(value3->x) ); /*quiero acceder a este puntero al campo X*/
    fscan(fd, "%d\n", &((*value3).y) ); /*es lo mismo pero en otra notación*/

    fclose(fd);
    }

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    char fname[1024];
    FILE *fd;

    /*Rango de N_value 2*/
    if (N_value2 < 1 || N_value2 > 32) {
        return -1;  /*Error, N_value 2 está fuera de rango*/
    }

    sprintf(fname, "./KV/%d", key);

    fd = fopen(fname, "w"); 
    if (fd == NULL) {
        return -1; /*Error, no existe un elemento con dicha clave*/
    }

    fprintf(fd, "%s\n", value1);  
    fprintf(fd, "%d\n", N_value2);  
    
    for (int i = 0; i < N_value2; i++) {
        fprintf(fd, "%le\n", V_value2[i]);  
    }

    fprintf(fd, "%d\n", value3.x);  
    fprintf(fd, "%d\n", value3.y);  

    
    fclose(fd);

    return 0;  


/**
 * @brief Este servicio permite modificar los valores asociados a la clave key. 
 * La función devuelve 0 en caso de éxito y -1 en caso de error, por ejemplo, 
 * si no existe un elemento con dicha clave o si se produce un error en las comunicaciones. 
 * También se devolverá -1 si el valor N_value2 está fuera de rango.
 * 
 * 
 * @param key clave.
 * @param value1   valor1 [256].
 * @param N_value2 dimensión del vector V_value2 [1-32].
 * @param V_value2 vector de doubles [32].
 * @param value3   estructura Coord.
 * @return int El servicio devuelve 0 si se insertó con éxito y -1 en caso de error.
 * @retval 0 si se modificó con éxito.
 * @retval -1 en caso de error.
 */
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);

/**
 * @brief Este servicio permite borrar el elemento cuya clave es key. 
 * La función devuelve 0 en caso de éxito y -1 en caso de error. 
 * En caso de que la clave no exista también se devuelve -1.
 * 
 * @param key clave.
 * @return int La función devuelve 0 en caso de éxito y -1 en caso de error.
 * @retval 0 en caso de éxito.
 * @retval -1 en caso de error.
 */
int delete_key(int key);

/**
 * @brief Este servicio permite determinar si existe un elemento con clave key.
 * La función devuelve 1 en caso de que exista y 0 en caso de que no exista. 
 * En caso de error se devuelve -1. Un error puede ocurrir en este caso por un problema en las comunicaciones.
 * 
 * @param key clave.
 * @return int La función devuelve 1 en caso de que exista y 0 en caso de que no exista. En caso de error se devuelve -1.
 * @retval 1 en caso de que exista.
 * @retval 0 en caso de que no exista.
 * @retval -1 en caso de error.
 */
int exist(int key);

#endif
