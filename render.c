#include "imports.h"
#include "functions.c"

void RenderBkgd(HDC hdc)
{
    BITMAP bm;
    HBITMAP Image = (HBITMAP)LoadImage(NULL, L"imagens/BackGround.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HDC BitmapDC = CreateCompatibleDC(hdc);
    SelectObject(BitmapDC, Image);
    GetObject((HGDIOBJ)Image, sizeof(bm), &bm);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, BitmapDC, 0, 0, SRCCOPY);
    DeleteDC(BitmapDC);
    DeleteObject(Image);
}

void RenderMap(const DArray * Map, HDC hdc)
{
    for(int i = 0; i < Map->Size; ++i)
    {
        block * B = (block *) Map->List[i];
        DrawImg(hdc, &B->hitbox, B->img);
    }
}

void RenderPlayer(const character * Player, HDC hdc)
{
    wchar_t * img;
    if(Player->state == 0)
    {
        if (Player->facing == 1)
        {
            img = L"imagens/Ferzinho3.bmp";
        } else {
            img = L"imagens/Ferzinho3DireitaParado.bmp";
        }
    }
    else
    {
        if (Player->facing == 1)
        {
            img = L"imagens/Ferzinho3Andante.bmp";
        } else {
            img = L"imagens/FerzinhoDireita.bmp";
        }
    }

    DrawImg(hdc, &Player->hitbox, img);
}

void RenderZombie(const zombie * zombie, HDC hdc)
{
    wchar_t * img;
    if(zombie->state == 0)
    {
        img = L"imagens/Zumbi.bmp";
    }
    else
    {
        img = L"imagens/ZumbiDireita.bmp";
    }
    DrawImg(hdc, &zombie->hitbox, img);
}

void renderInv(HDC hdc)
{
    wchar_t * img = L"BarraInv.bmp";
    RECT rect = {10, 10, 288, 32};
    DrawImg(hdc, &rect, img);
}
