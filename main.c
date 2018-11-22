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

/*Defini��o dos registros de tamanho fixo e campos de tamanho fixo.
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

/*Defini��o de uma struct auxiliar que guarda informa��es dos arquivos gerados durante a
execu��o do programa.
string de 30 bytes para guardar o nome do arquivo criado.
inteiro que guarda o numero de registros guardados em determinado arquivo gerado.
inteiro que guarda se o arquivo est� ou n�o ordenado. */
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

/* Fun��o que remove os caracteres que contem lixo(espa�os que sobraram dentro das strings de campo) adicionando o
caractere @ a partir do \0 (final da string). Espa�os em branco s�o substituidos por _ para facilitar a visualiza��o
do campo. */
//Como par�metro � passado um ponteiro para um registro que est� na RAM.
void coletorLixo(tRegistro *registro)   //remove os espacos vazios nos campos, adicionando @ a eles.
{
    for(int i=0;i<c2;i++)       //loop que varia at� o tamanho do campo 2(30bytes)
   {
       if((*registro).str1[i]=='\0')    //quando acha o \0, todos os caracteres seguintes s�o substituidos por @
       {
           (*registro).str1[i]='@';
           for(i+1;i<c2;i++)
           {
               (*registro).str1[i]='@';  //@ nos campos vazios
           }
       }
       else
       {
           if((*registro).str1[i]==' ') // Quando acha um espa�o em branco, este � substituido por _
           {
               (*registro).str1[i]='_';
           }
       }
   }

   for(int i=0;i<c3;i++)        //mesma l�gica, s� que o loop varia ate o tamanho do campo 3(20 bytes)
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
    fwrite("0",sizeof(char),1,pArquivo); // registro de cabe�alho
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
        //loop que varia at� encontrar o @ na string. Passa para uppercase todos os caracteres anteriores ao @. Tanto para o campo 2 quanto para o campo 3.
        for(int j=0;(pReg->str1[j])!='@';j++)
        {
            pReg->str1[j]=toupper(pReg->str1[j]);
        }

        for(int j=0;(pReg->str2[j])!='@';j++)
        {
            pReg->str2[j]=toupper(pReg->str2[j]);
        }

        fwrite(pReg,sizeof(tRegistro),1,pArquivo);  //Escreve o registro no arquivo que est� sendo gerado.
    }
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);    // Atualiza o registro de cabe�alho.
    fclose(pArquivo);                       //Fecha os arquivos que est�o sendo usados.
    fclose(pArquivoCidades);
    fclose(pArquivoNomes);
    printf("Arquivo gerado.\n");
}

/* Fun��o que l� um arquivo gerado durante a execu��o para a mem�ria RAM.*/
//Par�metros: ponteiro para registro, nome do arquivo, total de registros que o arquivo em quest�o contem.
void lerArquivoParaRAM(tRegistro *pReg,char *nome,int totalDeRegistros) //Le para a RAM os registros de um arquivo
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"rb");      //O arquivo pedido � aberto para a leitura.
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fseek(pArquivo,sizeof(char),SEEK_SET);
    fread(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);    //Todos registros s�o lidos.
    fclose(pArquivo);                                           //O arquivo � fechado.
}


/*Fun��o que grava os registros contidos em RAM para um arquivo */
//Par�metros: ponteiro para o registro, nome do arquivo que ser� escrito, total de registros que ser�o gravados.
void gravaArquivoParaMEM(tRegistro *pReg,char *nome,int totalDeRegistros) //Grava o conteudo de registros em RAM num arquivo
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"wb");      //O arquivo pedido � aberto para escrita.
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArquivo);            //O Registro de cabe�alho � setado para 0.
    fwrite(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);   //Os registros s�o gravados no arquivo.
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);        //O Registro de cabe�alho � atualizado para 1
    fclose(pArquivo);
}

/* As fun��es a seguir s�o utilizadas para auxiliar durante a execu��o do programa.
Um arquivo chamado totalarquivos.bin � criado quando inicia-se a execu��o do programa.
Este arquivo cont�m a struct tArquivos, que possui nome do arquivo, total de registros e ordena��o como vari�veis.
Todos arquivos gerados durante a execu��o do programa s�o guardados no totalarquivos.bin para se ter f�cil  acesso �s informa��es sobre eles.*/

/*Fun��o que salva os dados de um arquivo criado no totalarquivos.bin */
//Par�metros: numero total de registros do arquivo, nome do arquivo, ordena��o do arquivo.
void salvarTotalArquivos(int numeroRegistros,char *nome,int ordenacao)      //Adiciona as informacoes de um arquivo criado novo ao arquivo totarquivos.bin
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;            //O ponteiro pArquivo � criado e aponta para a vari�vel arquivo.
    tArquivos arquivo;              //A vari�vel arquivo � criada para armazenar os dados.
    pArquivo=&arquivo;

    strcpy(arquivo.nomeArquivo,nome);           //a vari�vel recebe os dados(nome, n�mero de registros, ordena��o)
    arquivo.numeroRegistros=numeroRegistros;
    arquivo.ordenacao=ordenacao;

    pTotalArquivos=fopen("totalarquivos.bin","ab");     //totalarquivos.bin � aberto em append.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite(pArquivo,sizeof(tArquivos),1,pTotalArquivos);        //Os dados do arquivo s�o escritos.
    fclose(pTotalArquivos);
}

/*Fun��o que retorna a quantidade de registros que certo arquivo tem.*/
//Par�metro: nome do arquivo.
int lerTotalArquivos(char *nome)        //Entra com o nome do arquivo e retorna A QUANTIDADE DE REGISTROS que o arquivo pedido tem.
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;                  //Uma variavel do tipo arquivo � criada e um ponteiro para ela tamb�m
    pArquivo=&arquivo;

    pTotalArquivos=fopen("totalarquivos.bin","rb");     //O arquivo totalarquivos.bin � aberto para leitura, a fim de se procurar o nome passado como par�metro.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))                //O loop vai at� o fim do arquivo, lendo registro por registro at� encontrar um com o nome pedido.
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if((strcmp(pArquivo->nomeArquivo,nome))==0)         //quando o nome � encontrado, retorna-se a quantidade de registros do respectivo arquivo.
        {
            fclose(pTotalArquivos);
            return pArquivo->numeroRegistros;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}


/* Est� fun��o conta o tanto de registros que o arquivo pedido tem. Diferente da fun��o anterior, est� conta diretamente do arquivo,
enquanto a outra le diretamento do totalarquivos.bin*/
//Par�metro: nome do arquivo.
int contarRegistros(char *nome)     // conta o total de registro que certo arquivo tem
{
    FILE *pArquivo;
    tRegistro *pReg;
    tRegistro registro;         //Variavel e ponteiro para tRegistro s�o criadas.
    pReg=&registro;
    int contador=0;             //Contador que armazena os registros contados.

    pArquivo=fopen(nome,"rb");      //Abre-se para leitura
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pArquivo))          //Loop que varia at� o fim do arquivo.
    {
        fread(pReg,sizeof(tRegistro),1,pArquivo);           //Arquivo � lido e contador � somado de +1.
        contador++;
    }
    fclose(pArquivo);
    return contador-1;          //retorna contador-1 para descontar a ultima tentativa de leitura.
}

/*Fun��o que checa no totalarquivos.bin se o arquivo pedido est� ou n�o ordenado.*/
//Par�metro: nome do arquivo.
int checarOrdenacao(char *nome) {   //Entra com o nome do arquivo e retorna se o arquivo esta ordenado ou nao.
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;              //Ponteiro e vari�vel tArquivos s�o criados.
    pArquivo=&arquivo;

    pTotalArquivos=fopen("totalarquivos.bin","rb");     //Arquivo totalarquivos.bin � aberto para leitura.
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))                //Loop que varia at� encontrar um arquivo com o nome igual ao passado como par�metro.
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if(((strcmp(pArquivo->nomeArquivo,nome))==0)&&(pArquivo->ordenacao==1))         //Se encontrar e o arquivo est� ordenado, retorna 1, significando que est� ordenado.
        {
            fclose(pTotalArquivos);
            return 1;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}

/*Fun��o de compara��o utilizada pelo qsort para comparar dois elementos e checar qual � maior ou menor*/
//Par�metros: dois elementos que ser�o comparados.
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

/*Fun��o que realiza o merging de dois arquivos.*/
//Par�metros: Nome do arquivo ordenado 1, nome do arquivo ordenado 2, nome do arquivo de saida.
void merging(char *nomeArq1,char *nomeArq2,char *nomeArqSaida) {
    FILE *pArq1;                        //S�o criados ponteiros para os arquivos, ponteiros e vari�veis para tRegistros.
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    pArq1=fopen(nomeArq1,"rb");             //Arquivo 1 e 2 s�o abertos para leitura e arquivo de saida aberto para escrita.
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
    fwrite("0",sizeof(char),1,pArqSaida);           //Registro de cabe�alho do arquivo de saida � setado para 0.
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);     //Arquivo 1 e 2 s�o lidos depois do primeiro byte(registro de cabe�alho). O primeiro registro de 1 e de 2 s�o armazenados nos ponteiros para tRegistros 1 e 2 respectivamente.
    fread(pReg2,sizeof(tRegistro),1,pArq2);

        while((!feof(pArq2))&&(!feof(pArq1)))       //Loop que continua at� encontrar o fim de um dos arquivos.
        {
            if(pReg1->numero < pReg2->numero)       //O campo 1 dos registros 1 e 2 � comparado, o menor deles � escrito no arquivo de saida e o que foi escrito � substituido pelo pr�ximo registro do arquivo correspondente.
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
                    if(memcmp(pReg1->str1, pReg2->str1,30) < 0)        //Se os n�meros s�o iguais, o campo 2 � comparado, o menor deles � escrito no arquivo de saida e o que foi escrito � substituido pelo pr�ximo registro do arquivo correspondente.
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
                            if(memcmp(pReg1->str2,pReg2->str2,20) < 0)  //Se os campos 2 s�o iguais, o campo 3 � comparado, o menor deles � escrito no arquivo de saida e o que foi escrito � substituido pelo pr�ximo registro do arquivo correspondente.
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
                                    if(atoi(&pReg1->data[6]) < atoi(&pReg2->data[6]))       //Se os campos 3 s�o iguais, o campo 4 � comparado, primeiramente por ano, depois por m�s, depois por dia, o menor deles � escrito no arquivo de saida e o que foi escrito � substituido pelo pr�ximo registro do arquivo correspondente.
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
                                                        {   //se os registros s�o totalmente iguais, grava apenas 1, e os dois s�o substituidos pelos pr�ximos registros correspondentes de cada arquivo.
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
        } // Assim que um dos arquivo termina, precisa-se terminar de ler o que n�o atingiu o fim do arquivo ainda. Caso o arquivo 1 n�o tenha terminado, entra no primeiro loop e escreve o resto do arquivo 1.
        while(!feof(pArq1))
        {
            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
            fread(pReg1,sizeof(tRegistro),1,pArq1);
        }
        while(!feof(pArq2)) //Caso o arquivo 2 n�o tenha terminado, entra no segundo loop e escreve o resto do arquivo 2.
        {
            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
            fread(pReg2,sizeof(tRegistro),1,pArq2);
        }

    fclose(pArq1);      //Os arquivos s�o fechados
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArqSaida); //O registro de cabe�alho do arquivo de sa�da � atualizado e ele tamb�m � fechado.
    fclose(pArqSaida);
}

/*Fun��o que realiza o matching de dois arquivos.*/
//Par�metros: Nome do arquivo 1, nome do arquivo 2, nome do arquivo de saida, quantidade de registros do arquivo 1, quantidade de registros do arquivo 2.
void matching(char *nomeArq1,char *nomeArq2,char *nomeArqSaida,int qtdReg1,int qtdReg2) {
    FILE *pArq1;                //Ponteiros para os arquivos, vari�veis e ponteiros para tRegistros s�o criadas.
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    tRegistro *pRegAux;     //Uma vari�vel e ponteiro para tRegistros � criada para auxiliar.
    tRegistro regAux;
    pRegAux=&regAux;
    pRegAux->numero=-1;

    int i1=0;       //Contadores para a leitura dos arquivos, cada vez que o arquivo 1 � lido, i1 recebe i1+1. Mesma coisa para o i2 e arquivo 2.
    int i2=0;

    pArq1=fopen(nomeArq1,"rb");     //Os arquivos 1 e 2 s�o abertos para leitura.
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

    pArqSaida=fopen(nomeArqSaida,"wb");     //O arquivo de sa�da � aberto para escrita, registro de cabe�alho � setado para 0.
    if (pArqSaida == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArqSaida);
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);     //Os primeiros registros do arquivo 1 e arquivos 2 s�o lidos para pReg1 e pReg2. i1 e i2 s�o somado de 1.
    i1++;
    fread(pReg2,sizeof(tRegistro),1,pArq2);
    i2++;

    //Cada vez que um registro for escrito no arquivo de sa�da, o registro auxiliar (pRegAux) recebe o valor do campo 1 como mem�ria, para poder ser comparado com os registros posteriores, que estar�o em pReg1 e pReg2.
    while((i1<=qtdReg1)&&(i2<=qtdReg2)) //Loop que varia at� atingir o fim de um dos arquivos, ou seja, o arquivo vai ser lido at� o total de registros contidos.
    {
        if(pReg1->numero<pReg2->numero)     //Se o n�mero(n�mero refere-se ao campo 1) do registro 1 for menor que o do registro 2, o n�mero do registro 1 � comparado com o n�mero do registro auxiliar, que foi escrito anteriormente, para saber se h� inserc��o entre 1 o n�mero escrito anteriormente.
        {
            if(pReg1->numero==pRegAux->numero) //Se s�o iguais(h� intersec��o entre 1 e o registro escrito anteriormente), o n�mero do registro auxiliar recebe o n�mero do registro 1. O registro 1 � escrito no arquivo de saida. O pr�ximo registro do arquivo 1 � lido para pReg1.
            {
                pRegAux->numero=pReg1->numero;
                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
            else
            {           //Se o n�mero do registro auxiliar n�o for igual ao do registro 1(n�o h� intersec��o entre 1 e o n�mero escrito anteriormente), significa que a intersec��o com o n�mero contido no registro auxiliar acabou, ent�o pode ser lido o pr�ximo registro para pReg1.
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
        }
        else
        {
            if(pReg1->numero>pReg2->numero)     //O mesmo processo � feito no if a seguir. Este realiza a compara��o caso o registro 2 for menor.
            {
                if(pReg2->numero==pRegAux->numero)  //Compara registro 2 com o registro auxiliar. Se for igual, significa que a intersec��o n�o acabou, o registro 2 � escrito e o pr�ximo registro � lido do arquivo 2. O registro auxiliar recebe o registro 2 que foi escrito.
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
            }       //Se os n�meros s�o iguais, significa que h� intersec��o entre eles, ent�o o menor entre eles � escolhido pelos if's a seguir e � escrito no arquivo de sa�da. O registro auxiliar recebe o valor escrito como mem�ria para saber se a intersec��o continua para os registros posteriores.
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
                                        if(atoi(&pReg1->data[3])<atoi(&pReg2->data[3])) //Compara campo 4, compara o m�s.
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
                                                        //Se os registros s�o totalmente iguais,o registro auxiliar recebe o registro 1, escreve-se somente 1, e le os pr�ximos dos respectivos arquivos.
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

    //Quando um dos arquivos termina, precisa-se terminar de escrever o que n�o terminou. Por�m � necess�rio escrever somente os n�meros que pertencem a intersec��o, este n�mero est�
    //armazenado no registro auxiliar. Ent�o entra-se em um dos dois loops dependendo do arquivo que n�o terminou, e s� � escrito no arquivo de sa�da os pr�ximos valores que s�o iguais
    //ao n�mero armazenado no registro auxiliar(Que pertencem a intersec��o).
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

    fclose(pArq1);      //Os arquivos s�o fechados.
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);        //O arquivo de sa�da sofre altera��o no registro de cabe�alho para 1 e � fechado.
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



int main (int argc, char *argv[]) {
    srand(time(NULL));
    int flag=1;     //V�riavel que controla o loop da execu��o do programa
    FILE *pf;
    char strEntrada[30];        //Strings de entrada(input).
    char strEntrada2[30];
    char strEntrada3[30];
    int intEntrada;             //inteiro de entrada(input).
    int Nregistros;             //numero de registros que ter� no arquivo.

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
        if ((argc > 1) && (flagArgs)) {
            intEntrada = atoi(argv[1]);
            ARQUIVOS *parquivinho;
            nomeArquivosArgv(&parquivinho, argc, argv);
            flagArgs = 0;
            multiwaymerge(&parquivinho);
        }
        else {
            printf("Digite a opcao a ser executada:\n\n1-gerar arquivo/2-printar arquivo/3-ordenar arquivo/4-merging/5-matching/0-sair?\n");
            scanf("%d",&intEntrada);
            getchar();
        }
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
                salvarTotalArquivos(Nregistros,strEntrada,0);           //O arquivo gerado � armazenado no totalarquivos.bin
                tRegistro *pRegistro = calloc(Nregistros,sizeof(tRegistro));    //Um ponteiro � gerado para a quantidade de registros que ser� gerada.
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
                intEntrada=lerTotalArquivos(strEntrada); //Conta quantos registros tem no arquivo que ser� ordenado.
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
                if(checarOrdenacao(strEntrada))     //checa se o arquivo 1 est� ordenado.
                {
                    printf("Qual o nome do segundo arquivo?");
                    scanf("%s",strEntrada2);
                    if(checarOrdenacao(strEntrada2))        //checa se o arquivo 2 est� ordenado.
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
                if(checarOrdenacao(strEntrada))  //checa se o arquivo 1 est� ordenado.
                {
                    printf("Qual o nome do segundo arquivo?");
                    scanf("%s",strEntrada2);
                    if(checarOrdenacao(strEntrada2))        //checa se o arquivo 2 est� ordenado.
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
