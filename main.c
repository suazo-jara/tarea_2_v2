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

//Función que formatea las lecturas (elimina espacios y cambia a mayúsculas las iniciales)
char *formatear(char *lectura){
    int largo_palabra = strlen(lectura);
    int j = 0;
    for(int i = 0; i < largo_palabra; i++){
        if(lectura[i] == '-' || isalnum(lectura[i])){
            lectura[j] = lectura[i];
            j++;
        }
    }
    if (islower(lectura[0])) lectura[0] = toupper(lectura[0]);
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
void ingresar_en_mapas(Almacenamiento *almac, Pokedex *pokedex, Map *pokemon_por_id, Map *pokemon_por_tipo, Map *mapa_numero_pokedex){
    insertMap(pokemon_por_id, &almac->id, almac);
    insertMap(mapa_numero_pokedex, &pokedex->numero, pokedex);
}

//Función que agrega una existencia a un pokémon en la pokédex si se agrega otro de su mismo número
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

//Función que lee un archivo
void importarArchivo(Map *pokemon_por_id, Map *pokemon_por_tipo, Map *mapa_numero_pokedex, int *total_pokemon, int *copia_id)
{
    char *lectura = (char *)malloc(sizeof(char) * 100);
    char nombre_archivo[101];

    printf("Ingrese el nombre del archivo: ");
    scanf("%[^\n]s", nombre_archivo);

    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL){
        printf("No se encontro el archivo '%s'\n", nombre_archivo);
        return;
    }

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
            ingresar_en_mapas(almac, pokedex, pokemon_por_id, pokemon_por_tipo, mapa_numero_pokedex);
            cont_datos = 0;
            total_tipos = 0;
            existencias = 1;
            lista_tipos = create_list();
            *total_pokemon += 1;
        }
        lectura = (char *)malloc(sizeof(char) * 100);
    }
    fclose(archivo);
}

//Función que muestra las opciones del menu
void display_menu(){
    printf("------------ MENU ------------\n");
    printf("1. Importar desde un archivo .csv.\n");
    printf("2. Pokemon atrapado.\n");
    printf("3. Evolucionar pokemon.\n");
    printf("4. Buscar mis pokemon por tipo.\n");
    printf("5. Buscar mis pokemon por nombre.\n");
    printf("6. Buscar por nombre en pokedex.\n");
    printf("7. Mostrar todos los pokemon de la pokedex.\n");
    printf("8. Mostrar mis pokemon ordenados por PC.\n");
    printf("9. Liberar pokemon.\n");
    printf("10. Mostrar pokemon por region.\n");
    printf("11. Exportar a un archivo .csv.\n");
    printf("0. Salir.\n");

    printf("Escoja una opcion del menu: ");
}

//Funcion que agrega un pokémon al almacenamiento y su existencia en la pokédex
void agregar_pokemon(Map *pokemon_por_id, Map *pokemon_por_tipo, Map *mapa_numero_pokedex, int *total_pokemon, int *copia_id){
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
        printf("Ingrese el nombre del pokemon: ");
        scanf("%s", lectura);
        formatear(lectura);
        strcpy(nombre, formatear(lectura));
        lectura = (char *)malloc(sizeof(char) * 100);

        //Tipos
        printf("Ingrese el numero de tipos elementales: ");
        scanf("%i", &cantidad_tipos);
        for(int i = 0; i<cantidad_tipos; i++){
            printf("Ingrese el tipo %i : ", (i + 1));
            scanf("%s", lectura);
            push_back(lista_tipos, formatear(lectura));
            lectura = (char *)malloc(sizeof(char) * 100);
        }

        //PC
        printf("Ingrese el PC: ");
        scanf("%s", lectura);
        pc = atoi(lectura);
        lectura = (char *)malloc(sizeof(char) * 100);

        //PS
        printf("Ingrese el PS: ");
        scanf("%s", lectura);
        ps = atoi(lectura);
        lectura = (char *)malloc(sizeof(char) * 100);

        //Sexo
        printf("Ingrese el sexo (escriba hembra o macho): ");
        scanf("%s", lectura);
        formatear(lectura);
        if(strcmp(lectura, "Hembra") == 0){
            sexo = 'F';
        }else{
            sexo = 'M';
        }
        lectura = (char *)malloc(sizeof(char) * 100);

        //Evolucion previa
        printf("Ingrese la evolucion previa: ");
        scanf("%s", lectura);
        formatear(lectura);
        if(strcmp(lectura, "No") == 0){
            scanf("%s", lectura);
            strcpy(evolucionPrevia, "(--)");
        }else{
            strcpy(evolucionPrevia, formatear(lectura));
        }
        lectura = (char *)malloc(sizeof(char) * 100);

        //Evolucion Posterior
        printf("Ingrese la evolucion posterior: ");
        scanf("%s", lectura);
        formatear(lectura);
        if(strcmp(lectura, "No") == 0){
            strcpy(evolucionPosterior, "(--)");
            scanf("%s", lectura);
        }else{
            strcpy(evolucionPosterior, formatear(lectura));
        }
        lectura = (char *)malloc(sizeof(char) * 100);

        //Numero Pokedex
        printf("Ingrese el numero de la Pokedex: ");
        scanf("%s", lectura);
        numeroPokedex = atoi(lectura);
        buscar_en_pokedex(numeroPokedex, mapa_numero_pokedex);
        lectura = (char *)malloc(sizeof(char) * 100);
        //Region
        printf("Ingrese la region: ");
        scanf("%s", lectura);
        formatear(lectura);
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
        ingresar_en_mapas(almac, pokedex, pokemon_por_id, pokemon_por_tipo, mapa_numero_pokedex);
        printf("%s agregado exitosamente con ID %d en almacenamiento\n", nombre, id);
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

//Función que muestra pokémon del almacenamiento en orden de PC
void mostrarAlmacenamientoPC(Map *pokemon_id_almacenamiento){

    //Mapa que guarda a los pokémon por PC en orden de mayor a menor
    Map *pokemon_por_PC = createMap(is_unequal_int);
    setSortFunction(pokemon_por_PC, greater_than_int);

    Almacenamiento *pokemon = firstMap(pokemon_id_almacenamiento);
    while(pokemon){
        insertMap(pokemon_por_PC, &pokemon->PC, pokemon);
        pokemon = nextMap(pokemon_id_almacenamiento);
    }
    
    Almacenamiento *almac = firstMap(pokemon_por_PC);
    while(almac){
        printf("%d - %s - %d - %d - %c\n", almac->PC, almac->nombre, almac->id, almac->PS, almac->sexo);
        almac = nextMap(pokemon_por_PC);
    }
}

//Función que muestra a los pokémon en el almacenamiento dado su nombre
void pokemon_por_nombre(Map *pokemon_por_id){
    char nombre[101];
    printf("Ingrese el nombre que desea buscar\n");
    scanf("%s", nombre);
    formatear(nombre);
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
    char region[15];
    int cantidad = 0;
    Pokedex *p= firstMap(mapa_numero_pokedex);
    
    printf("Ingrese el nombre de la region a buscar\n");
    scanf("%s", region);
    formatear(region);
    printf("\n Region:%10s\n\n", region);
    printf(" Nombre               Ev. Prev        Ev. Post        Numero         Existencias          Tipo\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    
    while(p){

        if(strcmp(region, p->region) == 0){
            printf("|%-20.20s %-15s %-15s %-15d %-20d", p->nombre, p->evo_previa, p->evo_posterior, p->numero, p->existencias);
            List *iterador = first(p->tipos);
            while(iterador){
                printf("%s", iterador);
                iterador = next(p->tipos);
                if (iterador) printf(" / ");
            }
            printf("|\n");
            cantidad += p->existencias;
        }        
        p=nextMap(mapa_numero_pokedex);

    }
    printf("\nCantidad de pokemon en la region: %d\n\n", cantidad);

}

//Función que libera un pokémon del almacenamiento según su ID
void liberarPokemon(Map *pokemon_id_almacenamiento, Map *mapa_numero_pokedex){
    int id_almacenamiento;
    printf("Ingrese la ID del pokemon a liberar: ");
    scanf("%d", &id_almacenamiento);
    
    Almacenamiento *pokemon = searchMap(pokemon_id_almacenamiento, &id_almacenamiento);
    if(pokemon){
        Pokedex *en_pokedex = firstMap(mapa_numero_pokedex);
        while(en_pokedex){
            if (strcmp(en_pokedex->nombre, pokemon->nombre) == 0){
                en_pokedex->existencias--;
                break;
            }
            else (en_pokedex = nextMap(mapa_numero_pokedex));
        }
        eraseMap(pokemon_id_almacenamiento, &id_almacenamiento);
        printf("Un %s de ID %d ha sido liberado\n", en_pokedex->nombre, id_almacenamiento);
    }
    else{
        printf("No se encontro un pokemon de ID %d en el almacenamiento\n", id_almacenamiento);
    }
    return;
}

//Función que evoluciona a un pokémon según su ID
void evolucionarPokemon(Map *mapa_id_almacenamiento, Map *mapa_numero_pokedex){
    int id_almacenamiento;
    printf("Ingrese la ID del pokemon a evolucionar: ");
    scanf("%d", &id_almacenamiento);
    Almacenamiento *pokemon = searchMap(mapa_id_almacenamiento, &id_almacenamiento);
    
    if(pokemon){
        //Pokedex *en_pokedex = searchMap(mapa_numero_pokedex, pokemon->nombre);
        Pokedex *en_pokedex = firstMap(mapa_numero_pokedex);
        while(en_pokedex){
            if (strcmp(en_pokedex->nombre, pokemon->nombre) == 0) break;
            else (en_pokedex = nextMap(mapa_numero_pokedex)); 
        }
        
        if (strcmp(en_pokedex->evo_posterior, "(--)") == 0){
            printf("%s no tiene una evolucion posterior\n", en_pokedex->nombre);
            return;
        }
        else en_pokedex->existencias--;
        
        printf("El %s de ID %d evolucionara a %s\n", en_pokedex->nombre, id_almacenamiento, en_pokedex->evo_posterior);
        strcpy(pokemon->nombre, en_pokedex->evo_posterior);
        pokemon->PC *= 1.5;
        pokemon->PS *= 1.25;

        en_pokedex = firstMap(mapa_numero_pokedex);
        while(en_pokedex){
            if (strcmp(en_pokedex->nombre, pokemon->nombre) == 0){
                en_pokedex->existencias++;
                break;
            }
            else (en_pokedex = nextMap(mapa_numero_pokedex)); 
        }
        //printf("El %s de ID %d evoluciono a %s\n", en_pokedex->nombre, id_almacenamiento, pokemon->nombre);
    }
    else{
        printf("No se encontro un pokemon de ID %d en el almacenamiento\n", id_almacenamiento);
    }
    return;
}

//Funcion busca el nombre en pokedex y muestra su informacion
void mostrar_nombre_pokedex(Map *mapa_numero_pokedex){
    char nombre[101];
    printf("Ingrese el nombre que desea buscar en la pokedex\n");
    scanf("%s", nombre);
    formatear(nombre);

    Pokedex *iterador = firstMap(mapa_numero_pokedex);

    while(iterador){
        if(strcmp(iterador->nombre, nombre) == 0){
            printf("Nombre   /  existencia /   tipos     /      previa    /  posterior  /   numero pokedex /  region\n");
            printf("%s    -     %i    -        ", iterador->nombre, iterador->existencias);
            List *lista = first(iterador->tipos);
            while (lista){
                printf("%s", lista);
                lista = next(iterador->tipos);
                if (lista) printf(" / ");
            }
            printf("    %s    -   %s       -       %i    -     %s\n",  iterador->evo_previa, iterador->evo_posterior, iterador->numero, iterador->region);
            return;
        }
        iterador = nextMap(mapa_numero_pokedex);
    }
    //No se encontró el pokemon
        printf("----No se encuentra informacion del pokemon ingresado----\n");
}

//Función que muestra a los pokémon por tipo
void mostrar_por_tipo(Map *mapa_numero_pokedex, Map *mapa_numero_almacenamiento){
    //Mapa utilizado para buscar pokémon por tipo
    Map *mapa_tipos = createMap(is_equal_string);
    setSortFunction(mapa_tipos, lower_than_string);
    
    Almacenamiento *pokemon_almacenamiento;
    Pokedex *numero_pokedex;
    List *tipo_pokemon;
    List *iterador;

    char lectura[15];

    numero_pokedex = firstMap(mapa_numero_pokedex); //Iterador nivel 1: recorre mapa pokédex por número
    while(numero_pokedex){    
        tipo_pokemon = first(numero_pokedex->tipos); //Iterador nivel 2: recorre lista de tipos del pokémon
        while(tipo_pokemon){
            pokemon_almacenamiento = firstMap(mapa_numero_almacenamiento);
            while(pokemon_almacenamiento){
                if (strcmp(pokemon_almacenamiento->nombre, numero_pokedex->nombre) == 0){
                    iterador = searchMap(mapa_tipos, tipo_pokemon); //Iterador nivel 3: 
                    if (!iterador){
                        iterador = create_list();
                    }
                    push_back(iterador, pokemon_almacenamiento);
                    eraseMap(mapa_tipos, tipo_pokemon);
                    insertMap(mapa_tipos, tipo_pokemon, iterador);
                }
                pokemon_almacenamiento = nextMap(mapa_numero_almacenamiento);
            }
            tipo_pokemon = next(numero_pokedex->tipos);
        }
        numero_pokedex = nextMap(mapa_numero_pokedex);
    }

    printf("Ingrese el tipo elemental que desea buscar: ");
    scanf("%s", lectura);
    formatear(lectura);

    tipo_pokemon = searchMap(mapa_tipos, lectura);
    if (tipo_pokemon){
        pokemon_almacenamiento = first(tipo_pokemon);
        if(pokemon_almacenamiento){
            while(pokemon_almacenamiento){
                printf("%s - %i - %i - %i - %c\n", pokemon_almacenamiento->nombre, pokemon_almacenamiento->id, pokemon_almacenamiento->PC, pokemon_almacenamiento->PS, pokemon_almacenamiento->sexo);
                pokemon_almacenamiento = next(tipo_pokemon);
            }
        }
        else{
            printf("No hay existencia de pokemon tipo %s en el almacenamiento.\n", lectura);
        }
    }
    else printf("No existe este tipo de pokemon en la pokedex.\n");

}

//Función que exporta la lista de pokémon
void exportarArchivo(Map *pokemon_id_almacenamiento, Map *mapa_numero_pokedex){
    char *lectura = (char *)malloc(sizeof(char) * 100);
    char nombre_archivo[101];
    int verificar = 0;
    int primera_vez = 0;
    Pokedex *ficha_pokemon;
    Almacenamiento *pokemon;
    List *iterador_tipos;

    printf("Ingrese el nombre del archivo: ");
    scanf("%[^\n]s", nombre_archivo);

    FILE *archivo = fopen(nombre_archivo, "w");
    //if (archivo) printf("El archivo %s ya existe.\n", nombre_archivo);
    //else{
    fprintf(archivo, "id,nombre,tipos,pc,ps,sexo,evolucion previa,evolucion posterior,numero pokedex,region\n");

    //Mapa nombre pokédex
    Map *mapa_nombre_pokedex = createMap(is_equal_string);

    ficha_pokemon = firstMap(mapa_numero_pokedex);
    while(ficha_pokemon){
        insertMap(mapa_nombre_pokedex, ficha_pokemon->nombre, ficha_pokemon);
        ficha_pokemon = nextMap(mapa_numero_pokedex);
    }
    
    pokemon = firstMap(pokemon_id_almacenamiento);
    while(pokemon){
        ficha_pokemon = searchMap(mapa_nombre_pokedex, pokemon->nombre);
        fprintf(archivo, "%d,%s", pokemon->id, pokemon->nombre);

        iterador_tipos = first(ficha_pokemon->tipos);
        iterador_tipos = next(ficha_pokemon->tipos);
        verificar = 0;
        primera_vez = 0;
        //Verifica que exista uno tipo despues
        if(iterador_tipos != NULL){
            verificar = 1;
        }
        
        iterador_tipos = first(ficha_pokemon->tipos);
        while(iterador_tipos){
            if(verificar == 1){
                if(primera_vez == 0){
                    //Muestra comillas
                    fprintf(archivo, ",\"");
                    fprintf(archivo, "%s",iterador_tipos);
                    iterador_tipos = next(ficha_pokemon->tipos);
                    primera_vez = 1;
                }else{
                    fprintf(archivo, ", ");
                    fprintf(archivo, "%s",iterador_tipos);
                    iterador_tipos = next(ficha_pokemon->tipos);
                }
            }else{
                fprintf(archivo, ",%s",iterador_tipos);
                iterador_tipos = next(ficha_pokemon->tipos);
            }
        }

        if(verificar == 1){
            //Muestra comillas
            fprintf(archivo, "\"");
        }
        

        fprintf(archivo, ",%d,%d,", pokemon->PC, pokemon->PS);
        //Mostrar sexo
        if(pokemon->sexo == 'M'){
            fprintf(archivo, "Macho,");
        }else{
            fprintf(archivo, "Hembra,");
        }
        //Mostrar evolucion previa
        if(strcmp(ficha_pokemon->evo_previa,"(--)") == 0){
            fprintf(archivo, "No tiene,");
        }else{
            fprintf(archivo, "%s,", ficha_pokemon->evo_previa);
        }
        //Mostrar evolucion posterior
        if(strcmp(ficha_pokemon->evo_posterior,"(--)") == 0){
            fprintf(archivo, "No tiene,");
        }else{
            fprintf(archivo, "%s,", ficha_pokemon->evo_posterior);
        }
        fprintf(archivo, "%i,%s\n", ficha_pokemon->numero, ficha_pokemon->region);


        pokemon = nextMap(pokemon_id_almacenamiento);
    }

    fclose(archivo);
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
            importarArchivo(pokemon_id_almacenamiento, pokemon_por_tipo, mapa_numero_pokedex, &total_pokemon, &copia_id);
            break;
        case 2:
            agregar_pokemon(pokemon_id_almacenamiento, pokemon_por_tipo, mapa_numero_pokedex, &total_pokemon, &copia_id);
            break;
        case 3:
        //Evolucionar pokémon por ID
            evolucionarPokemon(pokemon_id_almacenamiento, mapa_numero_pokedex);
            break;
        case 4:
        //Mostrar pokémon por tipo
            mostrar_por_tipo(mapa_numero_pokedex, pokemon_id_almacenamiento);
            break;
        case 5:
        //Mostrar a los pokémon en el almacenamiento por el nombre que ingrese el usuario
            pokemon_por_nombre(pokemon_id_almacenamiento);
            break;
        case 6:
        //Mostrar a un pokémon por su nombre en la pokédex
            mostrar_nombre_pokedex(mapa_numero_pokedex);
            break;
        case 7:
        //Mostrar pokémon en pokédex por orden de número en pokédex    
            mostrar_pokemones_pokedex(mapa_numero_pokedex);
            break;
        case 8:
        //Mostrar pokémon en almacenamiento por PC
            mostrarAlmacenamientoPC(pokemon_id_almacenamiento);
            break;
        case 9:
        //Liberar pokemon por ID
            liberarPokemon(pokemon_id_almacenamiento, mapa_numero_pokedex);
            break;
        case 10:
        //Mostrar pokémon por región
            mostrar_pokemon_por_region(mapa_numero_pokedex);
            break;
        case 11:
            exportarArchivo(pokemon_id_almacenamiento, mapa_numero_pokedex);
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