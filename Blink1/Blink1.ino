#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>   
#include <MCUFRIEND_kbv.h>
#include "space_invaders.h"

using namespace SpaceInvader;

enum class GameState
{
    Ingame,
    Reset,
    Win,
};

MCUFRIEND_kbv lcd;
const int scale = 3;
GameState state = GameState::Ingame;
Game game(5, 3);

void DrawSprite(Sprite* sprite, short x, short y)
{
    char* pixels = (char*)sprite->pixels;

    for (short _y = 0; _y < sprite->height; _y++)
    {
        short yCal = ((_y + y * sprite->height) * scale);
        short yWidth = _y * sprite->width;

        for (short _x = 0; _x < sprite->width; _x++)
        {
            char c = pixels[yWidth + _x];

            if (c == '#')
            {
                lcd.fillRect(((_x + x * sprite->width) * scale), yCal, scale, scale, sprite->color);
            }
            else
            {
                lcd.fillRect(((_x + x * sprite->width) * scale), yCal, scale, scale, 0x0000);
            }
        }
    }
}
void DrawBlack(int x, int y)
{
    for (int _y = 0; _y < 8; _y++)
    {
        for (int _x = 0; _x < 11; _x++)
        {
            lcd.fillRect(_x * scale + (x * 11 * scale), _y * scale + (y * 8 * scale), scale, scale, 0x0000);
            
        }
    }
}
void DrawInvaders()
{
    for (int i = 0; i < game.invaderAmount; i++)
    {
        Invader* invader = game.invaders[i];

        if (!invader->isDead)
        {
            if (game.GetInvader(invader->px, invader->py) == nullptr)
            {
                DrawBlack(invader->px, invader->py);
            }

            DrawSprite(invader->sprite, invader->x, invader->y);
        }
        else
        {
            DrawBlack(invader->x, invader->y);
        }
    }
}

void GameTiming(void*)
{
    short interval = 0;

    while (1)
    {
        if (state == GameState::Win)
        {
            lcd.fillScreen(0x07E0);
            lcd.setCursor(0, 0);
            lcd.setTextSize(5);
            lcd.println("You win");
        }
        else if (state == GameState::Reset)
        {
            game.Reset();
            lcd.fillScreen(0);
            state = GameState::Ingame;
        }
        else
        {
            bool isInvaderHitted = game.UpdateProjectile();

            if (isInvaderHitted)
            {
                tone(2, 440 / 2, 30);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                tone(2, 455 / 2, 30);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                tone(2, 580 / 2, 30);
                vTaskDelay(50 / portTICK_PERIOD_MS);
                tone(2, 455 / 2, 30);
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }

            if (interval % 5 == 0 || isInvaderHitted)
            {
                DrawBlack(game.projectile->px, game.projectile->py);
                game.projectile->UpdatePreviewsPos();

                if (interval % 5 == 0)
                {
                    game.UpdateInvaders();
                }

                DrawInvaders();
            }

            if (game.projectile->isShoted)
            {
                DrawBlack(game.projectile->px, game.projectile->py);
                DrawSprite(game.projectile->sprite, game.projectile->x, game.projectile->y);
            }

            DrawBlack(game.player->px, game.player->py);
            DrawSprite(game.player->sprite, game.player->x, game.player->y);
        }
       
        vTaskDelay(100 / portTICK_PERIOD_MS);
        interval++;
    }
}
void GameInput(void*)
{
    pinMode(31, INPUT);
    pinMode(33, INPUT);
    pinMode(35, INPUT);
    pinMode(2, OUTPUT);
  
    while (1)
    {
        Serial.println(game.player->x);

        if (digitalRead(31))
        {
            game.player->UpdatePreviewsPos();
            game.player->x--;
        }
        else if (digitalRead(33))
        {
            game.player->UpdatePreviewsPos();
            game.player->x++;
        }
        if (game.player->x < 0)
        {
            game.player->x = 0;
        }
        if (game.player->x > 11)
        {
            game.player->x = 11;
        }
        if (digitalRead(35))
        {
            if (state == GameState::Ingame)
            {
                if (!game.projectile->isShoted)
                {
                    game.Shoot();

                    for (int i = 0; i < 8; i++)
                    {
                        tone(2, 110 + (i * 26));
                        delay(25);
                    }
                    tone(2, 0, 1);
                }
            }
            if (state == GameState::Win)
            {
                state = GameState::Reset;
            }
        }

        if (state == GameState::Ingame)
        {
            if (game.CheckWin())
            {
                state = GameState::Win;
            }
            if (game.CheckLose())
            {
                state = GameState::Reset;
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup(void) 
{
    Serial.begin(9600);

    lcd.begin(lcd.readID());
    lcd.setRotation(1);
    lcd.fillScreen(0x0000);

    xTaskCreate(GameInput, "GameInput", 128, NULL, 1  , NULL);
    xTaskCreate(GameTiming, "GameTiming", 128, NULL, 2, NULL);

    pinMode(52, OUTPUT);
    digitalWrite(52, HIGH);
}

void loop(void) 
{
}
