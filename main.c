#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Map.h"

typedef struct{
    int id;
    char nombre[30];
    char **tipos;
    int PC;
    int PS;
    char sexo;
    char evo_previa[30];
    char evo_posterior[30];
    int numero;
    char region[15];
    int existencias;
}Pokemon;

//Función que retorna el pokémon con todos sus datos (p)
Pokemon* crearPokemon(int id, char *nombre, char **tipos, int PC, int PS, char sexo, char* evo_previa, char* evo_posterior, int numero, char *region){
    Pokemon* p = (Pokemon*) malloc (sizeof(Pokemon));
    p->id = id;
    strcpy(p->nombre, nombre);
    //strcpy(p->tipos, tipos);
    p->PC = PC;
    p->PS = PS;
    p->sexo = sexo;
    strcpy(p->evo_previa, evo_previa);
    strcpy(p->evo_posterior, evo_posterior);
    p->numero = numero;
    strcpy(p->region, region);

    return p;
}

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

void introducirMapas(Pokemon *p, Map *pokemon_por_id)
{
    insertMap(pokemon_por_id, &p->id, p);
}

//Función que lee el archivo
void importarArchivo(Map *pokemon_por_id)
{
    char *lectura = (char *)malloc(sizeof(char) * 100);
    //char nombre_archivo[101];
    //scanf("%s", nombre_archivo);
    FILE *archivo = fopen("pokemon prueba.csv", "r");
    if (archivo == NULL) exit(EXIT_FAILURE);

    int id, PC, PS, numero_pokedex, existencias = 0;
    char nombre[30], **tipos, sexo, evo_previa[30], evo_posterior[30], region[15];

    int modo_tipo = 0;
    int cont_datos = 0;
    int total_tipos = 0;
    
    //Se leen los datos del archivo coma por coma hasta que no haya nada por leer
    while (fscanf(archivo, "%[^,\n]s", lectura) != EOF){
        fgetc(archivo);
        
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

        int j = 0;
        for(int i = 0; i < largo_palabra; i++){
            if(isalnum(lectura[i])){
                lectura[j] = lectura[i];
                j++;
            }
        }
        lectura[j] = '\0';

        if (cont_datos == 0) id = atoi(lectura);
        if (cont_datos == 1) strcpy(nombre, lectura);
        if ((modo_tipo && cont_datos >= 1) || (!modo_tipo && cont_datos == 2)){
            tipos = malloc(total_tipos * sizeof(char*));
            for (int i = 0; i < total_tipos; i++){
                tipos[i] = malloc((total_tipos) * sizeof(char));
            }
            //strcpy(tipos[total_tipos - 1], lectura);
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
            if(strcmp("Notiene", lectura) == 0) strcpy(evo_previa, "--");
            else strcpy(evo_previa, lectura);
        }
        if (cont_datos == 7){
            largo_palabra = strlen(lectura);
            if(strcmp("Notiene", lectura) == 0) strcpy(evo_posterior, "--");
            else strcpy(evo_posterior, lectura);
        }
        if (cont_datos == 8) numero_pokedex = atoi(lectura);
        if (cont_datos == 9) strcpy(region, lectura);

        cont_datos++;
        if (cont_datos == 10){
            Pokemon *p = crearPokemon(id, nombre, tipos, PC, PS, sexo, evo_previa, evo_posterior, numero_pokedex, region);
            introducirMapas(p, pokemon_por_id);
            cont_datos = 0;
            total_tipos = 0;
        }
        lectura = (char *)malloc(sizeof(char) * 100);
    }
}

int main(){
    Map *pokemon_por_id = createMap(is_equal_int);
    setSortFunction(pokemon_por_id, lower_than_int);

    Map *pokemon_por_tipo = createMap(is_equal_string);
    setSortFunction(pokemon_por_tipo, lower_than_string);

    Map *pokemon_por_nombre = createMap(is_equal_string);
    setSortFunction(pokemon_por_nombre, lower_than_string);

    Map *pokemon_por_region = createMap(is_equal_string);
    setSortFunction(pokemon_por_region, lower_than_string);

    importarArchivo(pokemon_por_id);

    printf("Pokemon ordeandos por ID:\n");
    Pokemon *p = firstMap(pokemon_por_id);
    while (p){
        printf("%d - %s\n", p->id, p->nombre);
        p = nextMap(pokemon_por_id);
    }
}