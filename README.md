# Tarea3
Programa GraphQuest

## Descripcion
GraphQuest es un juego tipo puzzle de un laberinto a superar con un maximo de tiempo, intentando conseguir la mayor cantidad depuntaje posible

## Requisitos previos
Compilador de C: GCC (Linux/WSL) o MinGW (Windows)

Bibliotecas requeridas: list.h, extra.h (incluidas en el proyecto)

Editor recomendado: Visual Studio Code con extensión C/C++

Dataset llamado graphquest.csv con la informacion

## Compilacion y ejecucion 

Clonar repositorio:
```
bash
git clone https://github.com/LoliWokey/Tarea3.git
```

Compilar:

Primero hay que ingresar correctamente en la carpeta donde fue descargado y guardado

(Luego)
```
bash
gcc tarea3.c tdas/list.c tdas/extra.c tdas/heap.c -o tarea3.exe
```

Ejecutar:
```
bash
./tarea3.exe
```
## Funcionalidades
---- Sistema de Salas y Movimiento ----

Exploración de escenarios conectados (arriba, abajo, izquierda, derecha)

Descripciones detalladas de cada escenario

---- Gestión de Items ----

Recoger items de los escenarios (sistema de selección numérica)

Descartar items del inventario

Visualización detallada de items (nombre, valor, peso)

---- Sistema de Juego ----

Tiempo limitado (10 unidades iniciales)

Costo de tiempo por acciones (movimiento, recoger/descartar items)

Cálculo dinámico del tiempo de movimiento según peso

Sistema de puntuación basado en items recolectados

---- Gestión de Partida ----

Reinicio completo de partida con confirmación

Persistencia de items entre reinicios (mediante recarga del CSV)

---- Interfaz de Usuario ----

Menú de acciones numerado

Estado actual visible (tiempo, puntuación, peso)

Sistema de selección por números (no por nombres)

Mensajes de error/confirmación claros

---- Archivos y Datos ----

Carga de escenarios desde archivo CSV

Limpieza de memoria al reiniciar/salir

Estructura de datos eficiente para gestión de items

## Problemas conocidos

Al soltar un objeto, se pierde eternamente en la partida

Error grave de finalizacion, nunca llega a su fin aunque este en la ultima sala

Dificultad de legibilidad del juego 

## Mejoras planeadas

Finalizacion del juego y poder repetirse la cantidad veces queridas

Cuando se suelte un objeto, ese objeto va a poderese recojer en la sala soltada nuevamente

Mas legibilidad con limpiarPantalla(da errores)

Añadir enemigos que se puedan mover por el laverinto

Multijugador

##Ejemplo de uso

Este programa es bastante simple al ser todo indexado con numeros

1) Recoger item
```
Items disponibles en el escenario:
(insertar numero que aparece para escojer, si no es asi sera devuelto al menu)
```
2) Descartar Item
```
Items en tu inventario:
(si tienes items en el inventario, selecciona el numero mostrado para borrarlo o 0 para cancelarlo)
```
3) Moverse
```
Movimientos disponibles desde (sala/escena):
(van a aparecer todas las sals posibles por pasar, seleccionar numero respectivo para moverse a la sala requerida o 0 para cancelar)
```
4) Reiniciar partida
```
¿Estas seguro que quieres reiniciar la partida?:
(1 si quieres reiniciar, 2 si no)
```
5) Salir
```
Saliendo del juego...
```
