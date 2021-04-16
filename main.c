
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
/* version 5.2 -> Não mais time error. Tem 7 Wrong Awesers. Vou tentar mudar as tarefas de forma igual às atividades */

/* version 6.0 -> Feito */
/* version 6.2 -> Final Comments */

/* File: proj1.c
 * Author: Pedro Chaparro ist199298
 * Description: Projeto 1 de IAED: Criação de um programa que simula o método
 *              kanban. A idea geral basea-se na estrutura tarefa, composta
 *              por vários elementos do método kanban (duração, atividade, ...)
 *              e um vetor de tarefas.
 *              Estas tarefas e atividades serão manipuladas consoante o input
 *              do utilizador ao interagir com o terminal.
 *              Foi usada a convenção que uma função retorna 0 quando não
 *              ocorrem erros e 1 caso contrário.
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
 * definir */
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

/* Estrutura 'task'. Serve para armazenar toda a informação relativa a tarefas.
 * Guarda o identificador (id), a duração, o instante de inicio, a descrição,
 * o utilizador que tomou conta e a atividade em que a tarefa se encontra. */
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
 * se o número máximo de cada elemento é ultrapassado e em certos ciclos. */
int nr_tasks = 0, nr_activ = 0, nr_users = 0;

int time = 0; /* O tempo simulado, íniciado a 0. */

task task_list[MAX_T];           /* Vetor que armazena as tarefas;       */
char activ_list[MAX_A][MAX_N_A]; /* Vetor que armazena as atividades;    */
char user_list[MAX_U][MAX_N_U];  /* Vetor que armazena os utilizadores;  */

int aux[MAX_T]; /* Vetor auxiliado usado no algoritmo de ordenação */

/* Tarefa Vazia. Usada para inicializar o vetor de tarefas. */
const task blank_task = {0};

/* =============================================================================
 * ===============                   FUNÇÕES                   =================
 * =============================================================================
*/


/* -----------------------------    Auxiliares   ---------------------------- */



/*                    Verificações de Erros / Inicializações                  */

/* Estas funções auxiliares devolvem 1 caso tenha sido detetado algum erro.
 * Esse erro será depois processado na função principal correspondente. */

/* Verifica se existe algum erro na criação de uma tarefa nova */
int create_task_check(int dur, char descrip[]) {

    int i;

    /* Verifica o número máximo de tarefas */
    if (nr_tasks == MAX_T) {
        printf(ERROR5);
        return 1;
    }

    /* Verifica se a descrição é igual a alguma das tarefas existentes */
    for (i = 0; i < nr_tasks; i++) {
        if (strcmp(task_list[i].desc, descrip) == 0) {
            printf(ERROR6);
            return 1;
        }
    }

    /* Verifica se a duração é válida */
    if (dur <= 0) {
        printf(ERROR7);
        return 1;
    }

    return 0;
}

/* Verifica se todas as condições para mover uma tarefa se aplicam. */
int move_task_check(int id, char user[], char activ[]){

    /* encontrou -> variável de estado que indica se foi encontrado um
     * utilizador */
    int i, encontrou = NAO;

    /* Verifica se a task já foi criada */
    if (strcmp(task_list[id-1].desc, EMPTY_STR) == 0){
        printf(ERROR4);
        return 1;
    }

    /* Verifica se a atividade a mover é a mesma */
    if (strcmp(activ, task_list[id-1].ativ) == 0){
        return 1;
    }

    /* Verifica se a atividade a mover é "TO DO" */
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

/* Verifica se é possivel adicionar uma atividade */
int add_activity_check(char activ_desc[]){
    int i;

    /* Verifica atividades repetidas. */
    for (i = 0; i < nr_activ; i++){
        if ( strcmp(activ_desc, activ_list[i]) == 0) {
            printf(ERROR14);
            return 1;
        }
    }

    /* Verifica se alguma letra é minuscula. */
    for (i = 0; i < MAX_N_A-1 && activ_desc[i] != '\0'; i++){

        if (activ_desc[i] >= 'a' && activ_desc[i] <= 'z'){
            printf(ERROR13);
            return 1;
        }
    }

    /* Verifica se o total de atividades foi ultrapassado. */
    if (nr_activ == MAX_A){
        printf(ERROR12);
        return 1;
    }

    return 0;
}



/*                             Leituras do Terminal                           */

/* Funções usadas para ler do terminal alguma informação e fazer o seu
 * processamento. Usadas em conjunto com "switches" localizados na main(). */

/* Lê a descrição de uma tarefa */
int read_task_desc(char desc[]){

    char z;
    int i;
    /* Ignora os espaços */
    while ( (z = getchar()) == ' ') {}

    /* Guarda a descrição. */
    i = 0;
    while (( z != '\n') && (i < MAX_D_T-1)){
        desc[i] = z;
        z = getchar();
        i++;
    }
    desc[i] = '\0';

    return 0;

}


/* Prótótipo da fução que lista as tarefas */
int list_task( int num);

/*Lê do terminal uma sequência de números correspondentes a tarefas e lista-as*/
int read_and_list_tasks(char z){

    int num = 0, menos = 0, cont = 0;
    /* Acumula os números e faz a listagem após encontrar um
                 * espaco. */
    while (z != '\n') {
        if ('0' <= z && z <= '9') {
            num = (num * 10) + (z - '0');
            /* Conta o número de digitos */
            cont++;
        }
        /* Verifica um sinal negativo */
        else if (z == '-'){
            menos = SIM;
        }
            /* Se existir um espaço e tenha havido pelo menos um
             * dígito */
        else if ((z == ' ') && (cont != 0)) {
            if (menos == SIM)
                num = -num;
            list_task(num);

            /* As variáveis são reinicializadas */
            num = cont = 0;
            menos = NAO;

        }
        z = getchar();
    }

    /* Lista o último número, se diferente de 0. */
    if (num != 0){
        if (menos == SIM)
            num = -num;
        list_task(num);
    }

    return 0;
}

/* Prótótipo da fução que cria um utilizador */
int create_user(char name[]);

/* Lê do terminal um nome e cria um utilizador com esse nome */
int read_and_create_user(char z){

    /* O contador serve para contar o número de espaços.
     * Se existir algum, não é criado utilizador */
    int i = 0, cont = 0;
    char user[MAX_N_U];

    while ((z != '\n') && (z != ' ') && (i < MAX_N_U-1)){
        user[i] = z;
        i++;
        z = getchar();
    }

    if ( z == ' '){
        cont++;
    }

    user[i] = '\0';
    /* Apenas é criado um utilizador caso não existam espaços em branco */
    if (cont == 0) {
        create_user(user);
    }
    return 0;
}

/* Protótipo da função que adiciona atividades */
int add_activity(char activ[]);

/* Lê uma atividade e adiciona-a */
int read_and_add_activ(char z){

    int i = 0;
    char activ[MAX_N_A];

    while ((z != '\n') && (i < MAX_N_A-1)){
        activ[i] = z;
        z = getchar();
        i++;
    }
    activ[i] = '\0';
    add_activity(activ);

    return 0;

}

/* Lê o nome de uma atividade */
int read_activity(char activ[]){

    char z;
    int i = 0;

    getchar();
    while (( (z = getchar()) != '\n') && (i < MAX_N_A-1)){
        activ[i] = z;
        i++;
    }
    activ[i] = '\0';

    return 0;
}

/*                                     misc                                   */

/*Inicializa todos os vetores */
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

/*Função merge na ordenação por descrição */
void merge_l(int ids[], int left, int m, int right);
/*Função merge na ordenação por instante inicial */
void merge_d(int ids[], int left, int m, int right);

/* Função de ordenação mergesort.
 * Invoca diferentes merges dependendo se o vetor é ordenado por instante
 * inicial ou não. Para isso, é usada uma flag. */
void mergesort(int ids[], int left, int right, int start_time_flag){
    int m = (right+left)/2;

    if (right <= left) return;

    mergesort(ids, left, m, start_time_flag);
    mergesort(ids, m+1, right, start_time_flag);

    /* Verifica se a ordenação é por instante inicial ou descrição */
    start_time_flag ? merge_d(ids, left,m, right) : merge_l(ids, left,m, right);
}

/* Implementação do merge por instante inicial. Referente ao comando "d" */
void merge_d(int ids[], int left, int m, int right){

    int i, j, k;
    /* Tarefas atuais de cada iterador. Para tornar mais legível. */
    task i_t, j_t;

    /* Inicializa os vetores */
    for (i = m+1; i > left; i--)
        aux[i-1] = ids[i-1];
    for (j = m; j < right; j++)
        aux[right+m-j] = ids[j+1];

    /* Faz as comparações necessárias e atualiza o vetor principal */
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

/* Implementação do merge por descrição. Referente ao comando "l" */
void merge_l(int ids[], int left, int m, int right){
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


/* ---------------------------    Principais   ------------------------------ */


/* Cria uma tarefa nova. Referente ao comando "t" */
int create_task(int dur, char descrip[]){

    task new_task;

    /* Verifica se existe algum erro.
     * Nota: Se uma função der erro, retorna sempre 1. */
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

    /* Imprimo a mensagem, incremento o número de tarefas e retorno */

    nr_tasks++;
    printf("task %d\n", new_task.id);
    return 0;
}


/* Lista a tarefa com o id correspondente. Referente ao comando "l". */
int list_task(int id){

    /* Verifico se o id é válido e a tarefa do id correspondente está definida*/
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

    /*  sorted_ids -> representam os ids ordenados que serão usados para listar
     *                todas as tarefas;
     */

    int sorted_ids[MAX_T] = {0};
    int i;

    /* Coloca todos os ids definidos num vetor */
    for (i = 0; i < nr_tasks; i++){
        sorted_ids[i] = i+1;
    }
    /* Ordena o vetor consoante a ordem alfabética.
     * Foi usada a flag NAO para indicar que o merge não será feito por
     * instante inicial. */
    mergesort(sorted_ids, 0, nr_tasks-1, NAO);

    /* Lista todos os vetores, por ordem */
    for (i = 0; i < nr_tasks; i++){
        list_task(sorted_ids[i]);
    }

    return 0;

}


/* Atualiza o tempo. Referente ao comando "n". */
int time_skip(int n) {

    /* Caso o tempo seja inválido */
    if (n < 0) {
        printf(ERROR9);
        return 1;
    }

    /* Incrementa a variável global */
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

    /* Caso não exista, preenche o primeiro slot não ocupado. O indice "i" do
     * ciclo anterior é reutilizado. */
    strcpy(user_list[i], name);
    nr_users++; /* O número total de utilizadores é incrementado */
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


/* Move uma tarefa entre atividades. Referente ao comando "m". */
int move_task(int id, char user[], char activ[]) {

    /* destino -> verifica se a atividade de destino é DONE */
    int i, duration, slack, destino = !DONE;

    /* Verifica se existe algum erro.
     * Nota: Se uma função der erro, retorna sempre 1. */
    if (move_task_check(id, user, activ) == 1) {
        return 1;
    }

    /* Procura o indice da lista de atividades da atividade de destino.
     * Caso o destino seja DONE, é guardado numa variável. */
    for (i = 0; i < nr_activ; i++) {
        if (strcmp(activ, activ_list[i]) == 0) {

            if (strcmp(activ, A_LAST) == 0) {
                destino = DONE;
            }
            break;
        }
    }

    /* Verifica se a atividade de origem foi a inicial. Se sim, atualiza o
     * instante inicial */
    if (strcmp(task_list[id - 1].ativ, A_START) == 0) {
        task_list[id - 1].inst_init = time;
    }

    /* Atualiza os parâmetros "utilizador" e "atividade" da tarefa */
    strcpy(task_list[id - 1].user, user);
    strcpy(task_list[id - 1].ativ, activ);

    /* No caso do destino ser a atividade final, processa a impressão de saida*/
    if (destino == DONE) {
        duration = time - task_list[id - 1].inst_init;
        slack = duration - task_list[id - 1].dur;

        printf("duration=%d slack=%d\n", duration, slack);
    }

    return 0;
}


/* Lista todas as tarefas de uma atividade. Referente ao comando "d". */
int list_activity_tasks(char activ[]) {

    /*  sorted_ids -> representa os ids das tarefas ordenados que serão usados
     * para listar todas as tarefas; */
    int i, j = 0;
    int sorted_ids[MAX_T] = {0};
    task c_t;

    /* Verifica se a atividade existe */
    for (i = 0; i < nr_activ; i++){
        /* Se encontrar alguma atividade com o nome do input, para o ciclo */
        if (strcmp(activ, activ_list[i]) == 0){
            break;
        }
    }
    /*Se chegou ao fim do ciclo e não encontrou, então a atividade não existe */
    if (i == nr_activ){
        printf(ERROR3);
        return 1;
    }

    /* Percorre todas as tarefas e guarda os ids das pertencentes à atividade no
     * vetor */
    for (i = 0; i < nr_tasks; i++){
        if (strcmp(activ, task_list[i].ativ) == 0) {
            sorted_ids[j] = task_list[i].id;
            j++;
        }
    }

    /* Caso o vetor tenha mais que um elemento, ordena-o */
    if (j > 1)
        mergesort(sorted_ids, 0, j-1, SIM);

    /* Imprime todas as tarefas, ordenadas */
    if (j >= 1){
        for (i = 0; i < j; i++) {
            c_t = task_list[sorted_ids[i] - 1];
            printf("%d %d %s\n", c_t.id, c_t.inst_init, c_t.desc);
        }
    }

    return 0;
}


/* Adiciona uma atividade. Referente ao comando "a". */
int add_activity(char activ_desc[]){

    /* Verifica se é possível adicionar a atividade */
    if (add_activity_check(activ_desc) == 1){
        return 1;
    }

    /* Cria uma nova atividade no id = número de atividades + 1 */
    strcpy(activ_list[nr_activ], activ_desc);

    /* Incrementa o número de atividades */
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

    /* temp1,temp2, tempInt1 -> Variáveis temporárias usadas em multiplos casos;
     * z, c  -  -  -  -  - - -> Usadas para ler caracteres;
     * num  -  -  -  -  -  - -> Acumula um número ao ler do terminal;
     * menos -  -  -  -  - - -> Indica o sinal de um número ao ser lido;
     */

    int i;
    int tempInt1;
    char c, temp1[MAX_T],temp2[MAX_T], z;
    int num = 0;

    /* Inicializa todos os vetores */
    initialize_arrays();

    /* Processamento do input do utilizador */
    while (1){
        switch (c = getchar()) {

                /* Termina o Programa */
            case 'q':
                return 0;
                break;


                /* Cria uma Nova Tarefa */
            case 't':
                /* Guarda a duração e lê a descrição. */
                scanf("%d", &tempInt1);
                read_task_desc(temp1);
                create_task(tempInt1, temp1);
                break;


                /* Lista Tarefas */
            case 'l':
                /* Ignora os espaços */
                while ( (z = getchar()) == ' ') {}

                /* Se o primeiro caracter não vazio for '\n', lista todas as
                 * tarefas  */
                z == '\n' ? list_all_tasks() : read_and_list_tasks(z);
                break;


                /* Avança || Vê Tempo */
            case 'n':

                scanf("%d", &num);
                time_skip(num);
                break;


                /* Adicina ou Lista Utilizadores */
            case 'u':
                /* Ignora os espaços */
                while ( (z = getchar()) == ' ') {}

                /* Se o primeiro caracter não vazio for '\n', lista todos os
                 * utilizadores.  */
                z == '\n' ? list_all_users() : read_and_create_user(z);

                break;


                /* Move Tarefa */
            case 'm':
                /* Guarda o id e o Utilizador. */
                scanf("%d%s", &tempInt1, temp1);

                /* Obtem o nome da atividade de destino. */
                read_activity(temp2);

                /* Chama a função */
                move_task(tempInt1, temp1, temp2);
                break;


                /* Lista Atividades de Tarefa */
            case 'd':
                /* Obtém o nome da atividade. */
                read_activity(temp1);

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
                z == '\n' ? list_all_activities() :  read_and_add_activ(z);

                break;

        }

        /* É feito reset a todas as variáveis utilizadas */
        for (i = 0; i < MAX_T; i++){
            temp1[i] = ' ';
            temp2[i] = ' ';
        }
        num = 0;
    }

    /* Done! :D */
    return 0;
}


