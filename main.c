
/* version 2.0 -> Passou a todos os testes "oficiais" */
/* version 2.1 -> Corrigi alguns erros do 1o teste comunitario */
/* version 2.2 -> 2º ct: faltava um \n num print. Passei ao 3o. */
/* version 2.3 -> Corrigi o 2ºct */
/* version 2.4 -> Tentei otimizar usando nr_users / tasks / activ  ate linha 535*/
/* version 2.5 -> implementar mensagem "invalid duration" na new_task */

/* Tentar otimizar, fazendo nao percorrer tudo mas apenas < nr tasks */
/* Colocar os prints de erros em constantes */
/* Escrever uma descricao bonita */

/* File: main.c
 * Author: Pedro Chaparro ist199298
 * Description:
 *
 *  */


/* Segui a convencao que quando uma funcao retorna 1 e mau e 0 e bom
 * Segui a ideia de "o principal" serem as atividades, e dentro delas existirem
 * tarefas. */

/* =============================================================================
 * ===============                  INCLUDES                   =================
 * =============================================================================
*/

#include <stdio.h>
#include <string.h>


/* =============================================================================
 * ===============                   DEFINES                   =================
 * =============================================================================
*/

#define OCUPADO -1
#define VAZIO 0
#define TBD " "

/* Lista vazia para representar slots de utilizadores / atividades por definir*/
#define EMPTY_STR ""

#define SIM 1
#define NAO 0

#define PERTENCE 1
#define N_PERTENCE 0

#define DONE 1

#define MINUS 3

#define MAX_T 10000 /* Numero maximo de tarefas */
#define MAX_A 10    /* Numero maximo de atividades */
#define MAX_U 50    /* Numero maximo de utilizadores */

#define MAX_N_A 21  /* Numero de caracteres maximo do nome de uma atividade */
#define MAX_D_T 51  /* Numero de caracteres maximo da descricao de uma tarefa */
#define MAX_N_U 21  /* Numero de caracteres maximo do nome de um utilizador */

/* =============================================================================
 * ===============                   STRUCTS                   =================
 * =============================================================================
 */

/* Estrutura 'task'. Serve para armazenar toda a informacao relativa a tarefas*/
typedef struct {
    int id, dur, inst_init;
    char desc[MAX_D_T];
    char user[MAX_N_U];
    char ativ[MAX_N_A];
} task;


/* Estrutura 'activity'. Serve para armazenar todas as tarefas, por atividade.
 * Funciona em conjunto com a estrutura seguinte. */
typedef struct {
    char name[MAX_N_A];
    task tasks[MAX_T];
} activity;


/* Estrutura 'conjunto'. Serve para guardar a localizacao de cada tarefa,
 * associando o seu ID 'a sua Atividade atual.
 *
 * Se me for dado um ID, consigo verificar se o ID esta associado a alguma
 * tarefa (assumindo que ID = 0 significa não atribuido) e se sim, em qual
 * atividade se localiza. */
typedef struct {
    int id;
    char name[MAX_N_A];
} conjunto;

/* =============================================================================
 * ===============       CONSTANTES / VARIAVEIS GLOBAIS        =================
 * =============================================================================
*/


/* Tarefa Vazia. Usada para dar "reset" 'a posicao passada de uma tarefa quando
 * esta e' movida */
const task blank_task = {0};


/* Repositorio (Conjunto) de todos os IDs e nomes das atividades */
conjunto repo[MAX_T] = {0};


/* Numero de tarefas, atividades e utilizadores atuais. Usados para verificar
 * se o numero maximo e' ultrapassado. */
int nr_tasks = 0, nr_activ = 0, nr_users = 0;


int time = 0; /* O tempo simulado, iniciado a 0 */


activity activ_list[MAX_A]; /* Vetor que armazena as atividades */
int available_ids[MAX_T];  /* Vetor que armazena os IDs disponiveis */
char user_list[MAX_U][MAX_N_U]; /* Vetor que armazena os utilizadores */



/* =============================================================================
 * ===============                   FUNCOES                   =================
 * =============================================================================
*/


/* -----------------------------    Auxiliares   ---------------------------- */


/* Percorre uma lista de IDs e devolve o primeiro ID não ocupado. A posicao
 * lista e' atualizada com "OCUPADO" quando e' criada uma nova tarefa */
int find_lowest_id(int id_lst[]) {
    int i;
    for (i = 0; id_lst[i] == OCUPADO; i++) {}
    return i+1;
}


/* Verifica se um numero pertence a um vetor */
int belongs(int nr, int arr[MAX_T]) {

    int i = 0;
    while (arr[i] != 0 && i < MAX_T) {
        if (nr == arr[i]) {
            return PERTENCE;
        }
        i++;
    }
    return N_PERTENCE;
}

/* Verifica se todas as condicoes para mover uma tarefa se aplicam. */
int move_task_check(int id, char user[], char activ[]){

    int i, encontrou = NAO;

    /* Verifica se a atividade a mover e´ "TO DO" */
    if (strcmp(activ, "TO DO") == 0){
        printf("task already started\n");
        return 1;
    }

    /* Verifica se o utilizador existe, usando uma variável de estado */
    for (i = 0; i < nr_users; i++){
        if (strcmp(user_list[i], user) == 0){
            encontrou = SIM;
        }
    }
    if (encontrou == NAO){
        printf("no such user\n");
        return 1;
    }

    encontrou = NAO;
    /* Verifica se a atividade a mover existe, usando a mesma logica que na
     * verificacao de utilizadores*/
    for (i = 0; i < nr_activ; i++){
        if (strcmp(activ, activ_list[i].name) == 0){
            encontrou = SIM;
            break;
        }
    }
    if (encontrou == NAO){
        printf("no such activity\n");
        return 1;
    }

    /* Verifica se a task ja' foi criada, usando o ID associado no repositorio*/
    if (strcmp(repo[id-1].name, TBD) == 0){
        printf("no such task\n");
        return 1;
    }

    return 0;
}

/* ---------------------------    Principais   ------------------------------ */


/* Cria uma tarefa nova. Referente ao comando "t" */
int create_task(int dur, char descrip[]){

    task new_task;
    int i, j;
    /* Verifica o nr máximo de tarefas */
    if (nr_tasks == MAX_T){
        printf("too many tasks\n");
        return 1;
    }

    /* Verifica se a descricao e igual a alguma das tarefas existentes,
     * percorrendo as atividades e assumindo, no maximo, o maior numero de
     * tarefas ja criadas. */
    for (i = 0; i < nr_activ; i++){
        for (j = 0; j < nr_tasks; j++){
            if ( strcmp(activ_list[i].tasks[j].desc,descrip) == 0 ){
                printf("duplicate description\n");
                return 1;
            }
        }
    }

    if (dur <= 0){
        printf("invalid duration\n");
        return 1;
    }

    /* Criacao de uma nova tarefa */

    /* Encontra o menor id disponivel, atualiza os disponiveis e incrementa o
     * nr de tarefas */
    new_task.id = find_lowest_id(available_ids);

    available_ids[new_task.id - 1] = OCUPADO;

    nr_tasks++;

    /* Introduzo os parametros de tempo */
    new_task.dur = dur;
    new_task.inst_init = 0;

    /* Guarda a descricao e o nome da atividade TO DO */
    strcpy(new_task.desc, descrip);
    strcpy(new_task.ativ, "TO DO");

    /*O User fica por inicializar ate a task ser movida */

    /* Guarda a nova task na 1a atividade (TO DO).
     * Para isso, acedo a primeira atividade, as tarefas, e a tarefa de indice
     * ID - 1 */
    activ_list[0].tasks[new_task.id-1] = new_task;


    /* Guarda o conjunto no repositorio */
    repo[new_task.id-1].id = new_task.id;
    strcpy(repo[new_task.id-1].name, activ_list[0].name);

    /* Imprimo a mensagem e retorno */
    printf("task %d\n", new_task.id);
    return 0;
}


/* Lista a tarefa com o id correspondente. Referente ao comando "l". */
int list_task(int id){
    int i;
    char activ_atual[MAX_N_A];

    /* Encontra a atividade associada a task do ID fornecido, verificando se
     * a tarefa esta definida primeiro.
     * Verifica se o id esta no intervalo correto e se a atividade esta definida
     * usando o repositorio. */
    if ((0 < id && id <= MAX_T) && (strcmp(repo[id-1].name, TBD) == 0)){
        printf("%d: no such task\n", id);
        return 1;
    }

    /* Copia a atividade atual */
    strcpy(activ_atual, repo[id-1].name);
    /* Percorre todas as atividades ate chegar a atual*/
    for (i = 0; i < nr_activ; i++){
        if ( strcmp(activ_list[i].name, activ_atual) == 0){

            /*Verifica se a tarefa esta definida. Se sim, imprime a informacao*/

            if (activ_list[i].tasks[id-1].id != VAZIO) {
                printf("%d %s #%d %s\n",
                       id,
                       activ_atual,
                       activ_list[i].tasks[id - 1].dur,
                       activ_list[i].tasks[id - 1].desc);
                return 0;
            }
        }
    }
    /*  Caso a tarefa nao estar definida */
    printf("%d: no such task\n", id);
    return 1;
}


/* Lista todas as tarefas. Referente ao comando "l", sem parâmetros. */
int list_all_tasks(){

    /*  k          -> indices do vetor all_tasks, que armazena todas as tarefas
     *                definidas;
     *  c_task     -> representa a tarefa atual (mais legível);
     *  menor      -> representa a tarefa com menor descricao, alfabeticamente;
     *  sorted_ids -> representam os ids ordenados que seram usados para listar
     *                todas as tarefas;
     *  all_tasks  -> vetor com todas as tarefas definidas. */

    int i, j, k = 0, sorted_ids[MAX_T] = {0};
    task all_tasks[MAX_T] = {0}, menor;
    task c_task;

    /* Percorro todas as atividades definidas e adiciono as tarefas definidas */
    for (i = 0; i < nr_activ; i++){
        /* Verifico se a atividade esta definida */
        if (strcmp(activ_list[i].name, EMPTY_STR) != 0){

            /* Percorro todas as tarefas */
            for (j = 0; j < nr_tasks; j++){
                /* Adiciono as tarefas definidas ao vetor */

                if (activ_list[i].tasks[j].id != VAZIO){

                    all_tasks[k] = activ_list[i].tasks[j];
                    k++;

                }
            }
        }
    }
    /* Acabados os ciclos, o vetor "all_tasks" tem todas as tarefas definidas.*/
    /* Agora, ordenam-se os IDs consoante a ordem das descricoes. */

    /* Ciclo exterior. Serve para colocar no vetor "sorted_ids". */
    for (i = 0; i < nr_tasks; i++){

        /* Procuro a primeira tarefa que nao pertenca ja aos ids ordenados e
         * escolho-a como menor. */
        for (j = 0; j < nr_tasks; j++){
            c_task = all_tasks[j];
            if (belongs(c_task.id, sorted_ids) == N_PERTENCE){
                menor = c_task;
                break;
            }
        }

        /* Tendo uma menor, percorro agora todas as tarefas ate ao fim.  */
        for (j = 0; j < nr_tasks; j++){
            /* Para ser mais legivel, "renomeio" a tarefa atual */
            c_task = all_tasks[j];

            /* Se alguma tiver a descricao menor que a atual menor, substituo
             * caso nao esta ja nos ids ordenados */
            if ((strcmp(c_task.desc, menor.desc) < 0) &&
                (belongs(c_task.id, sorted_ids) == N_PERTENCE)){
                menor = c_task;
            }
        }
        /* Chegado ao fim, atualizo os ids_ordenados usando "i" como indice */
        sorted_ids[i] = menor.id;
    }

    /* Tendo o vetor com os ids por ordem de descricao, chamo a funcao de
     * listagem individual para cada entrada do vetor */
    for (i = 0; i < nr_tasks; i++){
        list_task(sorted_ids[i]);
    }

    return 0;
}


/* Atualiza o tempo. Referente ao comando "n". */
int time_skip(int n) {

    if (n < 0) {
        printf("invalid time\n");
        return 1;
    }

    time += n;
    printf("%d\n", time);
    return 0;

}


/* Cria um novo utilizador. Referente ao comando "u". */
int create_user(char name[]){
    int i;

    /* Verifica se excedeu o numero de utilizadores */
    if (nr_users == MAX_U){
        printf("too many users\n");
        return 1;
    }

    /* Verifica se o utilizador ja existe, percorrendo um ciclo enquanto
     * existirem utilizadores definidos (ou chegar ao maximo de utilizadores) */
    for (i = 0; strcmp(user_list[i], EMPTY_STR) != 0 && i < MAX_U; i++){
        if ( strcmp(user_list[i], name) == 0){
            printf("user already exists\n");
            return 1;
        }
    }

    /* Caso nao exista, preenche o primeiro slot nao ocupado. Reutilizo o indice
     * "i" do ciclo anterior. */
    strcpy(user_list[i], name);
    nr_users++; /* Incremento o numero total de utilizadores */
    return 0;
}

/* Lista todos os utilizadores. Referente ao comando "u", sem parâmetros. */
int list_all_users(){
    int i;
    for (i = 0; strcmp(user_list[i], EMPTY_STR) != 0 && i < MAX_U; i++) {
        printf("%s\n", user_list[i]);
    }

    return 0;
}


/* Move uma tarefa entre atividades. Referente ao comando "m".
 * Dividi em duas funcoes, onde a primeira verifica todos os possiveis erros. */
int move_task(int id, char user[], char activ[]){
    /* destino -> guarda a atividade de destino (DONE ou outra) */
    int i, ind, duration, slack, destino = !DONE;

    /* Verifica se existe algum erro. Nota: Se uma funcao der erro, retorna
     * sempre 1. */
    if (move_task_check(id, user, activ) == 1){
        return 1;
    }
    /* Procuro o indice da lista de atividades da atividade de destino.
     * Caso o destino seja DONE, guardo numa variável. */
    for (ind = 0; ind < nr_activ; ind++) {
        if (strcmp(activ, activ_list[ind].name) == 0) {

            if (strcmp(activ_list[ind].name, "DONE") == 0){
                destino = DONE;
            }
            break;
        }
    }


    /* Atualizo a tarefa:
     * -> Primeiro encontro a sua localizacao, comparando o nome do repositorio
     *    com os nomes das atividades definidas;
     * -> Ao descobrir a atividade, acedo 'a tarefa e altero a sua atividade
     *    atual. Faco o mesmo para o repositorio.
     * -> Caso o destino seja DONE, faco os calculos necessarios. */

    for (i = 0; i < nr_activ; i++){

        if (strcmp(repo[id-1].name, activ_list[i].name) == 0){

            /* Verifico se a atividade de origem foi a "TO DO". Se sim, entao
             * atualizo o instante inicial */

            if (strcmp(activ_list[i].tasks[id-1].ativ, "TO DO") == 0){
                activ_list[i].tasks[id-1].inst_init = time;
            }

            /* Atualizo a atividade de destino usando o indice encontrado
             * anteriormente, substituindo a tarefa */
            activ_list[ind].tasks[id-1] = activ_list[i].tasks[id-1];

            /* Atualizo os nomes da atividade e do utilizador da tarefa
             * e o repositorio */
            strcpy(activ_list[ind].tasks[id-1].ativ, activ_list[ind].name);
            strcpy(activ_list[ind].tasks[id-1].user, user);

            strcpy(repo[id-1].name, activ_list[ind].name);

            /* "Limpo" a tarefa da atividade de origem e defino o ID como
             * vazio de forma a fazer as comparações */
            activ_list[i].tasks[id-1] = blank_task;
            activ_list[i].tasks[id-1].id = VAZIO;
            /* No caso do destino ser DONE, processo o formato de saida */
            if (destino == DONE) {
                duration = time - activ_list[ind].tasks[id - 1].inst_init;
                slack = duration - activ_list[ind].tasks[id - 1].dur;

                printf("duration=%d slack=%d\n", duration, slack);
            }
            /* No caso de ter entrado no ciclo, faz return aqui */
            return 0;
        }
    }
    return 0;
}


/* Lista todas as tarefas de uma atividade. Referente ao comando "d". */
int list_activity_tasks(char activ[]) {

    /* menor     -> tarefa com o menor instante de inicio e com a menor descricao
     *              alfabeticamente.
     * c_task    -> tarefa atual. (Tornar mais legivel)
     * blacklist -> vetor que guarda as tarefas ja impressas, de forma a nao
     *              existirem repetidas. */

    int ind, i, j, cont = 0;
    task menor, c_task;
    int blacklist[MAX_T] = {0};

    /* Verifico se a atividade existe */

    for (i = 0; i < nr_activ; i++){
        /* Se encontrar alguma atividade com o nome do input, para o ciclo */
        if (strcmp(activ, activ_list[i].name) == 0){
            break;
        }
    }
    /* Se chegou ao fim e nao encontrou, entao a atividade nao existe */
    if (i == nr_activ){
        printf("no such activity\n");
        return 1;
    }

    /* Procuro pelo indice da atividade de destino */
    for (ind = 0; ind < nr_activ; ind++) {
        if (strcmp(activ, activ_list[ind].name) == 0) {
            break;
        }
    }

    /* Conto o numero de tarefas definidas */
    for (i = 0; i < MAX_T; i++) {
        if (activ_list[ind].tasks[i].id != VAZIO)
            cont++;
    }

    /* Percorro todas as tarefas, "nr_tasks" vezes, de forma a encontrar a
     * menor em cada iteracao. */
    for (i = 0; i < cont; i++) {

        /* Em cada nova iteracao, considero a primeira tarefa definida. */
        for (j = 0; j < MAX_T; j++) {
            if ((activ_list[ind].tasks[j].id != VAZIO) &&
                (belongs(activ_list[ind].tasks[j].id, blacklist) == N_PERTENCE))
            {
                menor = activ_list[ind].tasks[j];
                break;
            }
        }

        /* Percorro entao toda a lista e faco sucessivas comparacoes ate
         * encontrar a menor tarefa */
        for (j = 0; j < MAX_T; j++) {
            c_task = activ_list[ind].tasks[j]; /* Mais legivel */

            /* Primeiro verifico se a tarefa existe e nao esta na blacklist,
             * usando a funcao "belongs". */
            if ( (c_task.id != VAZIO) &&
                (belongs(c_task.id, blacklist) == N_PERTENCE) ){

                /*Verifico se o instante inicial e menor que o da tarefa
                 * considerada menor. Se for, atualizado a tarefa menor. */
                if (activ_list[ind].tasks[j].inst_init < menor.inst_init) {
                    menor = activ_list[ind].tasks[j];
                }

                /* A outra possibilidade de mudanca acontece se os instantes
                 * forem iguais. Nesse caso, comparo as descricoes. */
                else if (activ_list[ind].tasks[j].inst_init ==
                         menor.inst_init) {

                    /* Caso a primeira seja alfabeticamente menor, substituo. */
                    if (strcmp(activ_list[ind].tasks[j].desc, menor.desc) < 0) {
                        menor = activ_list[ind].tasks[j];
                    }
                }
            }
        }
        /* Imprimo a menor tarefa e coloco o seu id na blacklist */
        printf("%d %d %s\n", menor.id, menor.inst_init, menor.desc);
        blacklist[i] = menor.id;
    }
    return 0;
}


/* Adiciona uma atividade. Referente ao comando "a". */
int add_activity(char activ_desc[]){

    int i, j;

    /* Verifica se o total de atividades foi ultrapassado. */
    if (nr_activ == MAX_A){
        printf("too many activities\n");
        return 1;
    }

    /* Verifica se cada letra da descricao nao e maiuscula, um espaco ou uma
     * tabulacao. */
    for (i = 0; i < MAX_N_A-1 && activ_desc[i] != '\0'; i++){

        if ( !((activ_desc[i] >= 'A' && activ_desc[i] <= 'Z') ||
               (activ_desc[i] >= '0' && activ_desc[i] <= '9') ||
                activ_desc[i] == ' ' ||
                activ_desc[i] == '\t') ){
            printf("invalid description\n");
            return 1;
        }
    }

    /* Verifica atividades repetidas. */
    for (i = 0; i < MAX_A; i++){
        if ( strcmp(activ_desc, activ_list[i].name) == 0) {
            printf("duplicate activity\n");
            return 1;
        }
    }

    /* Procura um slot de atividades disponivel e atualiza-o com a nova. */
    for (i = 0; i < MAX_A; i++ ){

        if (strcmp(activ_list[i].name, EMPTY_STR) == 0){
            strcpy(activ_list[i].name, activ_desc);

            /* Faz reset a todas as tasks da nova atividade */
            for (j = 0; j < MAX_T; j++){
                activ_list[i].tasks[j] = blank_task;
            }
            nr_activ++;
            return 0;
        }
    }

    return 0;
}

/* Lista todas as atividades. Referente ao comando "a", sem parametros. */
int list_all_activities(){
    int i;

    /* Imprime as atividades ate encontrar uma nao definida ou chegar ao nr
     * maximo de atividades */
    for (i = 0; strcmp(activ_list[i].name, EMPTY_STR) != 0 && i < MAX_A; i++){
        printf("%s\n", activ_list[i].name);
    }

    return 0;
}



/* =============================================================================
 * ===============                    MAIN                     =================
 * =============================================================================
*/

int main() {

    /* id  -  -  -  -  -  -   -> Usada para definir um vetor com todos os ids.
     * temp1, temp2, tempInt1 -> Variaveis temporarias "multi-funcoes",
     *                           sao usadas em varios casos do Switch.
     * z, c  -  -  -  -  -  - -> Usadas para ler caracteres, tambem
     *                           "multi-funcoes".
     * num  -  -  -  -  -  -  -> Acumula um numero ao ler do terminal
     * sign -  -  -  -  -  -  -> Indica o sinal do numero do tempo;
     */

    int i,j, id, cont = 0, sign = !MINUS;
    int tempInt1;
    char c, temp1[MAX_T],temp2[MAX_T], z;
    int num = 0;

    /* Definir todos os possiveis IDs. */
    for (id = 0; id < MAX_T; id++){
        available_ids[id] = id+1;
    }

    /* Inicializa a lista de utilizadores. */
    for (i = 0; i < MAX_U; i++){
        strcpy(user_list[i], EMPTY_STR);
    }

    /* Inicializa a lista de atividades e as respetivas tarefas. */

    for (i = 0; i < MAX_A; i++){
        strcpy(activ_list[i].name, EMPTY_STR);

        for(j = 0; j < MAX_T; j++){
            activ_list[i].tasks[j] = blank_task;
        }
    }

    /* Inicializa o repositorio. */

    for (id = 1; id <= MAX_T; id++){
        strcpy(repo[id-1].name, TBD);
    }

    /* Altera o nome das 3 atividades pre-definidas */
    strcpy(activ_list[0].name, "TO DO");
    strcpy(activ_list[1].name, "IN PROGRESS");
    strcpy(activ_list[2].name, "DONE");
    nr_activ = 3;


    while (1){
        switch (c = getchar()) {

            /* Acaba o programa */
            case 'q':
                return 0;
                break;

            /* Nova Tarefa */
            case 't':
                /* Guarda a duracao e "salta" o espaco */
                scanf("%d", &tempInt1);
                getchar();

                /* Guarda a descricao. */
                i = 0;
                while (( (z = getchar()) != '\n') && (i < MAX_D_T-1)){
                    temp1[i] = z;
                    i++;
                }
                temp1[i] = '\0';
                create_task(tempInt1, temp1);
                break;

            /* Lista Tarefas */
            /* variavel 'cont' usada para contar o numero de listagens. Apenas
             * importante se for != 0 */
            case 'l':
                /* Se o primeiro caracter for '\n', lista todas as tarefas
                 * automaticamente */
                z = getchar();

                if (z == '\n'){
                    list_all_tasks();
                    cont++;
                    break;
                }
                else {
                    /* Vai acumulando os numeros e fazendo a listagem apos
                     * encontrar um espaco. */
                    while ((z = getchar()) != '\n') {
                        if ('0' <= z && z <= '9') {
                            num = (num * 10) + (z - '0');
                            cont++;
                        }
                        else if ((z == ' ') && (cont != 0)) {
                            list_task(num);
                            num = 0;
                        }
                    }
                }
                /* Lista o ultimo numero */
                if ((cont != 0) && (num != 0)){
                    list_task(num);
                }
                /* Caso em que foram inseridos varios espacos e '\n'.
                 * Exemplo "l         '\n'"*/
                if (cont == 0) {
                    list_all_tasks();
                }
                break;


            case 'n':

                num = 0;
                while ((z = getchar()) != '\n') {
                    if ('0' <= z && z <= '9') {
                        num = (num * 10) + (z - '0');
                    }
                    if (z == '-'){
                        sign = MINUS;
                    }

                }

                if (sign == MINUS){
                    num = -num;
                }

                time_skip(num);
                break;

            case 'u':
                z = getchar();
                if (z == '\n'){
                    list_all_users();
                    break;
                }
                cont = 0;
                i = 0;
                z = getchar();

                if ( z == ' '){
                    cont++;
                }
                while ((z != '\n') && (z != ' ') && (i < MAX_N_U-1)){
                    temp1[i] = z;
                    i++;
                    z = getchar();
                }
                if ( z == ' '){
                    cont++;
                }

                temp1[i] = '\0';
                /* Apenas cria um user se nao apareceu um espaço em branco */
                if (cont == 0) {
                    create_user(temp1);
                }
                break;

            /* Move uma Tarefa */
            case 'm':
                /* Guarda o id e o utilizador */
                scanf("%d%s", &tempInt1, temp1);

                /* Obtem o nome da atividade de destino */
                i = 0;
                getchar();
                while (( (z = getchar()) != '\n') && (i < MAX_N_A-1)){
                    temp2[i] = z;
                    i++;
                }
                temp2[i] = '\0';
                /* Chama a funcao */
                move_task(tempInt1, temp1, temp2);
                break;


            /* Lista atividades de uma Tarefa */
            case 'd':
                /* Obtem o nome da atividade */
                i = 0;
                getchar();
                while (( (z = getchar()) != '\n') && (i < MAX_N_A-1)){
                    temp1[i] = z;
                    i++;
                }
                temp1[i] = '\0';
                list_activity_tasks(temp1);
                break;


            /* Adiciona uma atividade / Lista todas */
            case 'a':
                /* Se nao existirem argumentos, lista todas as atividades. */
                z = getchar();
                if (z == '\n'){
                    list_all_activities();
                    break;
                }
                /* Obtem o nome da atividade */
                cont = 0;
                i = 0;
                z = getchar();

                while ((z != '\n') && (i < MAX_N_A-1)){
                    temp1[i] = z;
                    z = getchar();
                    i++;
                }
                temp1[i] = '\0';
                add_activity(temp1);
                break;

        }

        /* Da reset a todas as variaveis utilizadas */
        for (j = 0; j <= i; j++){
            temp1[j] = ' ';
            temp2[j] = ' ';
        }
        num = cont = i = 0;
        sign = !MINUS;
    }

    printf("Hello, World!\n");
    return 0;
}


