/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				Programa que Simula a execução de um LBA Determinístico para determinadas Cadeias
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define QtdDigNomEst 50 					//Quantidade máxima de digitos do nome do estado
#define TamCadeia 100						//Quantidade máxima de digitos das cadeias a serem executada
#define QtdDigNomeArqIn 100					//Nome do Arquivo de Entrada

struct transicao
{
     char ler;
     char proxEstado[QtdDigNomEst];
     char escreve;
     char mover;
     struct transicao *proxTransicao;
};
typedef struct transicao Transicao;


struct estado 
{
     char nomeEstado[QtdDigNomEst];
     Transicao *transicoes;       
       
};
typedef struct estado Estado;

struct estados
{
       Estado estado;
       struct estados *proxEstado;
};
typedef struct estados Estados;

/*---------------------------------------------------------------------------------------------
										Protótipo das Funções
---------------------------------------------------------------------------------------------*/

void nomeArquivoSaida(char nomeArqEnt[], char nomeArqSai[], int qtdCadeia);
int lerCadeiaArq(char estadoArquivo[],char cadeia[]);
Estados* montaListaEstadosFinais(char estadoArquivo[], Estados *estados);
void lerNomeEstadoInicialArq( char estadoArquivo[], char nomeEstadoInicial[]);
Estados* lerEstadosArquivo(char estadoArquivo[]);
void adicionaTransicaoEstado(char nomeEstado[], Transicao* transicao, Estados** estados);
FILE *lerTransicao(Transicao **transicao, FILE *arq);
Transicao *criaTransicao( char ler, char estadoProxEstado[], char escreve, char mover, Transicao *transicoes);
Estados *criaEstado(char estado[], Transicao *transicoes, Estados *listaEstados );
Transicao* inseriTransicao(Transicao *transicoes, Transicao *nova);
Estados* inseriEstado(Estados *estados, Estado novo);
void execultaLBA(char cadeia[], Estado estadoInicial, Estados *estadosFinais, Estados *estados, char estadoArqOut[]);
int procuraEstado(char nomeEstado[], Estados *estados, Estado *novoEstado);
Transicao *procuraTransicao(char ler, Transicao *transicoes);
void simularLBADeterministico(char *nomeArqIn);

/*---------------------------------------------------------------------------------------------------------------------------------------
                                                        MAIN
----------------------------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{ 
    char *nomeArqIn;
    
	nomeArqIn = malloc(QtdDigNomeArqIn*sizeof(char*));

	if(argc != 2)
	{
		printf("ERRO: faltou uma sentenca como parametro\nUso:\n\t\t%s <string>\nOnde:\n\t\t<string> e uma sentenca a ser analisada.", argv[0]);
		exit(1);
	}	
    
	strcpy(nomeArqIn, argv[1]);
    simularLBADeterministico(nomeArqIn);
    
    printf("\n\tPrograma Execultado Com Sucesso!\n\n");
    
}


/*---------------------------------------------------------------------------------------------
										FUNÇÕES
---------------------------------------------------------------------------------------------*/
void simularLBADeterministico(char *nomeArqIn)
{
	Estados *estados;
	Estado estadoInicial;
	Estados *estadosFinais=NULL;
	char nomeEstadoInicial[QtdDigNomEst];
	char cadeia[TamCadeia]=""; 
	char nomeArqSaida[QtdDigNomEst];
	int possuiNovaCadeia;
	int qtdCadeias  = 0 ;

 	estados = lerEstadosArquivo(nomeArqIn);
      
    lerNomeEstadoInicialArq(nomeArqIn, nomeEstadoInicial);
    
    procuraEstado(nomeEstadoInicial, estados, &estadoInicial);  
    
    estadosFinais = montaListaEstadosFinais(nomeArqIn, estados);
   
    possuiNovaCadeia = lerCadeiaArq(nomeArqIn, cadeia);   
    
    while(possuiNovaCadeia!=0)
    {
         nomeArquivoSaida(nomeArqIn, nomeArqSaida, ++qtdCadeias);             
     
         execultaLBA(cadeia, estadoInicial, estadosFinais, estados,nomeArqSaida);
         possuiNovaCadeia = lerCadeiaArq(nomeArqIn, cadeia);    
    }
   
}

Transicao *procuraTransicao(char ler, Transicao *transicoes)
{     
     for(;transicoes!=NULL;transicoes=transicoes->proxTransicao)
          if(ler==transicoes->ler)
               return transicoes;                                                  
                     
     return NULL;                 
}

int procuraEstado(char nomeEstado[], Estados *estados, Estado *novoEstado)
{
       for(;estados!=NULL;estados=estados->proxEstado)
          if(strcmp(nomeEstado,estados->estado.nomeEstado)==0)
          {
               *novoEstado= estados->estado;
               return 1;                                                  
          }          
     return 0;                 
                   
}

void execultaLBA(char cadeia[], Estado estadoInicial, Estados *estadosFinais, Estados *estados, char estadoArqOut[])
{
     Estado estadoCorrente = estadoInicial;
     int pos=1;
     Transicao *aux;
     FILE *arq;
     int cont;
     int qtdTransicoes = 0;
     
     arq = fopen(estadoArqOut,"w");
     fprintf(arq,"\n\t\tResultados da Execucao\n\n");  
     
     cont =0;
     fprintf(arq,"\n\t-> ");
	 do{
	 	  if(cont!=pos)
               fprintf(arq,"%c" , cadeia[cont]);
     	  else
     	       fprintf(arq,"%s%c" ,estadoCorrente.nomeEstado, cadeia[cont]);
     	  cont++;
	 }while(cadeia[cont]!='\0');
	 
     aux = procuraTransicao(cadeia[pos],estadoCorrente.transicoes);
     
     while(aux!=NULL)
     {
         qtdTransicoes++;     
	     cadeia[pos] = aux->escreve;          
          //movimentacao
          if(aux->mover == 'D')
              pos++;
          else if(aux->mover == 'E')
              pos--;
              
          if( procuraEstado(aux->proxEstado, estados, &estadoCorrente) == 0 )
          {
               printf("Nome de estado errado!!!\a\a");    
          }
          aux = procuraTransicao(cadeia[pos],estadoCorrente.transicoes);
         
		 cont =0;
	     fprintf(arq,"\n\t-> ");
		 do{
		 	  if(cont!=pos)
	               fprintf(arq,"%c" , cadeia[cont]);
	     	  else
	     	       fprintf(arq,"%s%c" ,estadoCorrente.nomeEstado, cadeia[cont]);
	     	  cont++;
		 }while(cadeia[cont]!='\0');
	 }  

	 
    if( procuraEstado(estadoCorrente.nomeEstado, estadosFinais, &estadoCorrente) == 1)
          fprintf(arq,"\n\n\t\t CADEIA ACEITA! ");
    else
          fprintf(arq,"\n\n\t\t CADEIA NAO ACEITA! ");
    
     fprintf(arq,"\n\n\t\t Tam Cad e QtdMov = ");
     
     fprintf(arq," %d\t%d ", strlen(cadeia)-2,qtdTransicoes);
          
     fclose(arq);
}

Estados* inseriEstado(Estados *estados, Estado novo)
{
     Estados *aux = estados;
     
     if(aux==NULL)
     {
          aux = (Estados*) malloc(sizeof(Estados));        
          aux->estado = novo;
          aux->proxEstado = NULL;       
          return aux;
     }
     
     for(;aux->proxEstado!=NULL;aux=aux->proxEstado);
     
     Estados *n = (Estados*) malloc(sizeof(Estados));
     n->estado = novo;
     n->proxEstado = NULL;
     aux->proxEstado = n;
     return estados;
}

Transicao* inseriTransicao(Transicao *transicoes, Transicao *nova)
{
     Transicao *aux = transicoes;
     
     nova->proxTransicao = NULL; 
     
     if(aux==NULL)         
          return nova;
     
     for(;aux->proxTransicao!=NULL;aux=aux->proxTransicao);
     
     aux->proxTransicao = nova;
     
     return transicoes;
}

Estados *criaEstado(char estado[], Transicao *transicoes, Estados *listaEstados )
{
       Estado novo;
       strcpy(novo.nomeEstado, estado);
       novo.transicoes=transicoes;
       
       return inseriEstado(listaEstados, novo);
}

Transicao *criaTransicao( char ler, char estadoProxEstado[], char escreve, char mover, Transicao *transicoes)
{
     Transicao *nova = (Transicao*)malloc(sizeof(Transicao));
     
     nova->ler = ler;
     strcpy(nova->proxEstado, estadoProxEstado);
     nova->escreve = escreve;
     nova->mover = mover; 
     
     return inseriTransicao(transicoes, nova);  
}


/*---------------------------------------------------------------------------------------------------------------------------------------
                                    FUNÇÕES REFERENTES AOS ARQUIVOS E MONTAGEM DOS ESTADOS E TRANSICOES
----------------------------------------------------------------------------------------------------------------------------------------*/

FILE *lerTransicao(Transicao **transicao, FILE *arq)
{
     int cont;
     char letra;
     
     fseek(arq,2*sizeof(char),SEEK_CUR);
     fread(&(*transicao)->ler,sizeof(char),1,arq);
     
     fseek(arq,6*sizeof(char),SEEK_CUR);
     cont=0;
     fread(&letra,sizeof(char),1,arq);
     while(letra!='"')
     {
          (*transicao)->proxEstado[cont++]=letra;                
          fread(&letra,sizeof(char),1,arq);                 
     }
     (*transicao)->proxEstado[cont]='\0'; 
     
     fseek(arq,2*sizeof(char),SEEK_CUR);
     fread(&(*transicao)->escreve,sizeof(char),1,arq);
       
     fseek(arq,3*sizeof(char),SEEK_CUR);
     fread(&(*transicao)->mover,sizeof(char),1,arq); 
     
     return arq;  
}

void adicionaTransicaoEstado(char nomeEstado[], Transicao* transicao, Estados** estados)
{
     Estado estadoProcurado;
     Estados *aux; 
     transicao->proxTransicao = NULL;
        
    for(aux=*estados;aux!=NULL;aux=aux->proxEstado)
         if(strcmp(nomeEstado,aux->estado.nomeEstado)==0)
         {
              aux->estado.transicoes = inseriTransicao(aux->estado.transicoes, transicao);              
              return;
         }          
    *estados = criaEstado(nomeEstado, transicao, *estados);   

}

Estados* lerEstadosArquivo(char estadoArquivo[])
{
     FILE *arq;
     Estados *estados=NULL;
     Estado estadoCorrente;
     Estado auxEst;
     Transicao *auxTran;
     char letra;
     char nomeEstado[QtdDigNomEst]="";
     int cont;
     Transicao *transicao;
     
     if(arq=fopen(estadoArquivo,"r"))
     {
           fread(&letra,sizeof(char),1,arq);
           while(letra=='(')
           {
               transicao = (Transicao*)malloc(sizeof(Transicao));
               fseek(arq,sizeof(char),SEEK_CUR);
               fread(&letra,sizeof(char),1,arq);
               cont = 0;
               while(letra!='"')
               {
                    nomeEstado[cont++] = letra;
                    fread(&letra,sizeof(char),1,arq);
               }                        
               nomeEstado[cont] = '\0';                                   
               
               arq = lerTransicao( &transicao, arq );
               
               adicionaTransicaoEstado(nomeEstado, transicao, &estados);
               
               if( procuraEstado(transicao->proxEstado, estados,&auxEst) == 0)
               { 
                   auxTran = (Transicao*)malloc(sizeof(Transicao));
                   auxTran = NULL;
                   estados = criaEstado(transicao->proxEstado, auxTran, estados); 
                                                  
               }
               fseek(arq,4*sizeof(char),SEEK_CUR);
               fread(&letra,sizeof(char),1,arq);
           }                        
     }     
    fclose(arq);
    return estados;    
}

void lerNomeEstadoInicialArq( char estadoArquivo[], char nomeEstadoInicial[])
{
     FILE *arq;
     char letra;
     int cont =0;     
     if((arq=fopen(estadoArquivo,"r"))!=NULL)
     {
          fread(&letra,sizeof(char),1,arq);
          while(letra!= '=' )
               fread(&letra,sizeof(char),1,arq);
          fseek(arq,1*sizeof(char),SEEK_CUR);
                        
          fread(&letra,sizeof(char),1,arq);            
          while(letra!= ';' )
         {
                           
              nomeEstadoInicial[cont++]=letra;                
              fread(&letra,sizeof(char),1,arq);                 
         }    
         nomeEstadoInicial[cont]='\0';   
     }
    else
    {
    	exit(-1);
    }
     fclose(arq);
}

Estados* montaListaEstadosFinais(char estadoArquivo[], Estados *estados)
{
     FILE *arq;
     char nomeEstadoFinal[QtdDigNomEst]="";
     char letra;
     Estados *estadosFinais = NULL;
     Estado estadoF;
     int cont=0;
     
     if(arq=fopen(estadoArquivo,"r"))
     {
          //pula o primeiro igual do estado inical
          fread(&letra, sizeof(char), 1, arq);
          while(letra!= '=')     
               fread(&letra, sizeof(char), 1, arq);
          
          //acha o proximo igual que será dos estados finais
          fread(&letra, sizeof(char), 1, arq);
          
          while(letra!= '=')     
               fread(&letra, sizeof(char), 1, arq);
          
          fseek(arq,1*sizeof(char),SEEK_CUR);     
          fread(&letra,sizeof(char),1,arq);            
          while(letra!= ';' )
          {       
                                      
              nomeEstadoFinal[cont++]=letra;  
                        
              if(letra == ',')
              {
                   nomeEstadoFinal[cont-1]='\0';  
                   cont = 0;
                   
                   procuraEstado(nomeEstadoFinal, estados, &estadoF);
                   estadosFinais = inseriEstado(estadosFinais,estadoF);         
                   fread(&letra,sizeof(char),1,arq);            

              }
              fread(&letra,sizeof(char),1,arq);                 
          }    
         nomeEstadoFinal[cont]='\0'; 
         procuraEstado(nomeEstadoFinal, estados, &estadoF);
         estadosFinais = inseriEstado(estadosFinais,estadoF);  
          
     }
     fclose(arq);
     return estadosFinais;
}

int lerCadeiaArq(char estadoArquivo[],char cadeia[])
{
     FILE *arq;
     char letra;
     int cont = 0;
     static int numCadeia = 0;
     char letraAnt = ' ';
     int contCad = 0;
     int possuiCadeia;
     
     if(arq=fopen(estadoArquivo,"r"))
     {                      
          do
          {       
              possuiCadeia = 0;                           
              fread(&letra,sizeof(char),1,arq);                                   
              while(!feof(arq))        
              {          
                  if(letra=='#'&&(letraAnt ==' '||letraAnt == '='))
                  {
                      possuiCadeia = 1;                     
                      break; 
                  }   
                  letraAnt  = letra;
                  fread(&letra,sizeof(char),1,arq);                                   
              }
          }while(numCadeia>contCad++);
          
          if(possuiCadeia == 0)
              return 0;
              
          numCadeia++;
          while(letra!= ';'&&letra!= ',')
          {                           
               if(letra!=' ')                                                                    
                   cadeia[cont++] = letra;
               fread(&letra,sizeof(char),1,arq);
             
          }
          cadeia[cont]='\0';
            
     }     
     fclose(arq);
     return 1;
}

void nomeArquivoSaida(char nomeArqEnt[], char nomeArqSai[], int qtdCadeia)
{
	int cont=0;
	
	int tamNomeArqEnt = 0;
	
	while(nomeArqEnt[cont++]!='\0')
	     tamNomeArqEnt++;
	     
    tamNomeArqEnt-=4;
    cont=0;
	while(cont<tamNomeArqEnt)
	{
		nomeArqSai[cont] = nomeArqEnt[cont++];
	}
	nomeArqSai[cont++]='_';
	nomeArqSai[cont++]='O';
	nomeArqSai[cont++]='u';
	nomeArqSai[cont++]='t';
	nomeArqSai[cont++]='-';
	nomeArqSai[cont++]='C';
	nomeArqSai[cont++]='a';
	nomeArqSai[cont++]='d';
	nomeArqSai[cont++]=qtdCadeia+'0';
	nomeArqSai[cont++]='.';
	nomeArqSai[cont++]='t';
	nomeArqSai[cont++]='x';
	nomeArqSai[cont++]='t';
	nomeArqSai[cont]='\0';
}

