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

////Función para comparar claves de tipo string. Retorna 0 si son iguales
int is_unequal_string(void * key1, void * key2) {
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

//Función para comparar claves de tipo int. Retorna 0 si son diferentes
int is_unequal_int(void * key1, void * key2) {
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

char *formatear(char *lectura){
    int largo_palabra = strlen(lectura);
    int j = 0;
    for(int i = 0; i < largo_palabra; i++){
        if(lectura[i] == '-' || isalnum(lectura[i])){
            lectura[j] = lectura[i];
            j++;
        }
    }
    lectura[j] = '\0';
    return lectura;
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
Pokedex *guardarPokedex(char *nombre, List *tipos, char *evo_previa, char *evo_posterior, int numero_pokedex, int existencias, char *region){
    Pokedex *p = (Pokedex*) malloc (sizeof(Pokedex));
    strcpy(p->nombre, nombre);
    p->tipos = tipos;
    strcpy(p->evo_previa, evo_previa);
    strcpy(p->evo_posterior, evo_posterior);
    p->numero = numero_pokedex;
    p->existencias = existencias;
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
    List * iterador = first(pokedex->tipos);
    while(iterador){
        insertMap(pokemon_por_tipo, iterador, pokedex);
        iterador = next(pokedex->tipos);
    }
    
    //printf("%d - %s - %d - %d - %c\n", almac->id, almac->nombre, almac->PC, almac->PS, almac->sexo);
    //printf("%s - %s - %s - %d - %s\n", pokedex->nombre, pokedex->evo_previa, pokedex->evo_posterior, pokedex->numero, pokedex->region);
}

void buscar_en_pokedex(int numero_pokedex, Map *mapa_numero_pokedex){
    Pokedex *pokedex = firstMap(mapa_numero_pokedex);
    while(pokedex){
        if (pokedex->numero == numero_pokedex){
            pokedex->existencias++;
            return;
        }
        pokedex = nextMap(mapa_numero_pokedex);
    }
    return;
}

//Función que lee el archivo
void importarArchivo(Map *pokemon_por_id, Map *pokemon_por_tipo, Map *nombre_almacenamiento, Map *nombre_pokedex, Map *mapa_numero_pokedex, Map *pokemon_por_PC, Map *pokemon_por_region, int *total_pokemon, int *copia_id)
{
    char *lectura = (char *)malloc(sizeof(char) * 100);
    char nombre_archivo[101];

    printf("Ingrese el nombre del archivo\n");
    scanf("%[^\n]s", nombre_archivo);

    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) exit(EXIT_FAILURE);

    int id, PC, PS, numero_pokedex, existencias = 1;
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
        if (cont_datos == 1 && !modo_tipo) strcpy(nombre, formatear(lectura));
        if ((modo_tipo && cont_datos >= 1) || (!modo_tipo && cont_datos == 2)){
            push_back(lista_tipos, formatear(lectura));
        }
        if (cont_datos == 3) PC = atoi(lectura);
        if (cont_datos == 4) PS = atoi(lectura);
        if (cont_datos == 5){
            if(strcmp("Hembra", lectura) == 0) sexo = 'F';
            else sexo = 'M';
        }
        if (cont_datos == 6){
            if(strcmp("No tiene", lectura) == 0) strcpy(evo_previa, "(--)");
            else strcpy(evo_previa, formatear(lectura));
        }
        if (cont_datos == 7){
            if(strcmp("No tiene", lectura) == 0) strcpy(evo_posterior, "(--)");
            else strcpy(evo_posterior, formatear(lectura));
        }
        if (cont_datos == 8){
            numero_pokedex = atoi(lectura);
            buscar_en_pokedex(numero_pokedex, mapa_numero_pokedex);
        }
        if (cont_datos == 9) strcpy(region, lectura);

        cont_datos++;
        if (cont_datos == 10){
            *copia_id = id;
            Almacenamiento *almac = guardarAlmacenamiento(id, nombre, PC, PS, sexo);
            Pokedex *pokedex = guardarPokedex(nombre, lista_tipos, evo_previa, evo_posterior, numero_pokedex, existencias, region);
            ingresar_en_mapas(almac, pokedex, pokemon_por_id, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region);
            cont_datos = 0;
            total_tipos = 0;
            existencias = 1;
            lista_tipos = create_list();
            *total_pokemon +=1;
        }
        lectura = (char *)malloc(sizeof(char) * 100);
    }
}

//Función que muestra las opciones del menu
void display_menu(){
    printf("Escoja una opcion del menu\n");
    printf("1. Importar/Exportar desde un archivo CSV:\n");
    printf("2. Pokemon atrapado:\n");
    printf("3. Evolucionar pokemon:\n");
    printf("4. Buscar mis pokemon por tipo:\n");
    printf("5. Buscar mis pokemon por nombre:\n");
    printf("6. Buscar por nombre en pokedex:\n");
    printf("7. Mostrar todos los pokemon de la pokedex:\n");
    printf("8. Mostrar mis pokemon ordenados por PC:\n");
    printf("9. Liberar pokemon:\n");
    printf("10. Mostrar pokemon por region:\n");
    printf("0. Salir\n");
}

//Funcion pasar a mayuscula la primera letra 
char *pasar_mayus_primera_letra(char *nombre){
    int cantidad = strlen(nombre);
    int i;
    for(i = 0;i<cantidad; i++){
        if(i == 0){
            nombre[0] = toupper(nombre[0]);
        }else{
            nombre[i] = towlower(nombre[i]);
        }
    }
    nombre[i] = '\0';
}

//Funcion agregar pokemon
void agregar_pokemon(Map *pokemon_por_id, Map *pokemon_por_tipo, Map *nombre_almacenamiento, Map *nombre_pokedex, Map *mapa_numero_pokedex, Map *pokemon_por_PC, Map *pokemon_por_region, int *total_pokemon, int *copia_id){
    char nombre[101];
    List *lista_tipos = create_list();
    int pc;
    int ps;
    char sexo;
    char evolucionPrevia[101];
    char evolucionPosterior[101];
    int numeroPokedex;
    char region[101];
    char *lectura = (char *)malloc(sizeof(char) * 100);
    int cantidad_tipos;
    int existencias = 1;
    int id;

    if(*total_pokemon<100){
        //Nombre
        printf("Ingrese el nombre del pokemon:");
        scanf("%s", lectura);
        pasar_mayus_primera_letra(lectura);
        strcpy(nombre, formatear(lectura));
        printf("\n");
        lectura = (char *)malloc(sizeof(char) * 100);

        //Tipos
        printf("Ingrese el numero de cuantos tipos tiene:");
        scanf("%i", &cantidad_tipos);
        for(int i = 0; i<cantidad_tipos; i++){
            printf("Ingrese el nombre del tipo %i :", (i + 1));
            scanf("%s", lectura);
            push_back(lista_tipos, formatear(lectura));
            printf("\n");
            lectura = (char *)malloc(sizeof(char) * 100);
        }

        //PC
        printf("Ingrese el PC:");
        scanf("%s", lectura);
        pc = atoi(lectura);
        printf("\n");
        lectura = (char *)malloc(sizeof(char) * 100);

        //PS
        printf("Ingrese el PS:");
        scanf("%s", lectura);
        ps = atoi(lectura);
        printf("\n");
        lectura = (char *)malloc(sizeof(char) * 100);

        //Sexo
        printf("Ingrese el sexo (escriba Hembra o Macho):");
        scanf("%s", lectura);
        pasar_mayus_primera_letra(lectura);
        if(strcmp(lectura, "Hembra") == 0){
            sexo = 'F';
        }else{
            sexo = 'M';
        }
        lectura = (char *)malloc(sizeof(char) * 100);
        printf("\n");

        //Evolucion previa
        printf("Ingrese la evolucion previa:");
        scanf("%s", lectura);
        pasar_mayus_primera_letra(lectura);
        if(strcmp(lectura, "No") == 0){
            scanf("%s", lectura);
            strcpy(evolucionPrevia, "(--)");
        }else{
            strcpy(evolucionPrevia, formatear(lectura));
        }
        lectura = (char *)malloc(sizeof(char) * 100);
        printf("\n");

        //Evolucion Posterior
        printf("Ingrese la evolucion posterior:");
        scanf("%s", lectura);
        pasar_mayus_primera_letra(lectura);
        if(strcmp(lectura, "No") == 0){
            strcpy(evolucionPosterior, "(--)");
            scanf("%s", lectura);
        }else{
            strcpy(evolucionPosterior, formatear(lectura));
        }
        lectura = (char *)malloc(sizeof(char) * 100);
        printf("\n");

        //Numero Pokedex
        printf("Ingrese el numero de la Pokedex:");
        scanf("%s", lectura);
        numeroPokedex = atoi(lectura);
        buscar_en_pokedex(numeroPokedex, mapa_numero_pokedex);
        lectura = (char *)malloc(sizeof(char) * 100);
        printf("\n");
        //Region
        printf("Ingrese la region:");
        scanf("%s", lectura);
        strcpy(region, lectura);
        lectura = (char *)malloc(sizeof(char) * 100);

        //id
        Almacenamiento *iterador = firstMap(pokemon_por_id);
        *copia_id+= 1;
        while (iterador)
        {
            if(*copia_id == iterador->id){
                *copia_id += 1;
            }
            iterador = nextMap(pokemon_por_id);
        }
        id = *copia_id;
        
        //Agregar pokemon
        Almacenamiento *almac = guardarAlmacenamiento(id, nombre, pc, ps, sexo);
        Pokedex *pokedex = guardarPokedex(nombre, lista_tipos, evolucionPrevia, evolucionPosterior, numeroPokedex, existencias, region);
        ingresar_en_mapas(almac, pokedex, pokemon_por_id, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region);
        printf("Pokemon agregado exitosamente\n");
    }else{
        printf("No se puede agregar pokemon, limite excedido\n");
    }
}

//Función que muestra los pokémon de la pokédex por numero en pokédex
void mostrar_pokemones_pokedex( Map *mapa_numero_pokedex){
    Pokedex *pokedex = firstMap(mapa_numero_pokedex);
    while(pokedex){
        printf("%d - %s - EN EXISTENCIA: %d - %s - %s - %s - ", pokedex->numero, pokedex->nombre, pokedex->existencias, pokedex->evo_previa, pokedex->evo_posterior, pokedex->region);
        List *iterador = first(pokedex->tipos);
        printf("TIPOS: ");
        while(iterador){
            printf("%s", iterador);
            iterador = next(pokedex->tipos);
            if (iterador) printf(" / ");
        }
        printf("\n");
        pokedex = nextMap(mapa_numero_pokedex);
    }
}

//Función que muestra pokémon del almacenamiento por PC
void mostrar_pokemones_almacenamiento(Map *pokemon_por_PC){
    Almacenamiento *almac = firstMap(pokemon_por_PC);
    while(almac){
        printf("%d - %s - %d - %d - %c\n", almac->PC, almac->nombre, almac->id, almac->PS, almac->sexo);
        almac = nextMap(pokemon_por_PC);
    }
}

void pokemon_por_nombre(Map *pokemon_por_id){
    char nombre[101];
    printf("Ingrese el nombre que desea buscar\n");
    scanf("%s", nombre);
    pasar_mayus_primera_letra(nombre);
    Almacenamiento *iterador = firstMap(pokemon_por_id);

    int cont = 0;

    while(iterador){
        if(strcmp(nombre, iterador->nombre) == 0){
            printf("%s - %i - %i - %i - %c\n", iterador->nombre, iterador->id , iterador->PC, iterador->PS, iterador->sexo);
            cont = 1;
        }
        iterador = nextMap(pokemon_por_id);
    }
    if(cont == 0){
        printf("No se encuentra un pokemon con ese nombre\n");
    }
}

//Función que muestra pokémon por región
void mostrar_pokemon_por_region(Map *mapa_numero_pokedex){
    char region[15];int cantidad=0;int b=0;
    Pokedex *p= firstMap(mapa_numero_pokedex);
    
    printf("Ingrese el nombre de la region a buscar\n");
    scanf("%s", region);
    pasar_mayus_primera_letra(region);
    printf("\n\n Region:%10s\n\n", region);
    printf(" Nombre               Ev. Prev        Ev. Post        Numero         Tipo\n");
    printf("---------------------------------------------------------------------------------------\n");
    
    while(p){

        if(strcmp(region, p->region)==0){
            
            printf("|%-20.20s %-15s %-15s %-15d",p->nombre,p->evo_previa,p->evo_posterior,p->numero);
            List *iterador = first(p->tipos);
            while(iterador){
                printf("%s", iterador);
                iterador = next(p->tipos);
                if (iterador) printf(" / ");
            }
            printf("|\n");
            cantidad += p->existencias;
        }
        //printf("|%-20.20s %-10s %-10s %10d|\n",p->nombre,p->evo_previa,p->evo_posterior,p->numero);// agregar tipos existencias
        
        p=nextMap(mapa_numero_pokedex);
        
    
    }
    printf("|Cantidad de pokemones en esta region: %d|\n", cantidad);

}//pokemones.csv

//Funcion busca el nombre en pokedex y muestra su informacion
void mostrar_nombre_pokedex(Map *mapa_numero_pokedex){
    char nombre[101];
    printf("Ingrese el nombre que desea buscar en la pokedex\n");
    scanf("%s", nombre);
    pasar_mayus_primera_letra(nombre);
    int cont = 0;

    Pokedex *iterador =firstMap(mapa_numero_pokedex);

    while(iterador){
        if(strcmp(iterador->nombre, nombre) == 0){
            if(cont == 0){
                printf("Nombre   /  existencia /   tipos     /      previa    /  posterior  /   numero pokedex /  region\n");
            }
            printf("%s    -     %i    -        ", iterador->nombre, iterador->existencias);
            List *lista = first(iterador->tipos);
            while (lista){
                printf("%s", lista);
                lista = next(iterador->tipos);
                if (lista) printf(" / ");
            }
            printf("    %s    -   %s       -       %i    -     %s\n",  iterador->evo_previa, iterador->evo_posterior, iterador->numero, iterador->region);
            
            cont = 1;
        }
        iterador = nextMap(mapa_numero_pokedex);
    }

    //No se encontro el pokemon
    if(cont == 0){
        printf("----No se encuentra informacion del pokemon ingresado----\n");
    }
}

int main(){
    //Mapa que guarda los pokémon en el almacenamiento con clave ID de pokémon
    Map *pokemon_id_almacenamiento = createMap(is_equal_int);
    setSortFunction(pokemon_id_almacenamiento, lower_than_int);

    //Mapa que guarda a los pokémon en la pokédex en orden y sin repetir con clave ID de pokédex
    Map *mapa_numero_pokedex = createMap(is_equal_int);
    setSortFunction(mapa_numero_pokedex, lower_than_int);

    //Mapa utilizado para buscar pokémon por tipo
    Map *pokemon_por_tipo = createMap(is_equal_string);
    setSortFunction(pokemon_por_tipo, lower_than_string);

    //Mapa utilizado para buscar pokémon por nombre
    Map *nombre_almacenamiento = createMap(is_equal_string);
    setSortFunction(nombre_almacenamiento, lower_than_string);

    //Mapa utilizado para buscar pokémon por nombre en pokédex
    Map *nombre_pokedex = createMap(is_equal_string);
    setSortFunction(nombre_pokedex, lower_than_string);

    //Mapa utilizado para mostrar a los pokémon del almacenamiento por PC
    Map *pokemon_por_PC = createMap(is_unequal_int);
    setSortFunction(pokemon_por_PC, greater_than_int);

    //Mapa utilizado para mostrar a los pokémon por región
    Map *pokemon_por_region = createMap(is_equal_string);
    setSortFunction(pokemon_por_region, lower_than_string);

    int escaneo;
    int total_pokemon = 0;
    int copia_id = 0;
    do{
        display_menu();
        scanf("%i", &escaneo);
        getchar();
        switch (escaneo)
        {
        case 1:
            importarArchivo(pokemon_id_almacenamiento, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region, &total_pokemon, &copia_id);
            break;
        case 2:
            agregar_pokemon(pokemon_id_almacenamiento, pokemon_por_tipo, nombre_almacenamiento, nombre_pokedex, mapa_numero_pokedex, pokemon_por_PC, pokemon_por_region, &total_pokemon, &copia_id);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
        //Mostrar a los pokémon por el nombre que ingrese el usuario
            pokemon_por_nombre(pokemon_id_almacenamiento);
            break;
        case 6:
        //Mostrar nombre en pokedex
            mostrar_nombre_pokedex(mapa_numero_pokedex);
            break;
        case 7:
        //Mostrar pokémon en pokédex por número en pokédex    
            mostrar_pokemones_pokedex(mapa_numero_pokedex);
            break;
        case 8:
        //Mostrar pokémon en almacenamiento por PC
            mostrar_pokemones_almacenamiento(pokemon_por_PC);
            break;
        case 9:
            break;
        case 10:
            mostrar_pokemon_por_region(mapa_numero_pokedex);
            break;
        case 0:
            return 0;
            break;
        default:
            printf("Ingrese una opcion valida\n");
            break;
        }
    }while(escaneo != 0);
    return 0;
}