#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

//Tamanho dos registros constante 64 bytes
#define c1 4
#define c2 30
#define c3 20
#define c4 10

//SIZE_OF registro == 64 bytes
//Numero de registros por paginas de disco == 64

/*Definição dos registros de tamanho fixo e campos de tamanho fixo.
inteiro de 4 bytes numero.
string de 30 bytes str1.
string de 20 bytes str2.
string de 10 bytes data, no formato DD/MM/AAAA. */
typedef struct{
    int numero;
    char str1[30];
    char str2[20];
    char data[10];
} tRegistro;

/*Definição de uma struct auxiliar que guarda informações dos arquivos gerados durante a
execução do programa.
string de 30 bytes para guardar o nome do arquivo criado.
inteiro que guarda o numero de registros guardados em determinado arquivo gerado.
inteiro que guarda se o arquivo está ou não ordenado. */
typedef struct{
    char nomeArquivo[30];
    int numeroRegistros;
    int ordenacao; //1- ordenado, 0-nao ordenado
} tArquivos;

typedef struct {
    FILE * arqEntrada;
    FILE   arqSaida;
    int    numArqEntrada;
} pARQUIVOS;

typedef struct {
    char  **arqEntrada;
    char   arqSaida[30];
    int    numArq;
} ARQUIVOS;

typedef struct noHeap {
    tRegistro * registro;
    struct noHeap * filhoDir;
    struct noHeap * filhoEsq;
} NO_HEAP;

void swap (NO_HEAP vetor[], int i, int j) {
    NO_HEAP aux;
    aux = vetor[i];
    vetor[i] = vetor[j];
    vetor[j] = aux;
}

void heapify(NO_HEAP arr[], int n, int i) {
    int smaller = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l].registro->numero > arr[smaller].registro->numero)
        smaller = l;
    if (r < n && arr[r].registro->numero > arr[smaller].registro->numero)
        smaller = r;
    if (smaller != i)
    {
        swap(arr, i, smaller);
        heapify(arr, n, smaller);
    }
}

void heapSort(NO_HEAP arr[], int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    for (int i = n - 1; i >= 0; i--)
    {
        swap(arr, 0, i);
        heapify(arr, i, 0);
    }
}

/* Função que remove os caracteres que contem lixo(espaços que sobraram dentro das strings de campo) adicionando o
caractere @ a partir do \0 (final da string). Espaços em branco são substituidos por _ para facilitar a visualização
do campo. */
//Como parâmetro é passado um ponteiro para um registro que está na RAM.
void coletorLixo(tRegistro *registro)   //remove os espacos vazios nos campos, adicionando @ a eles.
{
    for(int i=0;i<c2;i++)       //loop que varia até o tamanho do campo 2(30bytes)
   {
       if((*registro).str1[i]=='\0')    //quando acha o \0, todos os caracteres seguintes são substituidos por @
       {
           (*registro).str1[i]='@';
           for(i+1;i<c2;i++)
           {
               (*registro).str1[i]='@';  //@ nos campos vazios
           }
       }
       else
       {
           if((*registro).str1[i]==' ') // Quando acha um espaço em branco, este é substituido por _
           {
               (*registro).str1[i]='_';
           }
       }
   }

   for(int i=0;i<c3;i++)        //mesma lógica, só que o loop varia ate o tamanho do campo 3(20 bytes)
   {
       if((*registro).str2[i]=='\0')
       {
           (*registro).str2[i]='@';
           for(i+1;i<c3;i++)
           {
               (*registro).str2[i]='@';  //@ nos campos vazios
           }
       }
       else
       {
           if((*registro).str2[i]==' ')
           {
               (*registro).str2[i]='_';
           }
       }
   }
}

int atualizarPaginaRAM (tRegistro *vetor, FILE *fp) {
    if(fread(vetor, sizeof(tRegistro), 64, fp))
        return 1;
    else
        return 0;
}

tRegistro *inicializarArqEmRAM (FILE *fp) {
    tRegistro *novaPag;

    if (ftell(fp) == 0)
        fseek(fp, 4096, ftell(fp));
    else {
        printf("ERRO em inicializarArqEmRAM: Stream pointer not in 0.\n");
        return NULL;
    }
    novaPag = (tRegistro *) calloc(64, sizeof(tRegistro));
    return novaPag;
}

int printarRegistros(int numeroRegistros,char *nome)
{
    //numeroRegistros = 6000;
    if(numeroRegistros==0)
    {
        printf("Arquivo vazio.\n");
        return 0;
    }

    FILE *pArquivo;
    tRegistro *pReg;
    tRegistro regTMP;
    //pReg=&regTMP;

    pArquivo=fopen(nome,"rb");
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit (0);
    }
    long int v = 0;
    pReg = inicializarArqEmRAM(pArquivo);
    printf("%ld\n", ftell(pArquivo));
    //Printa cada registro na tela char a char, eliminando o @
    for(int i = 0; i <= (int)(numeroRegistros/64); i++)
    {
        //if(feof(pArquivo))
        //    break;
        atualizarPaginaRAM(pReg, pArquivo);
        for(long int t = 0; t < 64; t++)
        {
            //printf("%ld   %ld  %ld\n",pReg, (pReg + t*v), pReg+1);
            printf("%d   ",pReg[t].numero);
            for(int j=0;pReg[t].str1[j]!='@';j++)
            {
                printf("%c",pReg[t].str1[j]);
            }
            printf("   ");
            for(int j=0;pReg[t].str2[j]!='@';j++)
            {
                printf("%c",pReg[t].str2[j]);
            }
            printf("   ");
            for(int j=0;j<c4;j++)
            {
                printf("%c",pReg[t].data[j]);
            }
            printf("\n");
            if(++v == numeroRegistros)
                break;
        }
        if(v == numeroRegistros)
            break;
    }
    printf("\n\n");
    fwrite(pReg, sizeof(pReg), 64, stdout);
    fclose(pArquivo);
    return 0;
}


void gerarRegistros(int numeroRegistros, tRegistro *pReg,char *nome)
{
    FILE *pArquivo;
    FILE *pArquivoCidades;
    FILE *pArquivoNomes;
    int intRandom;

    pArquivo=fopen(nome,"wb");
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArquivo); // registro de cabeçalho
    for (int k = 0; k < 4095; k++)
        fwrite("@", sizeof(char), 1, pArquivo);

    pArquivoCidades=fopen("cities.txt","rb");
    if (pArquivoCidades == NULL)
    {
        printf("Falha no processamento.\n");
        exit(0);
    }

    pArquivoNomes=fopen("names.txt","rb");
    if (pArquivoNomes == NULL)
    {
        printf("Falha no processamento.\n");
        exit(0);
    }


    for(int i=0;i<numeroRegistros;i++,pReg++)
    {
        //Gerador de numero
        pReg->numero=rand()%10000;


        //Gerador de cidade (campo 2)
        int cont=0;
        int flag=1;

        intRandom=rand()%20580;

        fseek(pArquivoCidades,0,SEEK_SET);
        while ((fgets(pReg->str1, sizeof (pReg->str1), pArquivoCidades) != NULL)&&(flag==1))
        {
            if (cont == intRandom)
            {
            flag=0;
            }
            else
            {
            cont++;
            }
        }
        for(int j=0;j<c2;j++)
        {
            if((pReg->str1[j])=='\r')
            {
                (pReg->str1[j])='\0';
                j=c2;
            }
        }


        //Gerador de nomes (campo 3)
        int cont2=0;
        int flag2=1;

        intRandom=rand()%4945;

        fseek(pArquivoNomes,0,SEEK_SET);
        while ((fgets(pReg->str2, sizeof (pReg->str2), pArquivoNomes) != NULL)&&(flag2==1))
        {
            if (cont2 == intRandom)
            {
            flag2=0;
            }
            else
            {
            cont2++;
            }
        }
        for(int j=0;j<c3;j++)
        {
            if((pReg->str2[j])=='\r')
            {
                (pReg->str2[j])='\0';
                j=c3;
            }
        }

        //Gerador de data de 1900 a 2019 (ano)
        intRandom=rand()%4;
        switch(intRandom)
        {
        case 3:
            pReg->data[0]=3+'0';
            pReg->data[1]=rand()%2 +'0';
            pReg->data[2]='/';
            break;
        default:
            pReg->data[0]=intRandom +'0';
            pReg->data[1]=rand()%10 +'0';
            pReg->data[2]='/';
            break;
        case 0:
            pReg->data[0]=0 +'0';
            pReg->data[1]=(1+rand()%9) +'0';
            pReg->data[2]='/';
        }

        intRandom=rand()%2;
        switch(intRandom)
        {
        case 1:
            pReg->data[3]=1+'0';
            pReg->data[4]=rand()%3 +'0';
            pReg->data[5]='/';
            break;
        default:
            pReg->data[3]=intRandom +'0';
            pReg->data[4]=(1+rand()%9) +'0';
            pReg->data[5]='/';
            break;
        }

        intRandom=(1 + rand()%2);
        switch(intRandom)
        {
        case 2:
            pReg->data[6]='2';
            pReg->data[7]='0';
            pReg->data[8]=(rand()%2)+ '0';
            pReg->data[9]=(rand()%10)+ '0';
            break;
        case 1:
            pReg->data[6]='1';
            pReg->data[7]='9';
            pReg->data[8]= (rand()%10)+ '0';
            pReg->data[9]= (rand()%10)+ '0';
        }
        //Coloca @ nos espacos vazios que contem lixo
        coletorLixo(pReg);
        //loop que varia até encontrar o @ na string. Passa para uppercase todos os caracteres anteriores ao @. Tanto para o campo 2 quanto para o campo 3.
        for(int j=0;(pReg->str1[j])!='@';j++)
        {
            pReg->str1[j]=toupper(pReg->str1[j]);
        }

        for(int j=0;(pReg->str2[j])!='@';j++)
        {
            pReg->str2[j]=toupper(pReg->str2[j]);
        }

        fwrite(pReg,sizeof(tRegistro),1,pArquivo);  //Escreve o registro no arquivo que está sendo gerado.
    }
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);    // Atualiza o registro de cabeçalho.
    fclose(pArquivo);                       //Fecha os arquivos que estão sendo usados.
    fclose(pArquivoCidades);
    fclose(pArquivoNomes);
    printf("Arquivo gerado.\n");
}

/* Função que lê um arquivo gerado durante a execução para a memória RAM.*/
//Parâmetros: ponteiro para registro, nome do arquivo, total de registros que o arquivo em questão contem.
void lerArquivoParaRAM(tRegistro *pReg,char *nome,int totalDeRegistros) //Le para a RAM os registros de um arquivo
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"rb");      //O arquivo pedido é aberto para a leitura.
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fseek(pArquivo,sizeof(char),SEEK_SET);
    fread(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);    //Todos registros são lidos.
    fclose(pArquivo);                                           //O arquivo é fechado.
}


/*Função que grava os registros contidos em RAM para um arquivo */
//Parâmetros: ponteiro para o registro, nome do arquivo que será escrito, total de registros que serão gravados.
void gravaArquivoParaMEM(tRegistro *pReg,char *nome,int totalDeRegistros) //Grava o conteudo de registros em RAM num arquivo
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"wb");      //O arquivo pedido é aberto para escrita.
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArquivo);            //O Registro de cabeçalho é setado para 0.
    fwrite(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);   //Os registros são gravados no arquivo.
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);        //O Registro de cabeçalho é atualizado para 1
    fclose(pArquivo);
}

/* As funções a seguir são utilizadas para auxiliar durante a execução do programa.
Um arquivo chamado totalarquivos.bin é criado quando inicia-se a execução do programa.
Este arquivo contém a struct tArquivos, que possui nome do arquivo, total de registros e ordenação como variáveis.
Todos arquivos gerados durante a execução do programa são guardados no totalarquivos.bin para se ter fácil  acesso às informações sobre eles.*/

/*Função que salva os dados de um arquivo criado no totalarquivos.bin */
//Parâmetros: numero total de registros do arquivo, nome do arquivo, ordenação do arquivo.
void salvarTotalArquivos(int numeroRegistros,char *nome,int ordenacao)      //Adiciona as informacoes de um arquivo criado novo ao arquivo totarquivos.bin
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;            //O ponteiro pArquivo é criado e aponta para a variável arquivo.
    tArquivos arquivo;              //A variável arquivo é criada para armazenar os dados.
    pArquivo=&arquivo;

    strcpy(arquivo.nomeArquivo,nome);           //a variável recebe os dados(nome, número de registros, ordenação)
    arquivo.numeroRegistros=numeroRegistros;
    arquivo.ordenacao=ordenacao;

    pTotalArquivos=fopen("totalarquivos.bin","ab");     //totalarquivos.bin é aberto em append.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite(pArquivo,sizeof(tArquivos),1,pTotalArquivos);        //Os dados do arquivo são escritos.
    fclose(pTotalArquivos);
}

/*Função que retorna a quantidade de registros que certo arquivo tem.*/
//Parâmetro: nome do arquivo.
int lerTotalArquivos(char *nome)        //Entra com o nome do arquivo e retorna A QUANTIDADE DE REGISTROS que o arquivo pedido tem.
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;                  //Uma variavel do tipo arquivo é criada e um ponteiro para ela também
    pArquivo=&arquivo;

    pTotalArquivos=fopen("totalarquivos.bin","rb");     //O arquivo totalarquivos.bin é aberto para leitura, a fim de se procurar o nome passado como parâmetro.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))                //O loop vai até o fim do arquivo, lendo registro por registro até encontrar um com o nome pedido.
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if((strcmp(pArquivo->nomeArquivo,nome))==0)         //quando o nome é encontrado, retorna-se a quantidade de registros do respectivo arquivo.
        {
            fclose(pTotalArquivos);
            return pArquivo->numeroRegistros;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}


/* Está função conta o tanto de registros que o arquivo pedido tem. Diferente da função anterior, está conta diretamente do arquivo,
enquanto a outra le diretamento do totalarquivos.bin*/
//Parâmetro: nome do arquivo.
int contarRegistros(char *nome)     // conta o total de registro que certo arquivo tem
{
    FILE *pArquivo;
    tRegistro *pReg;
    tRegistro registro;         //Variavel e ponteiro para tRegistro são criadas.
    pReg=&registro;
    int contador=0;             //Contador que armazena os registros contados.

    pArquivo=fopen(nome,"rb");      //Abre-se para leitura
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pArquivo))          //Loop que varia até o fim do arquivo.
    {
        fread(pReg,sizeof(tRegistro),1,pArquivo);           //Arquivo é lido e contador é somado de +1.
        contador++;
    }
    fclose(pArquivo);
    return contador-1;          //retorna contador-1 para descontar a ultima tentativa de leitura.
}

/*Função que checa no totalarquivos.bin se o arquivo pedido está ou não ordenado.*/
//Parâmetro: nome do arquivo.
int checarOrdenacao(char *nome) {   //Entra com o nome do arquivo e retorna se o arquivo esta ordenado ou nao.
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;              //Ponteiro e variável tArquivos são criados.
    pArquivo=&arquivo;

    pTotalArquivos=fopen("totalarquivos.bin","rb");     //Arquivo totalarquivos.bin é aberto para leitura.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))                //Loop que varia até encontrar um arquivo com o nome igual ao passado como parâmetro.
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if(((strcmp(pArquivo->nomeArquivo,nome))==0)&&(pArquivo->ordenacao==1))         //Se encontrar e o arquivo está ordenado, retorna 1, significando que está ordenado.
        {
            fclose(pTotalArquivos);
            return 1;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}

/*Função de comparação utilizada pelo qsort para comparar dois elementos e checar qual é maior ou menor*/
//Parâmetros: dois elementos que serão comparados.
int cmpFunc (const void * a, const void * b) {
    const tRegistro *p1 = a;
    const tRegistro *p2 = b;

    if (p1->numero > p2->numero)  //compara o numero
        return(+1);
    else if (p1->numero < p2->numero)
        return(-1);
    else if (memcmp(p1->str1,p2->str1,30) > 0)  //compara a string 1
        return(+1);
    else if (memcmp(p1->str1,p2->str1,30) < 0)
        return(-1);
    else if(memcmp(p1->str2,p2->str2,20) > 0)  // compara a string 2
        return(+1);
    else if(memcmp(p1->str2,p2->str2,20) < 0)
        return(-1);
    else if(atoi(&p1->data[6])>atoi(&p2->data[6]))  //compara o ano
        return(+1);
    else if(atoi(&p1->data[6])<atoi(&p2->data[6]))
        return(-1);
    else if(atoi(&p1->data[3])>atoi(&p2->data[3]))  //compara o mes
        return(+1);
    else if(atoi(&p1->data[3])<atoi(&p2->data[3]))
        return(-1);
    else if(atoi(&p1->data[0])>atoi(&p2->data[0]))  //compara o dia
        return(+1);
    else if(atoi(&p1->data[0])<atoi(&p2->data[0]))
        return(-1);
}

/*Função que realiza o merging de dois arquivos.*/
//Parâmetros: Nome do arquivo ordenado 1, nome do arquivo ordenado 2, nome do arquivo de saida.
void merging(char *nomeArq1,char *nomeArq2,char *nomeArqSaida) {
    FILE *pArq1;                        //São criados ponteiros para os arquivos, ponteiros e variáveis para tRegistros.
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    pArq1=fopen(nomeArq1,"rb");             //Arquivo 1 e 2 são abertos para leitura e arquivo de saida aberto para escrita.
    if (pArq1 == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    pArq2=fopen(nomeArq2,"rb");
    if (pArq2 == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    pArqSaida=fopen(nomeArqSaida,"wb");
    if (pArqSaida == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArqSaida);           //Registro de cabeçalho do arquivo de saida é setado para 0.
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);     //Arquivo 1 e 2 são lidos depois do primeiro byte(registro de cabeçalho). O primeiro registro de 1 e de 2 são armazenados nos ponteiros para tRegistros 1 e 2 respectivamente.
    fread(pReg2,sizeof(tRegistro),1,pArq2);

        while((!feof(pArq2))&&(!feof(pArq1)))       //Loop que continua até encontrar o fim de um dos arquivos.
        {
            if(pReg1->numero < pReg2->numero)       //O campo 1 dos registros 1 e 2 é comparado, o menor deles é escrito no arquivo de saida e o que foi escrito é substituido pelo próximo registro do arquivo correspondente.
            {
                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                fread(pReg1,sizeof(tRegistro),1,pArq1);
            }
            else
            {
                if(pReg1->numero > pReg2->numero)
                {
                    fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                }
                else
                {
                    if(memcmp(pReg1->str1, pReg2->str1,30) < 0)        //Se os números são iguais, o campo 2 é comparado, o menor deles é escrito no arquivo de saida e o que foi escrito é substituido pelo próximo registro do arquivo correspondente.
                    {
                        fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                        fread(pReg1,sizeof(tRegistro),1,pArq1);
                    }
                    else
                    {
                        if(memcmp(pReg1->str1,pReg2->str1,30) > 0)
                        {
                            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                            fread(pReg2,sizeof(tRegistro),1,pArq2);
                        }
                        else
                        {
                            if(memcmp(pReg1->str2,pReg2->str2,20) < 0)  //Se os campos 2 são iguais, o campo 3 é comparado, o menor deles é escrito no arquivo de saida e o que foi escrito é substituido pelo próximo registro do arquivo correspondente.
                            {
                                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                fread(pReg1,sizeof(tRegistro),1,pArq1);
                            }
                            else
                            {
                                if(memcmp(pReg1->str2,pReg2->str2,20) > 0)
                                {
                                    fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                                }
                                else
                                {
                                    if(atoi(&pReg1->data[6]) < atoi(&pReg2->data[6]))       //Se os campos 3 são iguais, o campo 4 é comparado, primeiramente por ano, depois por mês, depois por dia, o menor deles é escrito no arquivo de saida e o que foi escrito é substituido pelo próximo registro do arquivo correspondente.
                                    {
                                        fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                        fread(pReg1,sizeof(tRegistro),1,pArq1);
                                    }
                                    else
                                    {
                                        if(atoi(&pReg1->data[6]) > atoi(&pReg2->data[6]))
                                        {
                                            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                            fread(pReg2,sizeof(tRegistro),1,pArq2);
                                        }
                                        else
                                        {
                                            if(atoi(&pReg1->data[3]) < atoi(&pReg2->data[3]))
                                            {
                                                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                                fread(pReg1,sizeof(tRegistro),1,pArq1);
                                            }
                                            else
                                            {
                                                if(atoi(&pReg1->data[3]) > atoi(&pReg2->data[3]))
                                                {
                                                    fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                }
                                                else
                                                {
                                                    if(atoi(&pReg1->data[0]) < atoi(&pReg2->data[0]))
                                                    {
                                                        fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                                        fread(pReg1,sizeof(tRegistro),1,pArq1);
                                                    }
                                                    else
                                                    {
                                                        if(atoi(&pReg1->data[0]) > atoi(&pReg2->data[0]))
                                                        {
                                                            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                                            fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                        }
                                                        else
                                                        {   //se os registros são totalmente iguais, grava apenas 1, e os dois são substituidos pelos próximos registros correspondentes de cada arquivo.
                                                            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                                            fread(pReg1,sizeof(tRegistro),1,pArq1);
                                                            fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } // Assim que um dos arquivo termina, precisa-se terminar de ler o que não atingiu o fim do arquivo ainda. Caso o arquivo 1 não tenha terminado, entra no primeiro loop e escreve o resto do arquivo 1.
        while(!feof(pArq1))
        {
            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
            fread(pReg1,sizeof(tRegistro),1,pArq1);
        }
        while(!feof(pArq2)) //Caso o arquivo 2 não tenha terminado, entra no segundo loop e escreve o resto do arquivo 2.
        {
            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
            fread(pReg2,sizeof(tRegistro),1,pArq2);
        }

    fclose(pArq1);      //Os arquivos são fechados
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArqSaida); //O registro de cabeçalho do arquivo de saída é atualizado e ele também é fechado.
    fclose(pArqSaida);
}

/*Função que realiza o matching de dois arquivos.*/
//Parâmetros: Nome do arquivo 1, nome do arquivo 2, nome do arquivo de saida, quantidade de registros do arquivo 1, quantidade de registros do arquivo 2.
void matching(char *nomeArq1,char *nomeArq2,char *nomeArqSaida,int qtdReg1,int qtdReg2) {
    FILE *pArq1;                //Ponteiros para os arquivos, variáveis e ponteiros para tRegistros são criadas.
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    tRegistro *pRegAux;     //Uma variável e ponteiro para tRegistros é criada para auxiliar.
    tRegistro regAux;
    pRegAux=&regAux;
    pRegAux->numero=-1;

    int i1=0;       //Contadores para a leitura dos arquivos, cada vez que o arquivo 1 é lido, i1 recebe i1+1. Mesma coisa para o i2 e arquivo 2.
    int i2=0;

    pArq1=fopen(nomeArq1,"rb");     //Os arquivos 1 e 2 são abertos para leitura.
    if (pArq1 == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    pArq2=fopen(nomeArq2,"rb");
    if (pArq2 == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    pArqSaida=fopen(nomeArqSaida,"wb");     //O arquivo de saída é aberto para escrita, registro de cabeçalho é setado para 0.
    if (pArqSaida == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArqSaida);
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);     //Os primeiros registros do arquivo 1 e arquivos 2 são lidos para pReg1 e pReg2. i1 e i2 são somado de 1.
    i1++;
    fread(pReg2,sizeof(tRegistro),1,pArq2);
    i2++;

    //Cada vez que um registro for escrito no arquivo de saída, o registro auxiliar (pRegAux) recebe o valor do campo 1 como memória, para poder ser comparado com os registros posteriores, que estarão em pReg1 e pReg2.
    while((i1<=qtdReg1)&&(i2<=qtdReg2)) //Loop que varia até atingir o fim de um dos arquivos, ou seja, o arquivo vai ser lido até o total de registros contidos.
    {
        if(pReg1->numero<pReg2->numero)     //Se o número(número refere-se ao campo 1) do registro 1 for menor que o do registro 2, o número do registro 1 é comparado com o número do registro auxiliar, que foi escrito anteriormente, para saber se há insercção entre 1 o número escrito anteriormente.
        {
            if(pReg1->numero==pRegAux->numero) //Se são iguais(há intersecção entre 1 e o registro escrito anteriormente), o número do registro auxiliar recebe o número do registro 1. O registro 1 é escrito no arquivo de saida. O próximo registro do arquivo 1 é lido para pReg1.
            {
                pRegAux->numero=pReg1->numero;
                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
            else
            {           //Se o número do registro auxiliar não for igual ao do registro 1(não há intersecção entre 1 e o número escrito anteriormente), significa que a intersecção com o número contido no registro auxiliar acabou, então pode ser lido o próximo registro para pReg1.
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
        }
        else
        {
            if(pReg1->numero>pReg2->numero)     //O mesmo processo é feito no if a seguir. Este realiza a comparação caso o registro 2 for menor.
            {
                if(pReg2->numero==pRegAux->numero)  //Compara registro 2 com o registro auxiliar. Se for igual, significa que a intersecção não acabou, o registro 2 é escrito e o próximo registro é lido do arquivo 2. O registro auxiliar recebe o registro 2 que foi escrito.
                {
                    pRegAux->numero=pReg2->numero;
                    fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                    i2++;
                }
                else
                {
                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                    i2++;
                }
            }       //Se os números são iguais, significa que há intersecção entre eles, então o menor entre eles é escolhido pelos if's a seguir e é escrito no arquivo de saída. O registro auxiliar recebe o valor escrito como memória para saber se a intersecção continua para os registros posteriores.
            else
            {
                if(memcmp(pReg1->str1,pReg2->str1,30)<0)        //Compara campo 2
                {
                    pRegAux->numero=pReg1->numero;
                    fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                    fread(pReg1,sizeof(tRegistro),1,pArq1);
                    i1++;
                }
                else
                {
                    if(memcmp(pReg1->str1,pReg2->str1,30)>0)
                    {
                        pRegAux->numero=pReg2->numero;
                        fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                        fread(pReg2,sizeof(tRegistro),1,pArq2);
                        i2++;
                    }
                    else
                    {
                        if(memcmp(pReg1->str2,pReg2->str2,20)<0)   //Compara campo 3
                        {
                            pRegAux->numero=pReg1->numero;
                            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                            fread(pReg1,sizeof(tRegistro),1,pArq1);
                            i1++;
                        }
                        else
                        {
                            if(memcmp(pReg1->str2,pReg2->str2,20)>0)
                            {
                                pRegAux->numero=pReg2->numero;
                                fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                fread(pReg2,sizeof(tRegistro),1,pArq2);
                                i2++;
                            }
                            else
                            {
                                if(atoi(&pReg1->data[6])<atoi(&pReg2->data[6])) //Compara campo 4, primeiramente o ano.
                                {
                                    pRegAux->numero=pReg1->numero;
                                    fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                    fread(pReg1,sizeof(tRegistro),1,pArq1);
                                    i1++;
                                }
                                else
                                {
                                    if(atoi(&pReg1->data[6])>atoi(&pReg2->data[6]))
                                    {
                                        pRegAux->numero=pReg2->numero;
                                        fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                        fread(pReg2,sizeof(tRegistro),1,pArq2);
                                        i2++;
                                    }
                                    else
                                    {
                                        if(atoi(&pReg1->data[3])<atoi(&pReg2->data[3])) //Compara campo 4, compara o mês.
                                        {
                                            pRegAux->numero=pReg1->numero;
                                            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                            fread(pReg1,sizeof(tRegistro),1,pArq1);
                                            i1++;
                                        }
                                        else
                                        {
                                            if(atoi(&pReg1->data[3])>atoi(&pReg2->data[3]))
                                            {
                                                pRegAux->numero=pReg2->numero;
                                                fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                                fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                i2++;
                                            }
                                            else
                                            {
                                                if(atoi(&pReg1->data[0])<atoi(&pReg2->data[0])) //Compara campo 4, compara o dia.
                                                {
                                                    pRegAux->numero=pReg1->numero;
                                                    fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                                    fread(pReg1,sizeof(tRegistro),1,pArq1);
                                                    i1++;
                                                }
                                                else
                                                {
                                                    if(atoi(&pReg1->data[0])>atoi(&pReg2->data[0]))
                                                    {
                                                        pRegAux->numero=pReg2->numero;
                                                        fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                                        fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                        i2++;
                                                    }
                                                    else
                                                    {
                                                        //Se os registros são totalmente iguais,o registro auxiliar recebe o registro 1, escreve-se somente 1, e le os próximos dos respectivos arquivos.
                                                        pRegAux->numero=pReg1->numero;
                                                        fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                                        fread(pReg2,sizeof(tRegistro),1,pArq2);
                                                        fread(pReg1,sizeof(tRegistro),1,pArq1);
                                                        i2++;
                                                        i1++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    } //fim while do i1 e i2

    //Quando um dos arquivos termina, precisa-se terminar de escrever o que não terminou. Porém é necessário escrever somente os números que pertencem a intersecção, este número está
    //armazenado no registro auxiliar. Então entra-se em um dos dois loops dependendo do arquivo que não terminou, e só é escrito no arquivo de saída os próximos valores que são iguais
    //ao número armazenado no registro auxiliar(Que pertencem a intersecção).
    while(i1<=qtdReg1)
    {
        if(pRegAux->numero==pReg1->numero)
        {
            pRegAux->numero=pReg1->numero;
            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
            fread(pReg1,sizeof(tRegistro),1,pArq1);
            i1++;
        }
        else
        {
            i1++;
        }
    }

    while(i2<=qtdReg2)
    {
        if(pRegAux->numero==pReg2->numero)
        {
            pRegAux->numero=pReg2->numero;
            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
            fread(pReg2,sizeof(tRegistro),1,pArq2);
            i2++;
        }
        else
        {
            i2++;
        }
    }

    fclose(pArq1);      //Os arquivos são fechados.
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);        //O arquivo de saída sofre alteração no registro de cabeçalho para 1 e é fechado.
    fwrite("1",sizeof(char),1,pArqSaida);
    fclose(pArqSaida);

}

int nomeArquivos(ARQUIVOS **parq) {
    char *entrada, *scanI, *pch, **pString;
    ARQUIVOS *arq;
    arq = (ARQUIVOS *) malloc(sizeof(ARQUIVOS));
    arq->arqEntrada = (char **) calloc(3002, sizeof(char *));
    entrada = (char *) malloc(3002*30*sizeof(char));

    if(arq == NULL || parq == NULL)
        exit(-1);

    scanI = entrada;
    arq->numArq = 0;

    fgets (entrada, (3002 * 30 * sizeof(char)), stdin);
    pch = strtok (entrada, " ");

    for (int b = 0; b < 3002; b++)
    {
        if ((scanI == NULL) || (pch == NULL))
            break;
        strcpy(scanI, pch);
        arq->arqEntrada[b] = (char *) malloc((strlen(pch) + 1)*sizeof(char));
        strcpy(arq->arqEntrada[b],scanI);
        pch = strtok (NULL, " ");
        arq->numArq += 1;
    }
    pString = (char **) calloc((arq->numArq) - 1, sizeof(char *));
    for(int l = 0; l < arq->numArq; l++) {
        if (l == ((arq->numArq) - 1))
            strcpy(arq->arqSaida, arq->arqEntrada[l]);
        else {
            pString[l] = arq->arqEntrada[l];
            printf("%d: %s\n", l, pString[l]);
        }
    }
    printf("%s\n", arq->arqSaida);
    free(arq->arqEntrada);
    arq->arqEntrada = pString;
    free(entrada);
    arq->numArq -= 1;
    printf("%d\n", arq->numArq);
    *parq = arq;
    return 0;
}

void multiwaymerge(char *nomeArquivos) {
    //pARQUIVOS *ponteiroArquivos = abrirArquivos(nomeArquivos);


}



int main () {
    srand(time(NULL));
    int flag=1;     //Váriavel que controla o loop da execução do programa
    FILE *pf;
    char strEntrada[30];        //Strings de entrada(input).
    char strEntrada2[30];
    char strEntrada3[30];
    int intEntrada;             //inteiro de entrada(input).
    int Nregistros;             //numero de registros que terá no arquivo.

    //abertura do totalarquivos.bin para limpar o conteudo, ou criar
    FILE *pTotalArquivos;
    pTotalArquivos=fopen("totalarquivos.bin","wb");
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fclose(pTotalArquivos);
    printf("Batata_main_0\n");

    //execucao
    while(flag==1)
    {
        int intEntrada = 9;
        while (intEntrada != 0)
        {
            intEntrada = 9;
            printf("Digite a intEntrada a ser executada:\n\n");
            printf("1-gerar arquivo/2-printar arquivo/3-ordenar arquivo/4-merging/5-matching/0-sair?");
            scanf("%d",&intEntrada);

            if (intEntrada == 1) {
                printf("Qual o nome do arquivo que sera gerado?");
                scanf("%s",strEntrada);
                printf("Quantos registros terao o arquivo?");
                scanf("%d",&Nregistros);
                salvarTotalArquivos(Nregistros,strEntrada,0);           //O arquivo gerado é armazenado no totalarquivos.bin
                tRegistro *pRegistro = calloc(Nregistros,sizeof(tRegistro));    //Um ponteiro é gerado para a quantidade de registros que será gerada.
                gerarRegistros(Nregistros,pRegistro,strEntrada);
                free(pRegistro);
            }
            if (intEntrada == 2) {
                printf("Qual o nome do arquivo que sera lido?");
                scanf("%s",strEntrada);
                printf("%d\n",lerTotalArquivos(strEntrada));
                printarRegistros(lerTotalArquivos(strEntrada),strEntrada);
            }
            if (intEntrada == 3) {
                printf("Qual o nome do arquivo que sera ordenado?");
                scanf("%s",strEntrada);
                intEntrada=lerTotalArquivos(strEntrada); //Conta quantos registros tem no arquivo que será ordenado.
                tRegistro *pRegistro=calloc(intEntrada,sizeof(tRegistro));

                lerArquivoParaRAM(pRegistro,strEntrada,intEntrada);     //Le o arquivo para a RAM

                qsort(pRegistro, intEntrada, sizeof(tRegistro), cmpFunc); //Ordena o arquivo.

                printf("Qual o nome do arquivo novo?");
                scanf("%s",strEntrada);
                gravaArquivoParaMEM(pRegistro,strEntrada,intEntrada);       //Grava o arquivo ordenado.
                free(pRegistro);

                salvarTotalArquivos(intEntrada,strEntrada,1);           //salva o arquivo gerado no totalarquivos.bin
                printf("Arquivo de dados de entrada ordenado.\n");
                printf("Arquivo gerado.\n");
            }
            if (intEntrada == 4) {
                printf("Qual o nome do primeiro arquivo?");
                scanf("%s",strEntrada);
                if(checarOrdenacao(strEntrada))     //checa se o arquivo 1 está ordenado.
                {
                    printf("Qual o nome do segundo arquivo?");
                    scanf("%s",strEntrada2);
                    if(checarOrdenacao(strEntrada2))        //checa se o arquivo 2 está ordenado.
                    {
                        printf("Qual o nome do arquivo gerado?");
                        scanf("%s",strEntrada3);
                        merging(strEntrada,strEntrada2,strEntrada3);        //Realiza o merging
                        salvarTotalArquivos(contarRegistros(strEntrada3),strEntrada3,1);        //salva o arquivo gerado no totalarquivos.bin
                        printf("Arquivo gerado.\n");
                    }
                    else
                    {
                        printf("Arquivo nao esta ordenado ou nao existe.\n");
                    }
                }
                else
                {
                    printf("Arquivo nao esta ordenado ou nao existe.\n");
                }
            }
            if (intEntrada == 5) {
                printf("Qual o nome do primeiro arquivo?");
                scanf("%s",strEntrada);
                if(checarOrdenacao(strEntrada))  //checa se o arquivo 1 está ordenado.
                {
                    printf("Qual o nome do segundo arquivo?");
                    scanf("%s",strEntrada2);
                    if(checarOrdenacao(strEntrada2))        //checa se o arquivo 2 está ordenado.
                    {
                        printf("Qual o nome do arquivo gerado?");
                        scanf("%s",strEntrada3);
                        matching(strEntrada,strEntrada2,strEntrada3,contarRegistros(strEntrada),contarRegistros(strEntrada2));  //realiza o matching
                        salvarTotalArquivos(contarRegistros(strEntrada3),strEntrada3,1);        //salva o arquivo gerado no totalarquivos.bin
                        printf("Arquivo gerado.\n");
                    }
                    else
                    {
                        printf("Arquivo nao esta ordenado ou nao existe.\n");
                    }
                }
                else
                {
                    printf("Arquivo nao esta ordenado ou nao existe.\n");
                }
            }
            if (intEntrada == 6) {
                getchar();
                ARQUIVOS *arquivo;
                printf("Digite os nomes dos arquivos\n");
                printf("BAtata_-2");
                nomeArquivos(&arquivo);
                printf("BAtata_-1");

            }
            if (intEntrada == 0) {
                flag = 0;
                break;
            }

            if (intEntrada == 9) {
                scanf("%s",strEntrada);
                printf("%d",contarRegistros(strEntrada));
            }

        }
    }
   return(0);
}
