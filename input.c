#include "imports.h"
#include "render.c"

int gravity = 0;
int canJump = 1;
int knockback = 0;
int knockbackSide = 0;
int canMove = 1;


void MoveLeft(character * Player, int Pixels)
{
    Player->state = Player->state ^ 1;
    Player->facing = 1;
    Player->hitbox.left -= Pixels;
    Player->hitbox.right -= Pixels;
    block * B = MapCollision(&Player->hitbox);
    if(B != NULL)
    {
        Player->hitbox.left = B->hitbox.right;
        Player->hitbox.right = Player->hitbox.left + 31;
    }
}

void MoveRight(character * Player, int Pixels)
{
    Player->state = Player->state ^ 1;
    Player->facing = 2;
    Player->hitbox.left += Pixels;
    Player->hitbox.right += Pixels;
    block * B = MapCollision(&Player->hitbox);
    if(B != NULL)
    {
        Player->hitbox.right = B->hitbox.left - 1;
        Player->hitbox.left = Player->hitbox.right - 31;
    }
}

void MoveDown(character * Player, int Pixels)
{
    Player->hitbox.top += Pixels;
    Player->hitbox.bottom += Pixels;
    block * B = MapCollision(&Player->hitbox);
    if(B != NULL)
    {
        gravity = 0;
        canJump = 1;
        knockback = 0;
        canMove = 1;
        Player->hitbox.bottom = B->hitbox.top;
        Player->hitbox.top = Player->hitbox.bottom - 63;
    }
}

void MoveUp(character * Player, int Pixels)
{
    Player->hitbox.top -= Pixels;
    Player->hitbox.bottom -= Pixels;
    block * B = MapCollision(&Player->hitbox);
    if(B != NULL)
    {
        canJump = 0;
        gravity = 0;
        knockback = 0;
        canMove = 1;
        Player->hitbox.top = B->hitbox.bottom;
        Player->hitbox.bottom = Player->hitbox.top + 63;
    }
}

void Jump(character * Player, int Pixels) 
{
    if(canJump == 1)
    {
        if(gravity == 15)
        {
            canJump = 0;
            gravity = 0;
            return;
        }
        MoveUp(Player, Pixels);
    }
}

void Gravity(character * Player)
{
    MoveDown(Player, gravity);
    if(gravity < 20)
    {
        gravity++;
    }
}

void Knockback(character * Player)
{
    if(knockbackSide == 1)
    {
        MoveLeft(Player, 7);
    }
    else
    {
        MoveRight(Player, 7);
    }
    MoveUp(Player, 7);
}

int CoolDown = 0;

void Input(HDC hdc, character * Player, zombie * Zombie, DArray * Map)
{
    if(CoolDown)
    {
        --CoolDown;
    }
    if(GetAsyncKeyState(VK_1))
    {
        Player->mainSlot = 0;
    }else
    if(GetAsyncKeyState(VK_2))
    {
        Player->mainSlot = 1;
    }else
    if(GetAsyncKeyState(VK_3))
    {
        Player->mainSlot = 2;
    }else
    if(GetAsyncKeyState(VK_4))
    {
        Player->mainSlot = 3;
    }
    if(GetAsyncKeyState(VK_LBUTTON))
    {
        if(!CoolDown)
        {
            POINT Mouse;
            GetCursorPos(&Mouse);
            ScreenToClient(Ghwnd, &Mouse);
            switch(Player->inventory[Player->mainSlot].id)
            {
                case 0:
                {
                    if(DestroyBlocks(Mouse))
                    {
                        RenderBkgd(hdc);
                        RenderMap(Map, hdc);
                        RenderLife(hdc, Player->life);
                    }
                }
                break;
                case 1:
                {
                    int res = Slash(Zombie, Player);
                    CoolDown = 10;
                    if(res < 0)
                    {
                        Zombie->knockback = 5;
                        knockbackSideZombie = 1;
                        KnockbackZombie(Zombie);
                    }else
                    if(res > 0)
                    {
                        Zombie->knockback = 5;
                        knockbackSideZombie = 2;
                        KnockbackZombie(Zombie);
                    }
                }
                break;
                case 2:
                {
                    int res = EstragarVelorio(Zombie, Player, Mouse);
                    CoolDown = 30;
                    if(res < 0)
                    {
                        Zombie->knockback = 8;
                        knockbackSideZombie = 1;
                        KnockbackZombie(Zombie);
                    }else
                    if(res > 0)
                    {
                        Zombie->knockback = 8;
                        knockbackSideZombie = 2;
                        KnockbackZombie(Zombie);
                    }
                }
                break;
                case 3:
                {
                    if(PlaceBlocks(Player, Zombie, Mouse))
                    {
                        RenderBkgd(hdc);
                        RenderMap(Map, hdc);
                        RenderLife(hdc, Player->life);
                    }
                }
                break;
                default:
                break;
            }
        }

    }
    if(GetAsyncKeyState(VK_A) && canMove)
    {
        MoveLeft(Player, 5);
    }
    if(GetAsyncKeyState(VK_D) && canMove)
    {
        MoveRight(Player, 5);
    }
    if(GetAsyncKeyState(VK_SPACE) && canMove)
    {
        Jump(Player, 15);
    }
    else if(canJump == 1)
    {
        canJump = 0;
        gravity = 0;
    }
    Gravity(Player);
    if (Collision(&Player->hitbox, &Zombie->hitbox) && Player->vulnerability == 0)
    {
        Player->vulnerability = 30;
        Player->life -= Zombie->damage;
        RenderLife(hdc, Player->life);
        knockback = 1;
        if(Player->hitbox.left > Zombie->hitbox.left)
        {
            knockbackSide = 2;
        }
        else if (Zombie->hitbox.right > Player->hitbox.right)
        {
            knockbackSide = 1;
        }
    }
    if(knockback)
    {
        Knockback(Player);
        canMove = 0;
    }
    if(Player->vulnerability > 0)
    {
        Player->vulnerability--;
    }
}

void Regeneration(character * player)
{
    HDC hdc;
    if (player->life < 10)
    {
        player->life += 1;
    }
}