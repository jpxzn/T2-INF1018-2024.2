#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"

//preenche o vertor do código com o valor hexadecimal de 8bytes(void*) necessário para instrução assembly
void preencheHexaPtr(void* f, unsigned char codigo[], int* indice)
{
    unsigned long int endLong = (unsigned long int)f;

    for (int i = 0; i < 8; i++) 
    {     
        codigo[*indice] = (unsigned char)(endLong & 0xff);
        (*indice)++;
        endLong >>= 8;
    }
}

//preenche o vertor do código com o valor hexadecimal de 4bytes(int) necessário para instrução assembly
void preencheHexaInt(int valor, unsigned char codigo[], int* indice)
{
    for (int i = 0; i < 4; i++) 
    {
        codigo[*indice] = (unsigned char)(valor & 0xff);
        (*indice)++;
        valor >>= 8;
    }
}


void cria_func (void* f, DescParam params[], int n, unsigned char codigo[])
{
    if (n < 1 || n > 3) 
    {
        fprintf(stderr, "Foram recebidos %d parametros. Apenas sao aceitos entre 1 a 3 parametros!\n", n);
        exit(1);
    }

    int indice = 0;

    //parte fixa
    codigo[indice++] = 0x55; //pushq %rbp

    //movq %rsp, %rbp
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89; 
    codigo[indice++] = 0xe5;

    //subq $32, %rsp
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x83;
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

    //movq %rdx, -24(%rbp)
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0x89;
    codigo[indice++] = 0x55;
    codigo[indice++] = 0xe8;

    //variável para controlar qual registrador tenho que pegar o valor na pilha
    int qtdParams = 0;

    //parte variante
    for(int i = 0; i < n; i++)
    {
        if(params[i].orig_val == FIX)
        {
            unsigned char regs[3] = {0xbf, 0xbe, 0xba};
            
            if(params[i].tipo_val == PTR_PAR)
            {
                //movq $endereço, %registradorParametro
                codigo[indice++] = 0x48;
                codigo[indice++] = regs[i];
                preencheHexaPtr(params[i].valor.v_ptr, codigo, &indice);
            } 
            else if(params[i].tipo_val == INT_PAR)
            {
                //movl $valorIntHexa, %registradorParametro
                codigo[indice++] = regs[i];
                preencheHexaInt(params[i].valor.v_int, codigo, &indice);
            }
        }
        else if(params[i].orig_val == IND)
        {
            unsigned char regs[3] = {0x39, 0x31, 0x11};

            //movq $endereço, %rcx
            codigo[indice++] = 0x48; 
            codigo[indice++] = 0xb9;
            preencheHexaPtr(params[i].valor.v_ptr, codigo, &indice);

            if(params[i].tipo_val == PTR_PAR)
            {
                //movq (%rcx), %registradorParametro
                codigo[indice++] = 0x48;
                codigo[indice++] = 0x8b;
                codigo[indice++] = regs[i];
            }
            else if(params[i].tipo_val == INT_PAR)
            {
                //movl (%rcx), %registradorParametro
                codigo[indice++] = 0x8b;
                codigo[indice++] = regs[i];
            }
        }

        if(params[i].orig_val == PARAM)
        {
            unsigned char origem[3] = {0xf8, 0xf0, 0xe8};
            unsigned char destino[3] = {0x7d, 0x75, 0x55};

            if(params[i].tipo_val == INT_PAR)
            {
                //movl -?(%rbp), %registradorParametro
                codigo[indice++] = 0x8b;
                codigo[indice++] = destino[i];
                codigo[indice++] = origem[qtdParams];
            }

            else if(params[i].tipo_val == PTR_PAR)
            {
                //movq -?(%rbp), %registradorParametro
                codigo[indice++] = 0x48;
                codigo[indice++] = 0x8b;
                codigo[indice++] = destino[i];
                codigo[indice++] = origem[qtdParams];
            }
            qtdParams++;
        }
    }

    //parte fixa
    //movq $endereço, %rax
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0xb8;
    preencheHexaPtr(f, codigo, &indice); 
    
    //call *%rax
    codigo[indice++] = 0xff; 
    codigo[indice++] = 0xd0;
    
    codigo[indice++] = 0xc9;//leave 
    codigo[indice++] = 0xc3; //ret
}