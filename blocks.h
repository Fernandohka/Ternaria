#ifndef BLOCKS
#define BLOCKS
#include <stdio.h>
#include "imports.h"


// Estrutura dos blocos, especificando com posição e tipo.
typedef struct block
{
    RECT hitbox;//Retângulo usado para colisão e desenho
    int x;//posições simplificadas
    int y;
    int type;
    wchar_t * img;//Caminho para a imagem que deve ser desenhada
} block;

// Função de definição da textura e colisão dos blocos
void BlockDefine(block * B)
{
    B->hitbox.left = B->x*32;
    B->hitbox.top = 977 - (B->y*32);
    B->hitbox.right = B->hitbox.left + 32;
    B->hitbox.bottom = B->hitbox.top + 32;
    switch (B->type)
    {
    case 1:
        B->img = L"imagens/Grama.bmp";
        break;

    case 2:
        B->img = L"imagens/Terra.bmp";
        break;

    case 3:
        B->img = L"imagens/Tronco.bmp";
        break;

    case 4:
        B->img = L"imagens/Folhas.bmp";
        break;

    case 5:
        B->img = L"imagens/Stone.bmp";
        break;

    default:
        break;
    }
}
// função para criar o mapa aleatoriamente
void CreateArchive()
{
    FILE * map = fopen("Map.txt", "w");
    int camada = rand()%8 + 6;
    int arvore = 0, arvtam = 0;
    int ypedra = camada - (rand()%2 + 5);
    
    for(int x = 0; x < 60; x++)
    {
        if(arvore == 0 && rand()%10 == 1) // criação aleatoria da arvore, 10% de chance de criar uma arvore caso o bloco anterior não tenha uma arvore
        {
            arvore = camada + 1;
            arvtam = rand()%6 + 3; // tamanho aleatorio da arvore
            for(int arvh = arvore; arvh < arvore + arvtam; arvh++) // criação dos troncos em linha reta para cima de acordo com o tamanho da arvore
            {
                fprintf(map, "%d,%d,%d\n", x, arvh, 3); // escreve o local do bloco no txt do mapa
            }
            // criação das folhas da arvore em volta do tronco de acordo com o tamanho da arvore
            for(int folhay = arvore + arvtam; folhay < arvore + arvtam * 2; folhay++)
            {
                for(int folhax = (x - arvtam / 2) + ((folhay - arvore - arvtam) / 2); folhax <= (x + arvtam / 2) - ((folhay - arvore - arvtam) / 2); folhax++)
                {
                    fprintf(map, "%d,%d,%d\n", folhax, folhay, 4); // escreve o local do bloco no txt do mapa
                }
            }
        }
        else if(arvore != 0) // caso o bloco anterior possua uma arvore retorna a variavel para 0
        {
            arvore = 0;
        }
        for(int y = camada; y >= 0; y--) // criação dos blocos do chão
        {
            fprintf(map, "%d,%d,%d\n", x, y, (y <= ypedra) ? 5 : (y == camada && arvore != camada + 1) ? 1 : 2); // escreve o local do bloco no txt do mapa e escolhe o tipo do bloco de acordo com a camada
        }
        camada += rand()%3 - 1; // aumenta ou diminui em 1 ou mantem a camada aleatoriamente
        if(camada < 6) // limita o minimo da camada que será gerada
        {
            camada = 6;
        }
        if(camada > 14) // limita o maximo da camada que será gerada
        {
            camada = 14;
        }
        ypedra += rand()%3 - 1; // aumenta ou diminui em 1 ou mantem a camada da pedra aleatoriamente
        if(ypedra > camada - 5) // limita o minimo da camada de pedra que será gerada
        {
            ypedra = camada - 5;
        }
        if(ypedra < camada - 7) // limita o maximo da camada de pedra que será gerada
        {
            ypedra = camada - 7;
        }
    }
    fclose(map); // finaliza a criação do mapa
}
/* 
Função de leitura do arquivo de mapa, fazemos a construção do mapa do Ternaria por meio da leitura desse arquivo.
*/ 
void ReadArchive(DArray *lista)
{
    int count = 0;
    FILE * File = fopen("Map.txt", "r");

    while(!feof(File))
    {
        ++count;
        block *bloco = malloc(sizeof(block));
        fscanf(File, "%i,%i,%i", &bloco->x, &bloco->y, &bloco->type);
        BlockDefine(bloco);
        DArrayAdd(lista, bloco);
    }
    fclose(File);
}

// Escreve no txt
void WriteArchive(DArray *lista)
{
    int count = 0;
    FILE * File = fopen("Map.txt", "w");
    for(int i = 0; i < lista->Size; ++i)
    {
        block * B = (block *) lista->List[i];
        fprintf(File, "%i,%i,%i\n", B->x, B->y, B->type);
    }
    fclose(File);
}
#endif
