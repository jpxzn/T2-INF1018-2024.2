#include <stdio.h>
#include "cria_func.h"

void preencheVetorEndereço(void* f,unsigned char codigo[], int* indice)
{
    unsigned long int endLong = (unsigned long int)f; 

    for(int i = 0; i < 8; i++)
    {
        codigo[i] = endLong & 0xff; 
        endLong >>= 8;
        *(indice)++;
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

//parte variante





//parte fixa 

    //passa endereço da função para %rax
    // movq $endereço, %rax
    codigo[indice++] = 0x48; 
    codigo[indice++] = 0xb8;
    preencheVetorEndereço(f, codigo[indice], &indice); 
    
    //chama função e retorna 
    codigo[indice++] = 0xff; codigo[indice++] = 0xd0; //call *%rax
    codigo[indice++] = 0xc9; //leave
    codigo[indice++] = 0xc3; //ret

}