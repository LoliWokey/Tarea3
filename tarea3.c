#include <stdio.h>
#include "tdas/extra.h"
#include "tdas/list.h"
#include <stdlib.h>
#include <string.h>
#define maxescenarios 17  //ya que hay un maximo de 16 datos en el csv es mas simple y se puede hacer en una lista
                          //el problema es que si se agregaran mas escenarios habria que hacer varios cambios importantes en la estructura
typedef struct {
    char *nombre;
    int valor;
    int peso;
} item;

typedef struct {
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
} posibilidades;

typedef struct {
    char *nombre;
    int id;
    char *descripcion;
    List *items;
    posibilidades enlaces;
    int es_final;
} salas;

salas *fullSalas[maxescenarios];

typedef struct {
    float tiempo;
    int puntajeTotal;
    int pesoTotal;
    List *items;
    salas *escenario_actual;
} player;

item *crearItem(char *nombre, int valor, int peso) {
    item *nuevo_item = malloc(sizeof(item));
    nuevo_item->nombre = strdup(nombre);
    nuevo_item->valor = valor;                    //crea un item, no mucho que decir
    nuevo_item->peso = peso;
    return nuevo_item;
}

void limpiarEscenarios() {
    for (int i = 0; i < maxescenarios; i++) {
        if (fullSalas[i] != NULL) {
            list_clean(fullSalas[i]->items);
            free(fullSalas[i]->items);
            free(fullSalas[i]->nombre);
            free(fullSalas[i]->descripcion);    //libera todos los escenarios para leer nuevamente el csv
            free(fullSalas[i]);                 //lo hice asi porque es un enredo dejar listas con la estructura basica hecha
            fullSalas[i] = NULL;                //es mas lento el reinicio pero debido a que el laberinto es pequeño no se nota en el reinicio
        }
    }
}

void leer_escenarios() {
    limpiarEscenarios();

    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    fgets((char[256]){0}, 256, archivo);

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        int id = atoi(campos[0]);
        salas *nueva_sala = malloc(sizeof(salas));
        nueva_sala->id = id;
        nueva_sala->nombre = strdup(campos[1]);
        nueva_sala->descripcion = strdup(campos[2]);      //añadir cada sala
        nueva_sala->items = list_create();
        nueva_sala->es_final = atoi(campos[8]);

        List *items = split_string(campos[3], ";");
        for (char *item_str = list_first(items); item_str != NULL; item_str = list_next(items)) {
            List *valores = split_string(item_str, ",");
            char *nombre_item = strdup(list_first(valores));        //añadir item a cada sala
            int valor = atoi(list_next(valores));
            int peso = atoi(list_next(valores));
            item *nuevo_item = crearItem(nombre_item, valor, peso);
            list_pushBack(nueva_sala->items, nuevo_item);
            list_clean(valores);
            free(valores);
        }
        list_clean(items);
        free(items);
        nueva_sala->enlaces.arriba = atoi(campos[4]);
        nueva_sala->enlaces.abajo = atoi(campos[5]);     //añadir enlaces a cada sala
        nueva_sala->enlaces.izquierda = atoi(campos[6]);
        nueva_sala->enlaces.derecha = atoi(campos[7]);
        fullSalas[id] = nueva_sala;
    }
    fclose(archivo);
    printf("Escenarios leídos correctamente.\n");
}

void recogerItems(player *jugador) {
    printf("Items disponibles en el escenario '%s':\n", jugador->escenario_actual->nombre);
    List *items = jugador->escenario_actual->items;
    
    int index = 0;
    for (item *i = list_first(items); i != NULL; i = list_next(items)) {
        printf("%d. %s (Valor: %d, Peso: %d)\n", ++index, i->nombre, i->valor, i->peso);
    }

    if (index == 0) {
        printf("No hay items disponibles en este escenario.\n");
        return;
    }

    printf("Seleccione el número del item a recoger (0 para cancelar): ");
    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 1 || opcion > index) {    //validar la opcion
        printf("Opción inválida o cancelada.\n");                       //de todas las opciones mostradas simplemente recorre la lista en busqueda del item seleccionado por el numero
        while (getchar() != '\n');
        return;
    }

    list_first(items);
    for (int i = 1; i < opcion; i++) {
        list_next(items);
    }
    item *item_seleccionado = list_popCurrent(items);                   // lo borra de la sala actual y lo agrega al inventario del jugador
                                                                        //asi generamos items unicos en el inventario del jugador
    if (jugador->pesoTotal + item_seleccionado->peso <= 10) {           //el problema es que se sueltan los items desaparecen
        list_pushBack(jugador->items, item_seleccionado);
        jugador->puntajeTotal += item_seleccionado->valor;
        jugador->pesoTotal += item_seleccionado->peso;
        jugador->tiempo--;
        printf("Item '%s' recogido.\n", item_seleccionado->nombre);
    } else {
        printf("No puedes recoger '%s', excede tu capacidad de peso.\n", item_seleccionado->nombre);
        list_pushBack(items, item_seleccionado);
    }
}

void descartarItems(player *jugador) {
    printf("Items en tu inventario:\n");
    int index = 0;
    for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items)) {
        printf("%d. %s (Valor: %d, Peso: %d)\n", ++index, i->nombre, i->valor, i->peso);
    }
    printf("0. Cancelar\n");

    printf("Seleccione el número del item a descartar: ");
    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 1 || opcion > index) {          //casi igual que recoger items
        printf("Opción inválida o cancelada.\n");                        
        while (getchar() != '\n');
        return;
    }

    list_first(jugador->items);
    for (int i = 1; i < opcion; i++) {
        list_next(jugador->items);
    }
    item *item_seleccionado = list_popCurrent(jugador->items);
    
    jugador->puntajeTotal -= item_seleccionado->valor;
    jugador->pesoTotal -= item_seleccionado->peso;                            //quita el item del inventario del jugador y le quita el peso y el puntaje
    jugador->tiempo--;
    
    free(item_seleccionado->nombre);
    free(item_seleccionado);                                                  // lo borra permanentemente de la memoria
    
    printf("Item descartado.\n");
}

float calcular_tiempo_movimiento(int peso) {
    return (peso + 1) / 10.0f;                                              
}

void mostrarEstadoActual(player *jugador) {
    printf("\n=== Estado Actual ===\n");
    printf("Escenario: %s\n", jugador->escenario_actual->nombre);
    printf("Descripción: %s\n\n", jugador->escenario_actual->descripcion);
    printf("Tiempo restante: %.1f\n", jugador->tiempo);                           //solo para mostrar estadisticas
    printf("Puntaje total: %d\n", jugador->puntajeTotal);
    printf("Peso total: %d/10\n", jugador->pesoTotal);
    printf("Inventario:\n");
    
    if (list_first(jugador->items) != NULL) {
        for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items)) {
            printf("  - %s (Valor: %d, Peso: %d)\n", i->nombre, i->valor, i->peso);
        }
    } else {
        printf("  (vacío)\n");
    }
}

void reiniciarPartida(player *jugador) {
    printf("\n¿Estás seguro que quieres reiniciar la partida?\n");
    printf("1. Sí, reiniciar\n");
    printf("2. No, continuar jugando\n");                                       //gracias a esto me esta dando fallos en la finalizacion del juego
    printf("Selección: ");                                                      //(lo presiento)
    
    int opcion;
    if (scanf("%d", &opcion) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    if (opcion == 1) {
        list_clean(jugador->items);
        free(jugador->items);
        leer_escenarios();
        jugador->tiempo = 10.0f;
        jugador->puntajeTotal = 0;
        jugador->pesoTotal = 0;                                                 //pongo las estadisticas de como parte siempre el jugador
        jugador->items = list_create();
        jugador->escenario_actual = fullSalas[1];
        printf("\n¡Partida reiniciada!\n");
    } else {
        printf("\nContinuando con la partida actual...\n");
    }
}

void finalizarJuego(player *jugador, int victoria) {
    printf("\n=== %s ===\n", victoria ? "¡FELICIDADES, HAS GANADO!" : "FIN DEL JUEGO");
    printf("Puntaje final: %d\n", jugador->puntajeTotal);
    printf("Tiempo restante: %.1f\n", jugador->tiempo);                          //esta funcion en teoria deberia finalizar el juego
    printf("Items recolectados:\n");                                             //debe ser un error de while en el main pero e intentado de todo y no lo puedo arreglar
    
    if (list_first(jugador->items) != NULL) {
        for (item *i = list_first(jugador->items); i != NULL; i = list_next(jugador->items)) {
            printf("  - %s (Valor: %d, Peso: %d)\n", i->nombre, i->valor, i->peso);
        }
    } else {
        printf("  (ninguno)\n");
    }
    
    printf("\n¿Qué deseas hacer?\n");
    printf("1. Jugar otra vez\n");
    printf("2. Salir del juego\n");                                               //aqui pregunta para cerrar el programa o no
    printf("Selección: ");
    
    int opcion;
    if (scanf("%d", &opcion) != 1) {
        printf("Entrada inválida. Saliendo del juego...\n");
        exit(0);
    }
    while (getchar() != '\n');

    if (opcion == 1) {
        reiniciarPartida(jugador);
    } else {
        printf("\nGracias por jugar. ¡Hasta la próxima!\n");
        list_clean(jugador->items);
        free(jugador->items);
        limpiarEscenarios();
        exit(0);
    }
}

void movimientosDescenarios(player *jugador) {
    printf("\nMovimientos disponibles desde '%s':\n", jugador->escenario_actual->nombre);
    
    if (jugador->escenario_actual->enlaces.arriba != -1)
        printf("1. Arriba -> %s\n", fullSalas[jugador->escenario_actual->enlaces.arriba]->nombre);
    if (jugador->escenario_actual->enlaces.abajo != -1)
        printf("2. Abajo -> %s\n", fullSalas[jugador->escenario_actual->enlaces.abajo]->nombre);
    if (jugador->escenario_actual->enlaces.izquierda != -1)
        printf("3. Izquierda -> %s\n", fullSalas[jugador->escenario_actual->enlaces.izquierda]->nombre);          //muestra las posibles direcciones a las que se puede mover el jugador
    if (jugador->escenario_actual->enlaces.derecha != -1)                                                         //el problema que no quedan bien enumerados
        printf("4. Derecha -> %s\n", fullSalas[jugador->escenario_actual->enlaces.derecha]->nombre);
    
    printf("0. Cancelar\n");
    printf("Seleccione una dirección (1-4): ");
    
    int opcion;
    if (scanf("%d", &opcion) != 1) {
        printf("Entrada inválida.\n");
        while (getchar() != '\n');
        return;
    }

    int nuevo_id = -1;
    switch (opcion) {
        case 1: nuevo_id = jugador->escenario_actual->enlaces.arriba; break;
        case 2: nuevo_id = jugador->escenario_actual->enlaces.abajo; break;
        case 3: nuevo_id = jugador->escenario_actual->enlaces.izquierda; break;               //aqui se asigna el nuevo id dependiendo de la opcion seleccionada
        case 4: nuevo_id = jugador->escenario_actual->enlaces.derecha; break;                 //va a la casilla con esa id
        case 0: printf("Movimiento cancelado.\n"); return;
        default: printf("Opción inválida.\n"); return;
    }

    if (nuevo_id != -1 && nuevo_id < maxescenarios && fullSalas[nuevo_id] != NULL) {
        float costo = calcular_tiempo_movimiento(jugador->pesoTotal);
        
        if (jugador->tiempo >= costo) {
            jugador->escenario_actual = fullSalas[nuevo_id];                                  //aqui le resta el tiempo al jugador dependiendo del peso que tenga                      
            jugador->tiempo -= costo;
            printf("\nTe moviste a '%s'. Tiempo usado: %.1f\n", jugador->escenario_actual->nombre, costo);
            
            if (jugador->escenario_actual->es_final) {
                finalizarJuego(jugador, 1); // Victoria
                return;
            }
        } else {
            printf("No tienes suficiente tiempo (%.1f necesarios, %.1f disponibles).\n", costo, jugador->tiempo);     //si no le alcanza el tiempo
        }
    } else {
        printf("Movimiento inválido o no hay escenario en esa dirección.\n");
    }
}

int main() {
    leer_escenarios();

    int seguir_jugando = 1;                                                 //en teoria deberia reiniciar el juego, pero a veces no para

    while (seguir_jugando) {                                                //intente muchas formas que reinciara y creo que tuve que dejarlo en base a la funcion, no en un while
        player jugador;                                                     //el problema que no me dio el tiempo, si lo hibiera quitado hubiera sido mas simple ver en que falla
        jugador.tiempo = 10.0f;
        jugador.puntajeTotal = 0;
        jugador.pesoTotal = 0;
        jugador.items = list_create();
        jugador.escenario_actual = fullSalas[1];

        while (jugador.tiempo > 0) {
            mostrarEstadoActual(&jugador);
            
            printf("\nOpciones:\n");
            printf("1. Recoger ítem\n");
            printf("2. Descartar ítem\n");
            printf("3. Moverse\n");
            printf("4. Reiniciar partida\n");
            printf("5. Salir\n");
            printf("Selección: ");
            
            int accion;
            if (scanf("%d", &accion) != 1) {
                printf("Entrada inválida.\n");
                while (getchar() != '\n');
                continue;
            }

            switch (accion) {
                case 1: recogerItems(&jugador); break;
                case 2: descartarItems(&jugador); break;
                case 3: movimientosDescenarios(&jugador); break;
                case 4: reiniciarPartida(&jugador); break;
                case 5: 
                    printf("Saliendo del juego...\n");
                    list_clean(jugador.items);
                    free(jugador.items);
                    limpiarEscenarios();
                    return 0;
                default: printf("Opción no válida.\n");
            }
        }

        // Fin de juego por tiempo agotado
        finalizarJuego(&jugador, 0); // este es cuando pierde


        printf("\n¿Quieres jugar otra vez?\n1. Sí\n2. No\nSelección: ");
        int decision;
        if (scanf("%d", &decision) != 1 || decision != 1) {
            seguir_jugando = 0;
            printf("Gracias por jugar.\n");
        }
        while (getchar() != '\n');

        list_clean(jugador.items);
        free(jugador.items);
    }

    limpiarEscenarios();
    return 0;
}