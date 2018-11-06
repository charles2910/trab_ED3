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

typedef struct{
    int numero;
    char str1[30];
    char str2[20];
    char data[10];
} tRegistro;

typedef struct{
    char nomeArquivo[30];
    int numeroRegistros;
    int ordenacao; //1- ordenado, 0-nao ordenado
} tArquivos;

//remove os espacos vazios nos campos, adicionando @ a eles.
void coletorLixo(tRegistro *registro)
{
    for(int i=0;i<c2;i++)
   {
       if((*registro).str1[i]=='\0')
       {
           (*registro).str1[i]='@';
           for(i+1;i<c2;i++)
           {
               (*registro).str1[i]='@';  //@ nos campos vazios
           }
       }
       else
       {
           if((*registro).str1[i]==' ')
           {
               (*registro).str1[i]='_';
           }
       }
   }

   for(int i=0;i<c3;i++)
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
        //Passar para uppercase
        for(int j=0;(pReg->str1[j])!='@';j++)
        {
            pReg->str1[j]=toupper(pReg->str1[j]);
        }

        for(int j=0;(pReg->str2[j])!='@';j++)
        {
            pReg->str2[j]=toupper(pReg->str2[j]);
        }

        fwrite(pReg,sizeof(tRegistro),1,pArquivo);
    }
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);
    fclose(pArquivo);
    fclose(pArquivoCidades);
    fclose(pArquivoNomes);
    printf("Arquivo gerado.\n");
}

//Le para a RAM os registros de um arquivo
void lerArquivoParaRAM(tRegistro *pReg,char *nome,int totalDeRegistros)
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"rb");
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fseek(pArquivo,sizeof(char),SEEK_SET);
    fread(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);
    fclose(pArquivo);
}
//Grava o conteudo de registros em RAM num arquivo
void gravaArquivoParaMEM(tRegistro *pReg,char *nome,int totalDeRegistros)
{
    FILE *pArquivo;
    pArquivo=fopen(nome,"wb");
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite("0",sizeof(char),1,pArquivo);
    fwrite(pReg,sizeof(tRegistro),totalDeRegistros,pArquivo);
    fseek(pArquivo,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArquivo);
    fclose(pArquivo);
}

//Adiciona as informacoes de um arquivo criado novo ao arquivo totarquivos
void salvarTotalArquivos(int numeroRegistros,char *nome,int ordenacao)
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;
    pArquivo=&arquivo;

    strcpy(arquivo.nomeArquivo,nome);
    arquivo.numeroRegistros=numeroRegistros;
    arquivo.ordenacao=ordenacao;

    pTotalArquivos=fopen("totalarquivos.bin","ab");
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fwrite(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
    fclose(pTotalArquivos);
}

//Entra com o nome do arquivo e retorna A QUANTIDADE DE REGISTROS que o arquivo pedido tem.
int lerTotalArquivos(char *nome)
{
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;
    pArquivo=&arquivo;
    int flag=1;

    pTotalArquivos=fopen("totalarquivos.bin","rb");
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if((strcmp(pArquivo->nomeArquivo,nome))==0)
        {
            fclose(pTotalArquivos);
            return pArquivo->numeroRegistros;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}

// conta o total de registro que certo arquivo tem
int contarRegistros(char *nome)
{
    FILE *pArquivo;
    tRegistro *pReg;
    tRegistro registro;
    pReg=&registro;
    int contador=0;

    pArquivo=fopen(nome,"rb");
    if (pArquivo == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pArquivo))
    {
        fread(pReg,sizeof(tRegistro),1,pArquivo);
        contador++;
    }
    fclose(pArquivo);
    return contador-1;
}

//Entra com o nome do arquivo e retorna se o arquivo esta ordenado ou nao.
int checarOrdenacao(char *nome) {
    FILE *pTotalArquivos;
    tArquivos *pArquivo;
    tArquivos arquivo;
    pArquivo=&arquivo;
    int flag=1;

    pTotalArquivos=fopen("totalarquivos.bin","rb");
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }

    while(!feof(pTotalArquivos))
    {
        fread(pArquivo,sizeof(tArquivos),1,pTotalArquivos);
        if(((strcmp(pArquivo->nomeArquivo,nome))==0)&&(pArquivo->ordenacao==1))
        {
            fclose(pTotalArquivos);
            return 1;
        }
    }
    fclose(pTotalArquivos);
    return 0;
}

int cmpFunc (const void * a, const void * b) {
    const tRegistro *p1 = a;
    const tRegistro *p2 = b;

    if (p1->numero > p2->numero)  //compara o numero
        return(+1);
    else if (p1->numero < p2->numero)
        return(-1);
    else if (strcmp(p1->str1,p2->str1) > 0)  //compara a string 1
        return(+1);
    else if (strcmp(p1->str1,p2->str1) < 0)
        return(-1);
    else if(strcmp(p1->str2,p2->str2) > 0)  // compara a string 2
        return(+1);
    else if(strcmp(p1->str2,p2->str2) < 0)
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

void merging(char *nomeArq1,char *nomeArq2,char *nomeArqSaida) {
    FILE *pArq1;
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    pArq1=fopen(nomeArq1,"rb");
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
    fwrite("0",sizeof(char),1,pArqSaida);
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);
    fread(pReg2,sizeof(tRegistro),1,pArq2);

        while((!feof(pArq2))&&(!feof(pArq1)))
        {
            if(pReg1->numero < pReg2->numero)
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
                    if(strcmp(pReg1->str1, pReg2->str1) < 0)
                    {
                        fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                        fread(pReg1,sizeof(tRegistro),1,pArq1);
                    }
                    else
                    {
                        if(strcmp(pReg1->str1,pReg2->str1) > 0)
                        {
                            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                            fread(pReg2,sizeof(tRegistro),1,pArq2);
                        }
                        else
                        {
                            if(strcmp(pReg1->str2,pReg2->str2) < 0)
                            {
                                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                                fread(pReg1,sizeof(tRegistro),1,pArq1);
                            }
                            else
                            {
                                if(strcmp(pReg1->str2,pReg2->str2) > 0)
                                {
                                    fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                    fread(pReg2,sizeof(tRegistro),1,pArq2);
                                }
                                else
                                {
                                    if(atoi(&pReg1->data[6]) < atoi(&pReg2->data[6]))
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
                                                        {   //se sao totalmente iguais, grava apenas 1
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
        } // fim do while
        while(!feof(pArq1))
        {
            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
            fread(pReg1,sizeof(tRegistro),1,pArq1);
        }
        while(!feof(pArq2))
        {
            fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
            fread(pReg2,sizeof(tRegistro),1,pArq2);
        }

    fclose(pArq1);
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArqSaida);
    fclose(pArqSaida);
}

void matching(char *nomeArq1,char *nomeArq2,char *nomeArqSaida,int qtdReg1,int qtdReg2) {
    FILE *pArq1;
    FILE *pArq2;
    FILE *pArqSaida;

    tRegistro *pReg1;
    tRegistro reg1;
    pReg1=&reg1;

    tRegistro *pReg2;
    tRegistro reg2;
    pReg2=&reg2;

    tRegistro *pRegAux;
    tRegistro regAux;
    pRegAux=&regAux;
    pRegAux->numero=-1;

    int i1=0;
    int i2=0;

    pArq1=fopen(nomeArq1,"rb");
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
    fwrite("0",sizeof(char),1,pArqSaida);
    fseek(pArq1,sizeof(char),SEEK_SET);
    fseek(pArq2,sizeof(char),SEEK_SET);

    fread(pReg1,sizeof(tRegistro),1,pArq1);
    i1++;
    fread(pReg2,sizeof(tRegistro),1,pArq2);
    i2++;

    while((i1<=qtdReg1)&&(i2<=qtdReg2))
    {
        if(pReg1->numero<pReg2->numero)
        {
            if(pReg1->numero==pRegAux->numero)
            {
                pRegAux->numero=pReg1->numero;
                fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
            else
            {
                fread(pReg1,sizeof(tRegistro),1,pArq1);
                i1++;
            }
        }
        else
        {
            if(pReg1->numero>pReg2->numero)
            {
                if(pReg2->numero==pRegAux->numero)
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
            }
            else
            {
                if(strcmp(pReg1->str1,pReg2->str1)<0)
                {
                    pRegAux->numero=pReg1->numero;
                    fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                    fread(pReg1,sizeof(tRegistro),1,pArq1);
                    i1++;
                }
                else
                {
                    if(strcmp(pReg1->str1,pReg2->str1)>0)
                    {
                        pRegAux->numero=pReg2->numero;
                        fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                        fread(pReg2,sizeof(tRegistro),1,pArq2);
                        i2++;
                    }
                    else
                    {
                        if(strcmp(pReg1->str2,pReg2->str2)<0)
                        {
                            pRegAux->numero=pReg1->numero;
                            fwrite(pReg1,sizeof(tRegistro),1,pArqSaida);
                            fread(pReg1,sizeof(tRegistro),1,pArq1);
                            i1++;
                        }
                        else
                        {
                            if(strcmp(pReg1->str2,pReg2->str2)>0)
                            {
                                pRegAux->numero=pReg2->numero;
                                fwrite(pReg2,sizeof(tRegistro),1,pArqSaida);
                                fread(pReg2,sizeof(tRegistro),1,pArq2);
                                i2++;
                            }
                            else
                            {
                                if(atoi(&pReg1->data[6])<atoi(&pReg2->data[6]))
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
                                        if(atoi(&pReg1->data[3])<atoi(&pReg2->data[3]))
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
                                                if(atoi(&pReg1->data[0])<atoi(&pReg2->data[0]))
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

    fclose(pArq1);
    fclose(pArq2);
    fseek(pArqSaida,0,SEEK_SET);
    fwrite("1",sizeof(char),1,pArqSaida);
    fclose(pArqSaida);

}



void multiwaymerge(char *nomeArquivos) {
    

}



int main () {
    srand(time(NULL));
    int flag=1;
    FILE *pf;
    char strEntrada[30];
    char strEntrada2[30];
    char strEntrada3[30];
    char strEntradaK[30][30];
    int intEntrada;
    int Nregistros;
    int contador=0;
    int flagOrd=1;

    //abertura do totalarquivos para limpar o conteudo, ou criar

    FILE *pTotalArquivos;
    pTotalArquivos=fopen("totalarquivos.bin","wb");
    if (pTotalArquivos == NULL)
    {
    printf("Falha no processamento.\n");
    exit(0);
    }
    fclose(pTotalArquivos);

    //execucao
    while(flag==1)
    {
        printf("1-gerar arquivo/2-printar arquivo/3-ordenar arquivo/4-merging/5-matching/0-sair?");
        scanf("%d",&intEntrada);
        switch(intEntrada)
        {
        case 1:
            printf("Qual o nome do arquivo que sera gerado?");
            scanf("%s",strEntrada);
            printf("Quantos registros terao o arquivo?");
            scanf("%d",&Nregistros);
            salvarTotalArquivos(Nregistros,strEntrada,0);
            tRegistro *pRegistro = calloc(Nregistros,sizeof(tRegistro));
            gerarRegistros(Nregistros,pRegistro,strEntrada);
            free(pRegistro);
            break;
        case 2:
            printf("Qual o nome do arquivo que sera lido?");
            scanf("%s",strEntrada);
            printf("%d\n",lerTotalArquivos(strEntrada));
            printarRegistros(lerTotalArquivos(strEntrada),strEntrada);
            break;
        case 3:
            printf("Qual o nome do arquivo que sera ordenado?");
            scanf("%s",strEntrada);
            intEntrada=lerTotalArquivos(strEntrada); //Conta quantos registros tem no arquivo que será ordenado
            pRegistro=calloc(intEntrada,sizeof(tRegistro));

            lerArquivoParaRAM(pRegistro,strEntrada,intEntrada);

            qsort(pRegistro, intEntrada, sizeof(tRegistro), cmpFunc);

            printf("Qual o nome do arquivo novo?");
            scanf("%s",strEntrada);
            gravaArquivoParaMEM(pRegistro,strEntrada,intEntrada);
            free(pRegistro);

            salvarTotalArquivos(intEntrada,strEntrada,1);
            printf("Arquivo de dados de entrada ordenado.\n");
            printf("Arquivo gerado.\n");
            break;
        case 4:
            printf("Qual o nome do primeiro arquivo?");
            scanf("%s",strEntrada);
            if(checarOrdenacao(strEntrada))
            {
                printf("Qual o nome do segundo arquivo?");
                scanf("%s",strEntrada2);
                if(checarOrdenacao(strEntrada2))
                {
                    printf("Qual o nome do arquivo gerado?");
                    scanf("%s",strEntrada3);
                    merging(strEntrada,strEntrada2,strEntrada3);
                    salvarTotalArquivos(contarRegistros(strEntrada3),strEntrada3,1);
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
            break;
        case 5:
            printf("Qual o nome do primeiro arquivo?");
            scanf("%s",strEntrada);
            if(checarOrdenacao(strEntrada))
            {
                printf("Qual o nome do segundo arquivo?");
                scanf("%s",strEntrada2);
                if(checarOrdenacao(strEntrada2))
                {
                    printf("Qual o nome do arquivo gerado?");
                    scanf("%s",strEntrada3);
                    matching(strEntrada,strEntrada2,strEntrada3,contarRegistros(strEntrada),contarRegistros(strEntrada2));
                    salvarTotalArquivos(contarRegistros(strEntrada3),strEntrada3,1);
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
            break;
        case 6:
            break;
        case 0:
            flag=0;
            break;
        case 9:
            scanf("%s",strEntrada);
            printf("%d",contarRegistros(strEntrada));
        }
    }

   return(0);
}
