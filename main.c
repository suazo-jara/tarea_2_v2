#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Map.h"
#include "list.h"

typedef struct{
    int id;
    char nombre[30];
    int PC;
    int PS;
    char sexo;
}Almacenamiento;

typedef struct{
    char nombre[30];
    int existencias;
    List *tipos;
    char evo_previa[30];
    char evo_posterior[30];
    int numero;
    char region[15];
}Pokedex;

//Función para comparar claves de tipo string. Retorna 1 si son iguales
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

//Función para comparar claves de tipo string. Retorna 1 si  key1 < key2
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}

//Función para comparar claves de tipo int. Retorna 1 si son iguales
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}

//Función para comparar claves de tipo int. Retorna 1 si key1 < key2
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}

//Función para comparar claves de tipo int. Retorna 1 si key1 > key2
int greater_than_int(void * key1, void * key2) {
    if(*(int*)key1 > *(int*)key2) return 1;
    return 0;
}

//Función que retorna el almacenamiento con los datos agregados de un nuevo pokemon
Almacenamiento *guardarAlmacenamiento(int id, char *nombre, int PC, int PS, char sexo){
    Almacenamiento *p = (Almacenamiento*) malloc (sizeof(Almacenamiento));
    p->id = id;
    strcpy(p->nombre, nombre);
    p->PC = PC;
    p->PS = PS;
    p->sexo = sexo;
    return p;
}

//Función que retorna la pokedex con los datos agregados de un nuevo pokemon
Pokedex *guardarPokedex(char *nombre, List *tipos, char *evo_previa, char *evo_posterior, int numero_pokedex, char *region){
    Pokedex *p = (Pokedex*) malloc (sizeof(Pokedex));
    strcpy(p->nombre, nombre);
    p->tipos = tipos;
    strcpy(p->evo_previa, evo_previa);
    strcpy(p->evo_posterior, evo_posterior);
    p->numero = numero_pokedex;
    strcpy(p->region, region);
    return p;
}

//Función para ingresar los datos nuevos en la pokédex y en el almacenamiento
void ingresar_en_mapas(Almacenamiento *almac, Pokedex *pokedex, Map *pokemon_por_id, Map *pokemon_por_tipo, Map *nombre_almacenamiento, Map *nombre_pokedex, Map *mapa_numero_pokedex, Map *pokemon_por_PC, Map *pokemon_por_region){
    insertMap(pokemon_por_id, &almac->id, almac);
    
    insertMap(nombre_almacenamiento, almac->nombre, almac);
    insertMap(nombre_pokedex, almac->nombre, pokedex);
    insertMap(mapa_numero_pokedex, &pokedex->numero, pokedex);
    insertMap(pokemon_por_PC, &almac->PC, almac);
    insertMap(pokemon_por_region, pokedex->region, pokedex);
    
    //printf("%d - %s - %d - %d - %c\n", almac->id, almac->nombre, almac->PC, almac->PS, almac->sexo);
    printf("%s - %s - %s - %d - %s\n", pokedex->nombre, pokedex->evo_previa, pokedex->evo_posterior, pokedex->numero, pokedex->region);
}

//Función que lee el archivo
void importarArchivo(Map *pokemon_por_id, Map *pokemon_por_tipo, Map *nombre_almacenamiento, Map *nombre_pokedex, Map *mapa_numero_pokedex, Map *pokemon_por_PC, Map *pokemon_por_region)
{
    char *lectura = (char *)malloc(sizeof(char) * 100);
    //char nombre_archivo[101];
    //scanf("%s", nombre_archivo);
    FILE *archivo = fopen("pokemon Archivo2.csv", "r");
    if (archivo == NULL) exit(EXIT_FAILURE);

    int id, PC, PS, numero_pokedex;
    char nombre[30], sexo, evo_previa[30], evo_posterior[30], region[15];
    List *lista_tipos = create_list();

    int modo_tipo = 0;
    int cont_datos = 0;
    int total_tipos = 0;
    int primera_lectura = 1;
    
    //Se leen los datos del archivo coma por coma hasta que no haya nada por leer
    while (fscanf(archivo, "%[^,\n]s", lectura) != EOF){
        fgetc(archivo);

        //Proceso para omitir la primera línea del .csv
        if (primera_lectura){
            if (cont_datos == 9){
                primera_lectura = 0;
                cont_datos = 0;
                continue;
            }
            else{
                cont_datos++;
                continue;
            }
        }

        modo_tipo = 0;
        int largo_palabra = strlen(lectura);
        if (!isalnum(lectura[0])){
            modo_tipo = 1;
            cont_datos--;
        }

        if (lectura[largo_palabra - 1] == '"'){
            cont_datos++;
            modo_tipo = 1;
        }

        if (!modo_tipo && cont_datos == 2) total_tipos++;
        if (modo_tipo) total_tipos++;

        if (cont_datos == 0) id = atoi(lectura);
        if (cont_datos == 1 && !modo_tipo) strcpy(nombre, lectura);
        if ((modo_tipo && cont_datos >= 1) || (!modo_tipo && cont_datos == 2)){
            
            int j = 0;
            for(int i = 0; i < largo_palabra; i++){
                if(isalnum(lectura[i])){
                    lectura[j] = lectura[i];
                    j++;
                }
            }
            lectura[j] = '\0';

            push_back(lista_tipos, lectura);
        }
        if (cont_datos == 3) PC = atoi(lectura);
        if (cont_datos == 4) PS = atoi(lectura);
        if (cont_datos == 5){
            largo_palabra = strlen(lectura);
            if(strcmp("Hembra", lectura) == 0) sexo = 'F';
            else sexo = 'M';
        }
        if (cont_datos == 6){
            largo_palabra = strlen(lectura);
            if(strcmp("No tiene", lectura) == 0) strcpy(evo_previa, "(--)");
            else strcpy(evo_previa, lectura);
        }
        if (cont_datos == 7){
            largo_palabra = strlen(lectura);
            if(strcmp("No tiene", lectura) == 0) strcpy(evo_posterior, "(--)");
            else strcpy(evo_posterior, lectura);
        }
        if (cont_datos == 8) numero_pokedex = atoi(lectura);
        if (cont_datos == 9) strcpy(region, lectura);

        cont_datos++;
        if (cont_datos == 10){
            Almacenamiento *almac = guardarAlmacenamiento(id, nombre, PC, PS, sexo);
            Pokedex *pokedex = guardarPokedex(nombre, lista_tipos, evo_previa, evo_posterior, numero_pokedex, region);
            ingresar_en_mapas(almac, pokedex, pokemon_por_id, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region);
            cont_datos = 0;
            total_tipos = 0;
            lista_tipos = create_list();
        }
        lectura = (char *)malloc(sizeof(char) * 100);
    }
}

int main(){
    //Mapa utilizado para evolucionar pokémon por ID
    Map *pokemon_por_id = createMap(is_equal_int);
    setSortFunction(pokemon_por_id, lower_than_int);

    //Mapa utilizado para buscar pokémon por tipo
    Map *pokemon_por_tipo = createMap(is_equal_string);
    setSortFunction(pokemon_por_tipo, lower_than_string);

    //Mapa utilizado para buscar pokémon por nombre
    Map *nombre_almacenamiento = createMap(is_equal_string);
    setSortFunction(nombre_almacenamiento, lower_than_string);

    //Mapa utilizado para buscar pokémon por nombre en pokédex
    Map *nombre_pokedex = createMap(is_equal_string);
    setSortFunction(nombre_pokedex, lower_than_string);

    //Mapa utilizado para mostrar a todos los pokémon de la pokédex ordenados por número
    Map *mapa_numero_pokedex = createMap(is_equal_int);
    setSortFunction(mapa_numero_pokedex, lower_than_int);

    //Mapa utilizado para mostrar a los pokémon del almacenamiento por PC
    Map *pokemon_por_PC = createMap(is_equal_int);
    setSortFunction(pokemon_por_PC, greater_than_int);

    //Mapa utilizado para mostrar a los pokémon por región
    Map *pokemon_por_region = createMap(is_equal_string);
    setSortFunction(pokemon_por_region, lower_than_string);

    int escaneo;

    do{
        printf("Opcion:");
        scanf("%d", &escaneo);
        if (escaneo == 1) importarArchivo(pokemon_por_id, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region);
        
        if (escaneo == 7){
            Pokedex *pokedex = firstMap(mapa_numero_pokedex);
            while(pokedex){
                printf("%d - %s - %s - %s - %s\n", pokedex->numero, pokedex->nombre, pokedex->evo_previa, pokedex->evo_posterior, pokedex->region);
                pokedex = nextMap(mapa_numero_pokedex);
            }
        }
        if (escaneo == 8){
            Almacenamiento *almac = firstMap(pokemon_por_PC);
            while(almac){
                printf("%d - %s - %d - %d - %c\n", almac->PC, almac->nombre, almac->id, almac->PS, almac->sexo);
                almac = nextMap(pokemon_por_PC);
            }
        }
    }while(escaneo != 0);
}