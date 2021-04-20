#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"

typedef struct{
    char nombre[101];
    int existencia;
    char tipos[101];
    char previa[101];
    char posterior[101];
    int numero_pokedex;
    char region [101];
}Pokedex;

typedef struct{
    int id;
    char nombre[101];
    int pc;
    int ps;
    char sexo[101];
}Almacenamiento;


/*
  función para comparar claves de tipo string
  retorna 1 si son iguales
*/
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo string
  retorna 1 si son key1<key2
*/
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son iguales
*/
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

/*
  función para comparar claves de tipo int
  retorna 1 si son key1<key2
*/
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}


void display_menu(){
    printf("Escoja el numero\n");
    printf("1.Importar/Exportar desde un archivo CSV:\n");
    printf("2.Pokemon atrapado:\n");
    printf("3.Evolucionar pokemon:\n");
    printf("4.Buscar mis pokemon por tipo:\n");
    printf("5.Buscar mis pokemon por nombre:\n");
    printf("6.Buscar por nombre en pokedex:\n");
    printf("7.Mostrar todos los pokemon de la pokedex:\n");
    printf("8.Mostrar mis pokemon ordenados por PC:\n");
    printf("9.Liberar pokemon:\n");
    printf("10.Mostrar pokemon por region:\n");
    printf("0.Salir\n");
}

int ver_repetido(Pokedex *datos_pokedex, Map *pokedex_map, char *lectura){
    Pokedex *iterador = firstMap(pokedex_map);
    while (iterador != NULL)
    {
        if(strcmp(lectura, iterador->nombre) == 0){
            return 1;
        }
        iterador = nextMap(pokedex_map);
    }
    return 0;
    
}

void agregar_pokemon(Map *pokedex_map, Map *almacenamiento_map, char *lectura, int *cont, Almacenamiento *datos_almacenamiento, Pokedex *datos_pokedex){    
    int numero;
    
    if(*cont == 0){
        numero = atoi(lectura);
        datos_almacenamiento->id = numero;
    }
    if(*cont == 1){
        if(ver_repetido(datos_pokedex, pokedex_map, lectura) == 1){
            datos_pokedex->existencia++;
        }
        strcpy(datos_almacenamiento->nombre,lectura);
        strcpy(datos_pokedex->nombre, lectura);
        datos_pokedex->existencia = 1;
    }
    if(*cont == 2){
        strcpy(datos_pokedex->tipos, lectura);
    }
    if(*cont == 3){
        numero = atoi(lectura);
        datos_almacenamiento->pc = numero;
    }
    if(*cont == 4){
        numero = atoi(lectura);
        datos_almacenamiento->ps = numero;
    }
    if(*cont == 5){
        strcpy(datos_almacenamiento->sexo, lectura);
    }
    if(*cont == 6){
        strcpy(datos_pokedex->previa, lectura);
    }
    if(*cont == 7){
        strcpy(datos_pokedex->posterior, lectura);
    }
    if(*cont == 8){
        numero = atoi(lectura);
        datos_pokedex->numero_pokedex = numero;
    }
    if(*cont == 9){
        strcpy(datos_pokedex->region, lectura);


        insertMap(pokedex_map, datos_pokedex->nombre, datos_pokedex);
        insertMap(almacenamiento_map, &datos_almacenamiento->id, datos_almacenamiento);

        Pokedex *iterador;
        iterador = firstMap(pokedex_map);
        Almacenamiento *ayuda;
        ayuda = firstMap(almacenamiento_map);
        printf("%s  ", searchMap(pokedex_map, "Charmander"));
        printf("id:%i existencia:%s    \n", ayuda->id, iterador->tipos );
        *cont = -1;
    }
   
    lectura = (char *)malloc(sizeof(char) * 100);
    *cont +=1;

}

int ver_dato(char *lectura){
    int i = strlen(lectura);
    if(lectura[0] == 34){
        if(lectura[i - 1] == 34){
            return 1;
        }
        return 0;
    }

    return 1;
}

void agregar(char *lectura, char *tipo){
    int i = strlen(lectura);
    for(int k = 0; k<strlen(tipo); k++){
        if(k == 0) lectura[i] = ',';
        else lectura[i] = tipo[k];
        i++;
    }
    lectura[i] = '\0';
}


void importar_archivo(Map *pokedex_map, Map *almacenamiento_map, Almacenamiento *datos_almacenamiento, Pokedex *datos_pokedex){
    char nombre_archivo[101];
    char *lectura = (char *)malloc(sizeof(char) * 100);
    int cont = 0;
    int verificar = 0;
    char *tipo = (char *)malloc(sizeof(char) * 100);

    printf("Ingrese el nombre del archivo\n");
    scanf("%s", nombre_archivo);

    FILE *archivo = fopen(nombre_archivo, "r");
    
    if(archivo == NULL){
        exit(EXIT_FAILURE);
    }

    while(fscanf(archivo, "%[^,\n]s", lectura ) != EOF){

        while (ver_dato(lectura) == 0)
        {
            fgetc(archivo);
            fscanf(archivo, "%[^,\n]s", tipo );
            agregar(lectura, tipo);
            tipo = (char *)malloc(sizeof(char) * 100);
        }
        fgetc(archivo);
        verificar += 1;
        if(verificar > 10){
        agregar_pokemon(pokedex_map, almacenamiento_map, lectura, &cont, datos_almacenamiento, datos_pokedex);
        }
    }

}

int main(){

    Map *pokedex_map = createMap(is_equal_string);
    setSortFunction(pokedex_map, lower_than_string);
    Map *almacenamiento_map = createMap(is_equal_int);

    Pokedex *datos_pokedex = (Pokedex *)malloc(sizeof(Pokedex));
    Almacenamiento *datos_almacenamiento = (Almacenamiento *)malloc(sizeof(Almacenamiento));

    int opcion;

    do{
        display_menu();
        scanf("%i", &opcion);

        switch (opcion)
        {
        case 1:
            importar_archivo(pokedex_map, almacenamiento_map, datos_almacenamiento, datos_pokedex);
             printf("%s  ", searchMap(pokedex_map, "Charmander"));
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 0:
            return 0;
            break;
        default:
            printf("Ingrese una opcion valida\n");
            break;
        }

    }while(opcion != 0);

    return 0;
}
