/* Compile:
gcc main.c -o Ternaria.exe -l gdi32 -l msimg32
*/

#include "imports.h"
#include "input.c"

DWORD ThreadID;
HANDLE Thread;


//Função da thread principal
DWORD WINAPI MainThread(LPVOID lpParam)
{
    HWND hwnd = *((HWND *)lpParam);
    srand(time(NULL));
    CreateArchive();

    int gameover = 0, count = 0;
    character player;
    player.hitbox.left = 320;
    player.hitbox.right = 351;
    player.hitbox.top = 120;
    player.hitbox.bottom = 183;
    player.life = 10;
    player.damage = 1;
    player.state = 0;
    //ReadItems(&player);
    player.mainSlot = 0;
    player.inventory[0].id = 0;
    player.inventory[1].id = 1;
    player.inventory[2].id = 2;
    player.inventory[3].id = 3;
    ItemDefine(&player.inventory[0]);
    ItemDefine(&player.inventory[1]);
    ItemDefine(&player.inventory[2]);
    ItemDefine(&player.inventory[3]);

    zombie zombie;
    zombie.hitbox.left =420;
    zombie.hitbox.right = 451;
    zombie.hitbox.top = 120;
    zombie.hitbox.bottom = 183;
    zombie.life = 12;
    zombie.damage = 2;
    zombie.jumpBot = 0;
    zombie.canJumpBot = 1;
    zombie.state = 1;

    DArrayCreate(&Map, 200);
    ReadArchive(&Map); 
    
    /*
    Normalmente, se usa um sleep para esperar uma determinada quantidade de tempo. As funções de sleep são
    imprecisas, pois liberam a thread para o sistema operacional, que pode te devolver depois do prazo.
    Os timers do Windows travam a thread, mas não a liberam, tornando-os muito precisos e eficientes.
    */

    HANDLE Timer = CreateWaitableTimer(NULL, 0, NULL);
    LARGE_INTEGER DueTime;
    DueTime.QuadPart = -333333;
    SetWaitableTimer(Timer, &DueTime, 33, NULL, NULL, 0);//Timer com intervalo de 33ms (30 fps)
    
    HDC hdc = GetDC(hwnd);
    RECT R;
    GetClientRect(hwnd, &R);
    RenderBkgd(hdc);
    RenderMap(&Map, hdc);
    RenderLife(hdc, player.life);
    ReleaseDC(hwnd, hdc);
    
    
    while(player.life >= 0)
    {
        HDC hdc = GetDC(hwnd);
        RECT R;
        GetClientRect(hwnd, &R);
        HDC TempDC = CreateCompatibleDC(hdc);
        HBITMAP Bitmap = CreateCompatibleBitmap(hdc, (R.right-R.left), (R.bottom - R.top));
        SelectObject(TempDC, Bitmap);
        BitBlt(TempDC, 0, 0, R.right-R.left, R.bottom-R.top, hdc, 0, 0, SRCCOPY);

        EraseRect(TempDC, &player.hitbox);
        EraseRect(TempDC, &zombie.hitbox);
        Input(TempDC, &player, &zombie, &Map);
        MoveZombie(&player, &zombie);
        RenderZombie(&zombie, TempDC);
        RenderPlayer(&player, TempDC);
        RenderTool(&player, TempDC);

        renderInv(TempDC, &player);

        count += 1;
        if(count == 300)
        {
            Regeneration(&player);
            RenderLife(TempDC, player.life);
            count = 0;
        }

        BitBlt(hdc, 0, 0, R.right-R.left, R.bottom-R.top, TempDC, 0, 0, SRCCOPY);
        DeleteDC(TempDC);
        DeleteObject(Bitmap);
        ReleaseDC(hwnd, hdc);

        WaitForSingleObject(Timer, INFINITE);//Aqui se espera até o timer terminar
    }
}

//Função para o tratamanto de mensagens
LRESULT CALLBACK WndProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT PS;
            HDC hdc = BeginPaint(hwnd, &PS);
            FillRect(hdc, &PS.rcPaint, CreateSolidBrush(RGB(255,255,255)));
            EndPaint(hwnd, &PS);
        }
        break;
        case WM_CLOSE:
            DArrayDestroy(&Map);
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const wchar_t WClassName[] = L"Ternaria";
    MSG Msg;
    WNDCLASS Window = {};
    Window.lpfnWndProc = WndProc;
    Window.hInstance = hInstance;
    Window.lpszClassName = WClassName;


    if(!RegisterClass(&Window))
    {
        MessageBox(NULL, L"Window Class Registration Failed", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx
    (
        0,
        WClassName,
        L"Ternaria",
        WS_OVERLAPPEDWINDOW,
        200, 10, 975, 720,
        NULL, NULL, hInstance, NULL
    );

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }
    Ghwnd = hwnd;
    
    HICON hIcon = (HICON)LoadImage(NULL, L"imagens/iconeTernaria.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    
    if (hIcon != NULL) {
        // Definindo o ícone grande da janela
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        // Definindo o ícone pequeno da janela
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    } else {
        MessageBox(NULL, L"Erro ao carregar o icone!", L"Erro", MB_ICONERROR);
    }

    HCURSOR Cursor = (HCURSOR)LoadImage(NULL, L"imagens/Hand.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    SetCursor(Cursor);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //Abrindo uma thread separada para rodar o jogo
    Thread = CreateThread(NULL, 0, MainThread, &hwnd, 0, &ThreadID);

    //Loop para receber as mensagens da WINAPI
    while(GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return 0;
}
