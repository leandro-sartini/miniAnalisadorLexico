/*  Engenharia da Computação - BEC
    Leandro Sartini de Campos
    Gustavo Donnangelo Cassettari */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NOVALINHA 13
#define BRANCO 32
#define TAMANHO 1000

//Inicia uma estrutura Atomo
typedef enum{
    ERRO,
    IDENTIFICADOR,
    NUM_INT,
    NUM_REAL,
    OP_LOGICO,
    OP_RELACIONAL,
    FRASE,
    COMENTARIO,
    PALAVRA_RESERVADA,
    ATRIBUICAO,
    PONTO,
    ABRE_PAR,
    FECHA_PAR,
    PONTO_VIRGULA,
    VIRGULA,
    SUBTRACAO,
    ADICAO,
    MULTIPLICACAO,
    DIVISAO,
    RESTO,
    EOS
}Atomo;

//Array com uma lista de strings para as mensagems com nomes dos Atomos
char *msgAtomo[] = {
    "ERRO",
    "IDENTIFICADOR",
    "NUMERO INTEIRO",
    "NUMERO REAL",
    "OP_LOGICO",
    "OP_RELACIONAL",
    "FRASE",
    "COMENTARIO",
    "PALAVRA_RESERVADA",
    "ATRIBUICAO",
    "PONTO",
    "ABRE_PAR",
    "FECHA_PAR",
    "PONTO_VIRGULA",
    "VIRGULA",
    "SUBTRACAO",
    "ADICAO",
    "MULTIPLICACAO",
    "DIVISAO",
    "RESTO",
    "FIM DO ARQUIVO"};

//Array com uma lista de strings para as mensagems com nomes das palavras reservadas
char *PalavrasRes[] = {
    "ALGORITMO", // Inicio do algoritmo
    "ATE", // Inicializa a variável de controle do PARA
    "CADEIA", // Tipo de dado para sequência de caracteres
    "CARACTER", // Tipo de dado para definir um caracter
    "ENQUANTO", // Determina um laço com condição no início
    "ENTAO", // usado no bloco de estrutura de seleção
    "FACA", // usado no bloco de estrutura de repetição
    "FIM", // Fim de um bloco ou do programa
    "FUNCAO", // Determina uma função no algoritmo
    "INICIO", // Início do algoritmo
    "INTEIRO", // Tipo de dado para números inteiros
    "PARA", // Determina um laço com variável de controle
    "PASSO", // Incremento da variável de controle na repetição
    "PROCEDIMENTO", // Determina um procedimento no algoritmo
    "REAL", // Tipo de Dado para números reais ( ponto flutuante )
    "REF", // Operador para de referência para variáveis
    "RETORNE", // Retorna de uma função
    "SE", // Determina uma estrutura de seleção
    "SENAO", // Caso contrário da instrução de seleção
    "VARIAVEIS" // Início do bloco de declaração de variáveis
};

//Declarando as funções
Atomo reconhece_ID(char);
Atomo reconhece_NUM(char);
Atomo reconhece_OP(char);
Atomo reconhece_FRASE(char);
Atomo reconhece_COMM(char);
Atomo reconhece_n_atrib(char);
char proximo_char(void);
Atomo proximo_token();
void retract_char();

//variaveis globais
char *buffer;
int nlinha = 1;
char *Atomo1;
int numPalavrasRes = 20;

//Faz o buffer andar um espaço para frente na memória do vetor
char proximo_char(void) {
    return *buffer++;
}

//Faz o buffer andar um espaço para trás na memória do vetor
void retract_char(){
    buffer--;
}


//Esta função compara a palavra que está recebendo da função reconhece_ID com uma palavra do Array PalavrasRes
Atomo reconhece_RES(char *palavra){
    for( int i = 0; i < 20; i++ ) {
        if ( ( stricmp(palavra, PalavrasRes[i]) ) == 0) {
            Atomo1 = palavra;
            return PALAVRA_RESERVADA; // Retorna Palavra reservada
        }
    }
    Atomo1 = palavra;
    return IDENTIFICADOR; //Retorna identificador caso não tenha encontrado Palavra Reservada
}


//Esta função reconhece se é identificador ou não
Atomo reconhece_ID(char n){
    char c;
    char *palavra = (char *)calloc( 50 , sizeof(char) );
    int i = 0;
    Atomo atomo;
    palavra[i++] = n;
    c = proximo_char();
    palavra[i++] = c;

    while( isalpha(c) || isdigit(c)) {
        c = proximo_char();
        if ( c != '\n' && c != ' ') {
            palavra[i++] = c;
        }   
    }
    
    atomo = reconhece_RES(palavra); //Joga a palavra na função para identificar se é palavra reservada ou não
    retract_char();
    return atomo; //Retorna o atomo para o main
}



//Essa função faz toda a leitura dos chars ao gravados no arquivo.ptl
Atomo proximo_token() {
    char c,d;
    Atomo atomo = ERRO;
    //ignora espaço em branco e trata nova linha
    while(*buffer == ' ' || *buffer == '\n') {
        if(*buffer == '\n') {
            nlinha++;
        }
        buffer++;
    }

    if(*buffer == '\0') {
            Atomo1 = "EOS";
            return EOS;
    }

    c = proximo_char();

    if( isalpha(c) ) { //Se c for alfanumérico ativa reconhece_ID
        atomo = reconhece_ID(c);
    }
    if( isdigit(c) ) { //Se c for um digito ativa reconhece_NUM
        atomo = reconhece_NUM(c);
    }

    if(isascii(c) && !isalnum(c) && c != '"'){ //Se c for ASCII, diferente de alfanumérico e diferente de " ativa reconhece_OP
        atomo = reconhece_OP(c);
    }

    if(c == '"'){ //Se c = A " Inicia a busca por uma frase ativando reconhece_FRASE
        atomo = reconhece_FRASE(c);
    }

    if(c == '/'){ //Se c = / Inicia a busca por um comentário ativando reconhece_COMM
        atomo = reconhece_COMM(c);
    } 

    if(((int)c >= 40 && (int) c <= 46) || (int) c == 37 || (int) c == 59){ //Se c estiver estre alguns ASCII especificos ativa reconhece_n_atrib
        atomo = reconhece_n_atrib(c);
    }

    return atomo;
}


//Esta função reconhece um número inteiro ou real com um operador fracionario ou exponencial utilizando máquinas de estado retornando o Atomo NUM_REAL ou NUM_INTEIRO e o Lexeme Número.númeroE(+||-)Número
Atomo reconhece_NUM(char n){
    char c,d;
    int i = 0;
    char *numero = (char *)calloc( 50 , sizeof(char) );
    numero[i++] = n;
    int estado = 2;
    while(1){
        switch (estado)
        {
            case 2:
                c = proximo_char();
                if (c == '.') {
                    numero[i++] = c;
                    estado = 3;
                } else if ( isdigit(c) ) {
                    numero[i++] = c;
                    estado = 2;
                } else if ( !isalpha(c) ) {
                    retract_char();
                    Atomo1 = numero;
                    return NUM_INT;
                } else if (c == 'e' || c == 'E'){
                    numero[i++] = c;
                    estado = 5;
                } else {
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 3:
                c = proximo_char();
                if ( isdigit(c)) {
                    numero[i++] = c;
                    estado = 4;
                } else {
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 4:
                c = proximo_char();
                if(isdigit(c)) {
                    numero[i++] = c;
                    estado = 4;
                } else if (c == 'e' || c == 'E'){
                    numero[i++] = c;
                    estado = 7;
                } else if ( !isalpha(c) ) {
                    numero[i++] = c;
                    retract_char();
                    Atomo1 = numero;
                    return NUM_REAL;
                } else {
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 5:
                c = proximo_char();
                if(c == '+' || c == '-'){
                    numero[i++]=c;
                    estado = 6;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;                    
                }
            break;
            case 6:
                c = proximo_char();
                if (isdigit(c)){
                    numero[i++]=c;
                    estado = 6;
                } else if ( !isalpha(c) ) {
                    numero[i++] = c;
                    retract_char();
                    Atomo1 = numero;
                    return NUM_INT;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;                    
                }
            break;
            case 7:
                c = proximo_char();
                if(c == '+' || c == '-'){
                    numero[i++]=c;
                    estado = 8;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;                    
                }
            break;
            case 8:
                c = proximo_char();
                if (isdigit(c)){
                    numero[i++]=c;
                    estado = 8;
                } else if ( !isalpha(c) ) {
                    numero[i++] = c;
                    retract_char();
                    Atomo1 = numero;
                    return NUM_REAL;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;                    
                }
            break;
        }
    }

}


//Esta função reconhece um Operador logico ou Relacional utilizando máquinas de estado e retornando o Atomo OP_RELACIONAL ou OP_Logico e o Lexeme < > = & $ ! <= >= <>
Atomo reconhece_OP(char n){
    char c,d;
    retract_char();
    int i = 0;
    char *numero = (char *)calloc( 50 , sizeof(char) );
    c = proximo_char();
    d = proximo_char();
    retract_char();
    int estado = 2;
    while(1){
        switch (estado)
        {
            case 2:
                if(c == '<' && (d == '>' || d == '=' || d == '_')){
                    numero[i++] = c;
                    estado = 3;
                } else if(c == '>' && d == '='){
                    numero[i++] = c;
                    estado = 4;
                } else if(c == '<'){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_RELACIONAL;
                } else if (c == '>'){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_RELACIONAL;
                } else if (c == '='){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_RELACIONAL;
                } else if (c == '$' || c == '&' || c == '!'){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_LOGICO;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 3:
                c = proximo_char();
                if((c =='=' || c == '>')){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_RELACIONAL;
                } else if(c == '_'){
                    numero[i++] =c;
                    Atomo1= numero;
                    return ATRIBUICAO;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 4:
                c = proximo_char();
                if(c == '='){
                    numero[i++] = c;
                    Atomo1 = numero;
                    return OP_RELACIONAL;
                } else{
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
        }
    }
}


//Esta função reconhece uma frase utilizando máquinas de estado e retornando o Atomo Frase e o Lexeme " Frase \" inputada "
Atomo reconhece_FRASE(char n){
    char c;
    retract_char();
    int i = 0;
    char *numero = (char *)calloc( 1000 , sizeof(char) );
    numero[i++] = n;
    c = proximo_char();
    int estado = 2;
    while(1){
        switch (estado)
        {
            case 2:
                c = proximo_char();
                if( (int)c == 92){
                    numero[i++] = c;                
                    estado = 4;   
                } else if (isalnum(c) || isascii(c)) {
                    numero[i++] = c;
                    estado = 3;
                } else {
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 3:
                c = proximo_char();
                if( (int)c == 92){                 
                    numero[i++] = c;
                    estado = 4;   
                } else if ( c == '"') {                   
                    numero[i++] = c;
                    Atomo1 = numero;    
                    return FRASE;
                } else if ( isalnum(c) || isascii(c)) {
                    numero[i++] = c;
                    estado = 3;
                } else {
                    Atomo1 = "ERRO";
                    return ERRO;
                }
            break;
            case 4:
                c = proximo_char();
                if ( c == '"') {                   
                    numero[i++] = c;
                    estado = 3;
                } else {
                    numero[i++] = c;
                    estado = 3;
                } 
            break;
        }
    }

}

////Esta função reconhece um comentário utilizando máquinas de estado e retornando o Atomo Comentario e o Lexeme /* Qualquer coisa digitada */
Atomo reconhece_COMM(char n){
    char c,d;
    int i = 0;
    char *numero = (char *)calloc( 1000 , sizeof(char) );
    numero[i++] = n;
    retract_char();
    c = proximo_char();
    d = proximo_char();
    retract_char();
    retract_char();
    int estado = 2;
    if(c == '/'){
        while(1){
            switch (estado)
            {   
                case 2:
                    c = proximo_char();
                    if(c == '/' && d == '*'){
                        estado = 3;
                    } else if (c == '/' && d != '*'){
                        Atomo1 = numero;
                        return DIVISAO;
                    } else {
                        Atomo1 = "ERRO";
                        return ERRO;
                    }
                break;
                case 3:
                    c = proximo_char();
                    if(c == '*'){
                        numero[i++] = c;
                        estado = 4;
                    } else if(c != '*'){
                        numero[i++] = c;
                        estado = 3;
                    } else{
                        Atomo1 = "ERRO";
                        return ERRO;
                    }
                break;
                case 4:
                    c = proximo_char();
                    if(c == '/'){
                        numero[i++] = c;
                        Atomo1 = numero;
                        return COMENTARIO;
                    } else if (c != '/'){
                        numero[i++] = c;
                        estado = 3;
                    } else{
                        Atomo1 = "ERRO";
                        return ERRO;
                    }
            }
        }
    } else{
        Atomo1 = "ERRO";
        return ERRO;
    }
}



////Esta função reconhece um Atomo não atribuido utilizando máquinas de estado e retornando o Atomo e o Lexeme do atomo não atribuido
Atomo reconhece_n_atrib(char n){
    char c,d;
    retract_char();
    int i = 0;
    char *numero = (char *)calloc( 10 , sizeof(char) );
    numero[i++] = n;
    c = proximo_char();
    if((int) c == 37){
        Atomo1 = numero;
        return RESTO;
    } else if((int) c == 40){
        Atomo1 = numero;
        return ABRE_PAR;
    } else if((int) c == 41){
        Atomo1 = numero;
        return FECHA_PAR;
    } else if((int) c == 42){
        Atomo1 = numero;
        return MULTIPLICACAO;
    } else if((int) c == 43){
        Atomo1 = numero;
        return ADICAO;
    } else if((int) c == 44){
        Atomo1 = numero;
        return VIRGULA;
    } else if((int) c == 45){
        Atomo1 = numero;
        return SUBTRACAO;
    } else if((int) c == 46){
        Atomo1 = numero;
        return PONTO;
    } else if((int) c == 59){
        Atomo1 = numero;
        return PONTO_VIRGULA;
    } else{
        return ERRO;
    }
}


//Main function
void main(int args, char *argv[]) {
    Atomo atomo;
    FILE *f;
    char c, *string;
    int contaAtomo = 0;

    //argv para chamar o arquivo com extensão .ptl
    f = fopen(argv[1], "r");

    while( fgetc(f) != EOF ) {
        contaAtomo++;
    }

    printf("%d\n", contaAtomo);

    //Maloca o contaAtomo na string
    string = (char *)malloc( (contaAtomo+1) * sizeof(char) );
    string[contaAtomo] = '\0';

    fseek(f, 0, SEEK_SET);
    for(int i = 0; i < contaAtomo; i++) {
        string[i] = fgetc(f);
        if (string[i] == '\n')
          printf("string[%d] = <ENTER>\n",i);
        else
          printf("string[%d] = %c\n", i, string[i]);
    }

    buffer = string;

    //Imprime Linha Número da Linha, Atomo ATOMO, Lexeme LEXEME
    do{
         atomo = proximo_token();
         printf("Linha %d atomo %s lexeme %s\n", nlinha, msgAtomo[atomo], Atomo1 );
    } while( atomo != EOS && atomo != ERRO );

    free(string);

    fclose(f);
}
