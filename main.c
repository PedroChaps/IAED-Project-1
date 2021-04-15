
/* version 2.0 -> Passou a todos os testes "oficiais" */
/* version 2.1 -> Corrigi alguns erros do 1o teste comunitario */
/* version 2.2 -> 2º ct: faltava um \n num print. Passei ao 3o. */
/* version 2.3 -> Corrigi o 2ºct */
/* version 2.4 -> Tentei otimizar usando nr_users / tasks / activ  ate linha 535*/
/* version 2.5 -> implementar mensagem "invalid duration" na new_task */

/* version 3.0 -> Tasks Revamp. Em termos de bugs, consegui fazer t10.in  */
/* version 3.1 -> Defines bonitos, espaços */
/* version 3.2 -> Mais funções para reduzir número de linhas */

/* version 4.0 -> PRÉ Mudar os sorts. Tornar mais eficiente. */
/* version 4.1 -> Sorts mudados. Fundido em 1 só. Passei a todos os testes até ct005, exceto ct005. */

/* version 5.1 -> Mooshak season. Apenas tenho o ct004 do Diogo por corrigir. Do mooshak tenho 5 errados */


/* File: proj1.c
 * Author: Pedro Chaparro ist199298
 * Description: Projeto 1 de IAED: Criação de um programa que simula o método
 *              kanban. A estrutura geral basea-se na estrutura tarefa, composta
 *              por vários elementos do método kanban (duração, atividade, ...)
 *              e um num vetor de tarefas.
 *              Estas tarefas e atividades serão manipuladas consoante o input
 *              do utilizador ao interagir com o terminal.
 *              Foi usada a convenção que uma função retorna 0 quando não
 *              ocorrem erros e 1 caso contrário.
 *  */


/* Err34: Mover para a mesma task ;
 * ErrT51-T54 Minusculas do "a":
 *
 *
 *
 *
 *
 *
 *  */




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

/* Lista vazia para representar descrições, utilizadores e atividades por
 * definir*/
#define EMPTY_STR ""

#define SIM 1
#define NAO 0

#define PERTENCE 1
#define N_PERTENCE 0

#define DONE 1

#define MAX_T 10000 /* Número máximo de tarefas */
#define MAX_A 10    /* Número máximo de atividades */
#define MAX_U 50    /* Número máximo de utilizadores */

#define MAX_N_A 21  /* Número de caracteres máximo do nome de uma atividade */
#define MAX_D_T 51  /* Número de caracteres máximo da descrição de uma tarefa */
#define MAX_N_U 21  /* Número de caracteres máximo do nome de um utilizador */

/* Erros das diversas funções */

#define ERROR1 "task already started\n"
#define ERROR2 "no such user\n"
#define ERROR3 "no such activity\n"
#define ERROR4 "no such task\n"
#define ERROR5 "too many tasks\n"
#define ERROR6 "duplicate description\n"
#define ERROR7 "invalid duration\n"
#define ERROR8 "%d: no such task\n"
#define ERROR9 "invalid time\n"
#define ERROR10 "too many users\n"
#define ERROR11 "user already exists\n"
#define ERROR12 "too many activities\n"
#define ERROR13 "invalid description\n"
#define ERROR14 "duplicate activity\n"

/* Atividades pré-definidas */
#define A_START "TO DO"
#define A_MIDDLE "IN PROGRESS"
#define A_LAST "DONE"



/* =============================================================================
 * ===============                   STRUCT                    =================
 * =============================================================================
 */

/* Estrutura 'task'. Serve para armazenar toda a informação relativa a tarefas*/
typedef struct {
    int id, dur, inst_init;
    char desc[MAX_D_T];
    char user[MAX_N_U];
    char ativ[MAX_N_A];
} task;


/* =============================================================================
 * ===============       CONSTANTES / VARIÁVEIS GLOBAIS        =================
 * =============================================================================
*/

/* Número de tarefas, atividades e utilizadores atuais. Usados para verificar
 * se o número máximo de cada elemento é ultrapassado e em outras condições. */
int nr_tasks = 0, nr_activ = 0, nr_users = 0;

int time = 0; /* O tempo simulado, íniciado a 0. */

task task_list[MAX_T];           /* Vetor que armazena as tarefas       */
char activ_list[MAX_A][MAX_N_A]; /* Vetor que armazena as atividades    */
char user_list[MAX_U][MAX_N_U];  /* Vetor que armazena os utilizadores  */

/* Vetor que armazena os ids ordenados alfabeticamente */
int sorted_ids_a[MAX_T] = {0};
/*Vetor que armazena os ids ordenados por instante inicial (e alfabeticamente)*/
int sorted_ids_t[MAX_T] = {0};

/* Tarefa Vazia. Usada para inicializar o vetor de tarefas. */
const task blank_task = {0};

/* =============================================================================
 * ===============                   FUNÇÕES                   =================
 * =============================================================================
*/


/* -----------------------------    Auxiliares   ---------------------------- */


/* Verifica se um número pertence a um vetor */
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


/*                    Verificações de Erros / Inicializações                  */

/* Verifica se existe algum erro na criação de uma tarefa nova */
int create_task_check(int dur, char descrip[]) {

    int i;

    /* Verifica o número máximo de tarefas */
    if (nr_tasks == MAX_T) {
        printf(ERROR5);
        return 1;
    }

    /* Verifico se a descrição é igual a alguma das tarefas existentes */
    for (i = 0; i < nr_tasks; i++) {
        if (strcmp(task_list[i].desc, descrip) == 0) {
            printf(ERROR6);
            return 1;
        }
    }

    /* Verifico se a duração é válida */
    if (dur <= 0) {
        printf(ERROR7);
        return 1;
    }

    return 0;
}

/* Verifica se todas as condições para mover uma tarefa se aplicam. */
int move_task_check(int id, char user[], char activ[]){

    int i, encontrou = NAO;

    /* Verifica se a task já foi criada */
    if (strcmp(task_list[id-1].desc, EMPTY_STR) == 0){
        printf(ERROR4);
        return 1;
    }

    /* Verifica se a atividade a mover e´ a mesma */
    if (strcmp(activ, task_list[id-1].ativ) == 0){
        return 1;
    }

    /* Verifica se a atividade a mover e´ "TO DO" */
    if (strcmp(activ, "TO DO") == 0){
        printf(ERROR1);
        return 1;
    }

    /* Verifica se o utilizador existe, usando uma variável de estado */
    for (i = 0; i < nr_users; i++){
        if (strcmp(user_list[i], user) == 0){
            encontrou = SIM;
        }
    }
    if (encontrou == NAO){
        printf(ERROR2);
        return 1;
    }

    encontrou = NAO;
    /* Verifica se a atividade a mover existe, usando a mesma logica que na
     * verificação de utilizadores*/
    for (i = 0; i < nr_activ; i++){
        if (strcmp(activ, activ_list[i]) == 0){
            encontrou = SIM;
            break;
        }
    }
    if (encontrou == NAO){
        printf(ERROR3);
        return 1;
    }


    return 0;
}

int add_activity_check(char activ_desc[]){
    int i;

    /* Verifica atividades repetidas. */
    for (i = 0; i < nr_activ; i++){
        if ( strcmp(activ_desc, activ_list[i]) == 0) {
            printf(ERROR14);
            return 1;
        }
    }

    /* Verifica se cada letra da descricao nao e maiuscula, um espaco ou uma
     * tabulacao. */
    for (i = 0; i < MAX_N_A-1 && activ_desc[i] != '\0'; i++){

        if (activ_desc[i] >= 'a' && activ_desc[i] <= 'z' ){
            printf(ERROR13);
            return 1;
        }
    }

    /*Coise dentro do if /\
     * !((activ_desc[i] >= 'A' && activ_desc[i] <= 'Z') ||
               (activ_desc[i] >= '0' && activ_desc[i] <= '9') ||
               activ_desc[i] == ' ' ||
               activ_desc[i] == '\t')*/



    /* Verifica se o total de atividades foi ultrapassado. */
    if (nr_activ == MAX_A){
        printf(ERROR12);
        return 1;
    }




    return 0;
}

int initialize_arrays(){

    int i;

    /* Inicializa a lista de utilizadores. */
    for (i = 0; i < MAX_U; i++){
        strcpy(user_list[i], EMPTY_STR);
    }

    /* Inicializa a lista de atividades. */
    for (i = 0; i < MAX_A; i++){
        strcpy(activ_list[i], EMPTY_STR);
    }

    /* Inicializa as tarefas. */
    for (i = 0; i < MAX_T; i++){
        task_list[i] = blank_task;
        strcpy(task_list[i].desc, EMPTY_STR);
    }

    /* Altera o nome das 3 atividades pre-definidas */
    strcpy(activ_list[0], A_START);
    strcpy(activ_list[1], A_MIDDLE);
    strcpy(activ_list[2], A_LAST);
    nr_activ = 3;

    return 0;
}

int aux[MAX_T];

void merge_d(int ids[], int left, int m, int right)
{
    int i, j, k;
    task i_t, j_t;
    for (i = m+1; i > left; i--)
        aux[i-1] = ids[i-1];

    for (j = m; j < right; j++)
        aux[right+m-j] = ids[j+1];

    for (k = left; k <= right; k++) {

        i_t = task_list[aux[i] - 1];
        j_t = task_list[aux[j] - 1];

        if (j_t.inst_init < i_t.inst_init || i == m + 1)
            ids[k] = aux[j--];
        else if ((j_t.inst_init == i_t.inst_init) &&
                 strcmp(j_t.desc, i_t.desc) < 0)
            ids[k] = aux[j--];
        else
            ids[k] = aux[i++];
    }
}

void merge_l(int ids[], int left, int m, int right)
{
    int i, j, k;
    task i_t, j_t;
    for (i = m+1; i > left; i--)
        aux[i-1] = ids[i-1];

    for (j = m; j < right; j++)
        aux[right+m-j] = ids[j+1];

    for (k = left; k <= right; k++) {

        i_t = task_list[aux[i] - 1];
        j_t = task_list[aux[j] - 1];

        if (strcmp(j_t.desc, i_t.desc) < 0 || i == m + 1)
            ids[k] = aux[j--];

        else
            ids[k] = aux[i++];
    }
}



/*
n_t = task_list[ids[i+1]-1];
c_t = task_list[ids[i]-1];

if (j_t.inst_init < i_t.inst_init) {
temp = ids[i+1];
ids[i+1] = ids[i];
ids[i] = temp;
trocar = SIM;
}
else if (n_t.inst_init == c_t.inst_init &&
(strcmp(n_t.desc, c_t.desc) < 0)) {
temp = ids[i+1];
ids[i+1] = ids[i];
ids[i] = temp;
trocar = SIM;
}
*/


void mergesort(int ids[], int left, int right, int start_time_flag)
{
    int m = (right+left)/2;
    if (right <= left) return;
    mergesort(ids, left, m, start_time_flag);
    mergesort(ids, m+1, right, start_time_flag);
    start_time_flag ? merge_d(ids, left, m, right) : merge_l(ids, left, m, right);
}



int sort_ids(int ids[], int start_time_flag, int total_tasks){

    int i, temp = 0, index = total_tasks - 1;
    int trocar = SIM;
    task n_t, c_t;


    while (trocar == SIM){
        trocar = NAO;

        for (i = 0; i < index; i++){

            n_t = task_list[ids[i+1]-1];
            c_t = task_list[ids[i]-1];
            /*printf("%d\n next: %d current: %d\n", i, n_t.id, c_t.id);*/
            if (start_time_flag == SIM){

                if (n_t.inst_init < c_t.inst_init) {
                    temp = ids[i+1];
                    ids[i+1] = ids[i];
                    ids[i] = temp;
                    trocar = SIM;
                }
                else if (n_t.inst_init == c_t.inst_init &&
                         (strcmp(n_t.desc, c_t.desc) < 0)) {
                    temp = ids[i+1];
                    ids[i+1] = ids[i];
                    ids[i] = temp;
                    trocar = SIM;
                }
            }

            else{
                if ((strcmp(n_t.desc, c_t.desc) < 0)){
                    /*printf("Trocou para a frente\n\n");*/
                    temp = ids[i+1];
                    ids[i+1] = ids[i];
                    ids[i] = temp;
                    trocar = SIM;
                }
            }

        }

        index--;
    }

    return 0;

}





/* ---------------------------    Principais   ------------------------------ */


/* Cria uma tarefa nova. Referente ao comando "t" */
int create_task(int dur, char descrip[]){

    task new_task;

    /* Verifica se existe algum erro. Nota: Se uma função der erro, retorna
     * sempre 1. */
    if (create_task_check(dur, descrip) == 1) {
        return 1;
    }

    /* Atualiza os parâmetros inteiros */
    new_task.id = nr_tasks + 1;
    new_task.dur = dur;
    new_task.inst_init = 0;

    /* Guarda a descrição e o nome da atividade inicial */
    strcpy(new_task.desc, descrip);
    strcpy(new_task.ativ, A_START);

    /*O utilizador fica por inicializar até a task ser movida */

    task_list[nr_tasks] = new_task;

    /* Imprimo a mensagem, incremento o numero de tarefas e retorno */

    nr_tasks++;
    printf("task %d\n", new_task.id);
    return 0;
}


/* Lista a tarefa com o id correspondente. Referente ao comando "l". */
int list_task(int id){

    /* Verifico se o ID é válido e a tarefa do ID correspondente está definida*/
    if (!(id > 0 && id <= MAX_T) ||
        (strcmp(task_list[id-1].desc, EMPTY_STR) == 0)){
        printf(ERROR8, id);
        return 1;
    }



    /* Como a tarefa foi validada, imprimo a informação */
    printf("%d %s #%d %s\n",
           id,
           task_list[id-1].ativ,
           task_list[id-1].dur,
           task_list[id-1].desc);
    return 0;

}


/* Lista todas as tarefas. Referente ao comando "l", sem parâmetros. */
int list_all_tasks(){

    /*  c_task     -> representa a tarefa atual (mais legível);
     *  menor      -> representa a tarefa com menor descrição, alfabeticamente;
     *  sorted_ids -> representam os ids ordenados que serão usados para listar
     *                todas as tarefas;
     *  all_tasks  -> vetor com todas as tarefas definidas. */

    /*static int tasks_since = 0;*/
    int sorted_ids[MAX_T] = {0};
    int i, j;
    task menor;
    task c_task;

    /* Coloco as novas tarefas criadas desde a invocação da função nas posições
     * correspondentes do vetor 'sorted_ids'.
     * Para saber quantas novas adicionar (de forma a colmatar criar um ciclo
     * extra), uso a variável 'tasks_since', incrementada a cada tarefa nova */

    /*for (i = tasks_since; i < nr_tasks; i++)*/
    /*sorted_ids_a[i] = i;*/

    for (i = 0; i < nr_tasks; i++){
        sorted_ids[i] = i+1;
    }
    mergesort(sorted_ids, 0, nr_tasks-1, NAO);
    /*
    sort_ids(sorted_ids, NAO, nr_tasks);
    */
    for (i = 0; i < nr_tasks; i++){
        list_task(sorted_ids[i]);
    }

    return 0;

    /* Ciclo exterior. Serve para colocar os ids no vetor "sorted_ids". */
    for (i = 0; i < nr_tasks; i++){

        /* Procuro a primeira tarefa que não pertenca já aos ids ordenados e
         * escolho-a como menor. */
        for (j = 0; j < nr_tasks; j++){
            if (belongs(task_list[j].id, sorted_ids) == N_PERTENCE){
                menor = task_list[j];
                break;
            }
        }

        /* Tendo uma menor, percorro agora todas as tarefas até ao fim. */
        for (j = 0; j < nr_tasks; j++){
            /* Para ser mais legivel, "renomeio" a tarefa atual */
            c_task = task_list[j];

            /* Se alguma tiver a descrição menor que a atual menor, substituo
             * caso não esta já nos ids ordenados */
            if ((strcmp(c_task.desc, menor.desc) < 0) &&
                (belongs(c_task.id, sorted_ids) == N_PERTENCE)){
                menor = c_task;
            }
        }
        /* Chegado ao fim, atualizo os ids_ordenados usando "i" como indice */
        sorted_ids[i] = menor.id;
    }


    /* Tendo o vetor com os ids por ordem de descrição, chamo a função de
     * listagem individual para cada entrada do vetor */
    for (i = 0; i < nr_tasks; i++){
        list_task(sorted_ids[i]);
    }

    return 0;
}


/* Atualiza o tempo. Referente ao comando "n". */
int time_skip(int n) {

    if (n < 0) {
        printf(ERROR9);
        return 1;
    }

    time += n;
    printf("%d\n", time);
    return 0;

}


/* Cria um novo utilizador. Referente ao comando "u". */
int create_user(char name[]){
    int i;

    /* Verifica se excedeu o número de utilizadores */
    if (nr_users == MAX_U){
        printf(ERROR10);
        return 1;
    }

    /* Verifica se o utilizador já existe, percorrendo um ciclo enquanto
     * existirem utilizadores definidos (ou chegar ao máximo de utilizadores) */
    for (i = 0; strcmp(user_list[i], EMPTY_STR) != 0 && i < MAX_U; i++){
        if ( strcmp(user_list[i], name) == 0){
            printf(ERROR11);
            return 1;
        }
    }

    /* Caso não exista, preenche o primeiro slot não ocupado. Reutilizo o indice
     * "i" do ciclo anterior. */
    strcpy(user_list[i], name);
    nr_users++; /* Incremento o número total de utilizadores */
    return 0;
}

/* Lista todos os utilizadores. Referente ao comando "u", sem parâmetros. */
int list_all_users(){
    int i;

    /* Imprime os elementos do vetor até encontrar um não definido */
    for (i = 0; strcmp(user_list[i], EMPTY_STR) != 0 && i < MAX_U; i++) {
        printf("%s\n", user_list[i]);
    }

    return 0;
}


/* Move uma tarefa entre atividades. Referente ao comando "m".
 * Dividi em duas funcões, onde a primeira verifica todos os possiveis erros. */
int move_task(int id, char user[], char activ[]) {

    /* destino -> guarda a atividade de destino ou outra */
    int i, duration, slack, destino = !DONE;

    /* Verifica se existe algum erro. Nota: Se uma função der erro, retorna
     * sempre 1. */
    if (move_task_check(id, user, activ) == 1) {
        return 1;
    }

    /* Procuro o indice da lista de atividades da atividade de destino.
     * Caso o destino seja DONE, guardo numa variável. */
    for (i = 0; i < nr_activ; i++) {
        if (strcmp(activ, activ_list[i]) == 0) {

            if (strcmp(activ, A_LAST) == 0) {
                destino = DONE;
            }
            break;
        }
    }

    /* Verifico se a atividade de origem foi a inicial. Se sim,atualizo o
     * instante inicial */
    if (strcmp(task_list[id - 1].ativ, A_START) == 0) {
        task_list[id - 1].inst_init = time;
    }

    /* Atualizo os parâmetros "utilizador" e "atividade" da tarefa */
    strcpy(task_list[id - 1].user, user);
    strcpy(task_list[id - 1].ativ, activ);

    /* No caso do destino ser a atividade final, processo a impressão de saida */
    if (destino == DONE) {
        duration = time - task_list[id - 1].inst_init;
        slack = duration - task_list[id - 1].dur;

        printf("duration=%d slack=%d\n", duration, slack);
    }

    return 0;
}


/* Lista todas as tarefas de uma atividade. Referente ao comando "d". */
int list_activity_tasks(char activ[]) {

    int i, j = 0;
    int sorted_ids[MAX_T] = {0};
    task c_t;

    /* Verifico se a atividade existe */
    for (i = 0; i < nr_activ; i++){
        /* Se encontrar alguma atividade com o nome do input, para o ciclo */
        if (strcmp(activ, activ_list[i]) == 0){
            break;
        }
    }
    /*Se chegou ao fim do ciclo e nao encontrou, entao a atividade nao existe */
    if (i == nr_activ){
        printf(ERROR3);
        return 1;
    }


    for (i = 0; i < nr_tasks; i++){
        if (strcmp(activ, task_list[i].ativ) == 0) {
            sorted_ids[j] = task_list[i].id;
            j++;
        }
    }

    if (j > 1)
        /*
        sort_ids(sorted_ids, SIM, j);
        */
        mergesort(sorted_ids, 0, nr_tasks-1, SIM);

    if (j == 1){
        c_t = task_list[sorted_ids[0]-1];
        printf("%d %d %s\n", c_t.id, c_t.inst_init, c_t.desc);

    }
    else if (j > 1){
        for (i = 0; i < j; i++) {
            c_t = task_list[sorted_ids[i] - 1];
            printf("%d %d %s\n", c_t.id, c_t.inst_init, c_t.desc);
        }
    }

    return 0;
}


/* Adiciona uma atividade. Referente ao comando "a". */
int add_activity(char activ_desc[]){

    if (add_activity_check(activ_desc) == 1){
        return 1;
    }

    /* Crio uma nova atividade no id = número de atividades + 1 */

    strcpy(activ_list[nr_activ], activ_desc);

    /*
    for (i = 0; activ_desc[i] != '\0'; i++){
        printf("char: %c, c == |t %d \n", activ_list[nr_activ][i], activ_list[nr_activ][i] == '\t');
    }
    printf("\n\n");
    */

    /* Incremento o número de atividades */
    nr_activ++;
    return 0;
}

/* Lista todas as atividades. Referente ao comando "a", sem paramêtros. */
int list_all_activities(){
    int i;

    /* Imprime as atividades usando o número de atividades existentes. */
    for (i = 0; i < nr_activ; i++){
        printf("%s\n", activ_list[i]);
    }

    return 0;
}



/* =============================================================================
 * ===============                    MAIN                     =================
 * =============================================================================
*/

int main() {

    /* id  -  -  -  -  -  -   -> Usada para definir um vetor com todos os ids.
     * temp1, temp2, tempInt1 -> Variáveis temporárias "multi-funções" que são
     *                           usadas em vários casos do Switch;
     * z, c  -  -  -  -  -  - -> Usadas para ler caracteres, também
     *                           "multi-funcoes";
     * num  -  -  -  -  -  -  -> Acumula um número ao ler do terminal;
     * sign -  -  -  -  -  -  -> Indica o sinal de um número ao ser lido;
     */


    int i,j, cont = 0;
    int tempInt1;
    char c, temp1[MAX_T],temp2[MAX_T], z;
    int num = 0;
    int menos = NAO;

    initialize_arrays();

    while (1){
        switch (c = getchar()) {

            /* Acaba o Programa */
            case 'q':
                return 0;
                break;

                /* Nova Tarefa */
            case 't':
                /* Guarda a duração e "salta" o espaço. */
                scanf("%d", &tempInt1);
                getchar();

                /* Guarda a descrição. */
                i = 0;
                while (( (z = getchar()) != '\n') && (i < MAX_D_T-1)){
                    temp1[i] = z;
                    i++;
                }
                temp1[i] = '\0';
                create_task(tempInt1, temp1);
                break;

                /* Lista Tarefas */
                /* variavel 'cont' usada para contar o número de listagens. Apenas
                 * importante se for != 0 */
            case 'l':
                /* Ignora os espaços */
                while ( (z = getchar()) == ' ') {}

                /* Se o primeiro caracter != ' ' for '\n', lista todas as
                 * tarefas  */
                if (z == '\n'){
                    list_all_tasks();
                    break;
                }
                else {
                    /* Vai acumulando os numeros e fazendo a listagem após
                     * encontrar um espaco. */
                    while (z != '\n') {
                        if ('0' <= z && z <= '9') {
                            num = (num * 10) + (z - '0');
                            cont++;
                        }

                        else if (z == '-'){
                            menos = SIM;
                        }
                        else if ((z == ' ') && (cont != 0)) {
                            if (menos == SIM)
                                num = -num;
                            list_task(num);
                            /* 'num' e 'cont' são reiniciados para obter um novo
                             * número e conseguir ignorar vários espaços
                             * seguidos, respetivamente. */
                            num = cont = 0;
                            menos = NAO;

                        }
                        z = getchar();
                    }
                }
                /* Lista o último número, se diferente de 0. */
                if (num != 0){
                    if (menos == SIM)
                        num = -num;
                    list_task(num);
                }

                break;

                /* Avança || Vê Tempo */
            case 'n':

                scanf("%d", &num);
                time_skip(num);

                break;

                /* Adicina || Lista Utilizadores */
            case 'u':

                /* Ignora os espaços */
                while ( (z = getchar()) == ' ') {}

                /* Se o primeiro caracter != ' ' for '\n', lista todos os
                 * utilizadores.  */
                if (z == '\n'){
                    list_all_users();
                    break;
                }

                /* O contador neste contexto conta o número de espaços. Caso
                 * tenha havido algum espaço, não é criado utilizador. */
                i = cont = 0;

                while ((z != '\n') && (z != ' ') && (i < MAX_N_U-1)){
                    temp1[i] = z;
                    i++;
                    z = getchar();
                }

                if ( z == ' '){
                    cont++;
                }

                temp1[i] = '\0';
                /* Apenas é criado um utilizador caso não tenha aparecido um
                 * espaço em branco */
                if (cont == 0) {
                    create_user(temp1);
                }
                break;

                /* Move Tarefa */
            case 'm':
                /* Guarda o id e o Utilizador. */
                scanf("%d%s", &tempInt1, temp1);

                /* Obtem o nome da atividade de destino. */
                i = 0;
                getchar();
                while (( (z = getchar()) != '\n') && (i < MAX_N_A-1)){
                    temp2[i] = z;
                    i++;
                }
                temp2[i] = '\0';

                /* Chama a função */
                move_task(tempInt1, temp1, temp2);
                break;


                /* Lista Atividades de Tarefa */
            case 'd':
                /* Obtém o nome da atividade. */
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
                tempInt1 = 0;
                /* Ignora os espaços */
                while ( (z = getchar()) == ' ')
                    tempInt1++;

                /* Se o primeiro caracter != ' ' for '\n', lista todas as
                 * atividades.  */
                if (z == '\n'){
                    list_all_activities();
                    break;
                }


                /* Obtém o nome da atividade. */
                i = 0;

                /* Compensa os espaços anteriores */
                tempInt1--;
                while(i < tempInt1){
                    temp1[i] = ' ';
                    i++;
                }


                while ((z != '\n') && (i < MAX_N_A-1)){
                    temp1[i] = z;
                    z = getchar();
                    i++;
                }
                temp1[i] = '\0';
                add_activity(temp1);
                break;

        }

        /* Dá reset a todas as variáveis utilizadas */
        for (j = 0; j <= i; j++){
            temp1[j] = ' ';
            temp2[j] = ' ';
        }
        num = cont = i = 0;
    }

    return 0;
}


