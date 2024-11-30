#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"

void preencheHexaPtr(void* f, unsigned char codigo[], int* indice)
{
    unsigned long int endLong = (unsigned long int)f; 

    for(int i = 0; i < 8; i++)
    {     
        codigo[(*indice)] = endLong & 0xff;
        (*indice)++;
        endLong >>= 8;

    }
}

void preencheHexaInt(int valor, unsigned char codigo[], int* indice){
    for(int i = 0; i < 4; i++)
    {
       codigo[(*indice)++] = valor & 0xff;
       valor >>= 8;
    } 
}

void cria_func (void* f, DescParam params[], int n, unsigned char codigo[])
{
    if (n < 1 || n > 3) {
        fprintf(stderr, "Foram recebidos %d parametros. Apenas sao aceitos entre 1 a 3 parametros!\n", n);
        exit(1);
    }

    int indice = 0;

//parte fixa

    //pushq %rbp
    codigo[indice++] = 0x55; 
    
    //movq %rsp,%rbp
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89; 
    codigo[indice++] = 0xe5;

    //subq $32,%rsp
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89;
    codigo[indice++] = 0xec;
    codigo[indice++] = 0x20;

    //movq %rdi, -8(%rbp)
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89;
    codigo[indice++] = 0x7d;
    codigo[indice++] = 0xf8;

    //movq %rsi, -16(%rbp)
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89;
    codigo[indice++] = 0x75;
    codigo[indice++] = 0xf0;

    //movq %rdx, -32(%rbp)
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89;
    codigo[indice++] = 0x55;
    codigo[indice++] = 0xe8;

//parte variante

    int qtdParams = 0;

    for(int i = 0; i < n;i++)
    {
        if(params[i].orig_val==FIX)
        {
            unsigned char regs[3] = {0xbf,0xbe,0xba};
            
            if(params[i].tipo_val==PTR_PAR)
            {
                //movq $endereço, %registardorParametro
                codigo[indice++] = 0x48;
                codigo[indice++] = regs[i];
                preencheHexaPtr(params[i].valor.v_ptr, codigo, &indice);
            } 
            else if(params[i].tipo_val==INT_PAR)
            {
                //movl $valorInt, %registardorParametro
                codigo[indice++] = regs[i];
                preencheHexaInt(params[i].valor.v_int, codigo, &indice);
            }
        }

        else if(params[i].orig_val==IND)
        {
            unsigned char regs[3] = {0x39,0x31,0x11};

            if(params[i].tipo_val==PTR_PAR)
            {
                //movq $endereço, %rcx
                codigo[indice++] = 0x48; 
                codigo[indice++] = 0xb9;
                preencheHexaPtr(params[i].valor.v_ptr, codigo, &indice);

                //movq (%rcx), %registardorParametro
                codigo[indice++] = 0x48;
                codigo[indice++] = 0x8b;
                codigo[indice++] = regs[i];
            }
            else if(params[i].tipo_val==INT_PAR)
            {
                //movq $endereço, %rcx
                codigo[indice++] = 0x48; 
                codigo[indice++] = 0xb9;
                preencheHexaInt(params[i].valor.v_int, codigo, &indice);

                //movl (%rcx), %registardorParametro
                codigo[indice++] = 0x8b;
                codigo[indice++] = regs[i];
            }
        }

        if(params[i].orig_val==PARAM)
        {
            unsigned char origem[3] = {0xf8, 0xf0,0xe8};
            unsigned char destino[3] = {0x7d, 0x75, 0x55};
            if(params[i].tipo_val==INT_PAR)
            {
                //movl -valor(%rbp), %registradorParametro
                codigo[indice++] = 0x8b;
                codigo[indice++] = destino[i];
                codigo[indice++] = origem[qtdParams];

            }

            else if(params[i].tipo_val==PTR_PAR)
            {
                codigo[indice++] = 0x48;
                codigo[indice++] = 0x8b;
                codigo[indice++] = destino[i];
                codigo[indice++] = origem[qtdParams];

            }
            qtdParams++;
        }
    }

//parte fixa 

    //passa endereço da função para %rax
    // movq $endereço, %rax
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0xb8;
    preencheHexaPtr(f, codigo, &indice); 
    
    //chama função e retorna 
    codigo[indice++] = 0xff; codigo[indice++] = 0xd0; //call *%rax

    codigo[indice++] = 0xc9; //leave
    codigo[indice++] = 0xc3; //ret
}