#include "chip8screen.h"

// Controllo se i pixel esistono veramente
static void are_pixels_out_of_bounds(int x, int y)
{
    assert(x >= 0 && y >= 0 && x < CHIP8_WIDTH && y < CHIP8_HEIGTH);
}

// Funzione per impostare un pixel on
void chip8_screen_set(struct chip8_screen *screen, int x, int y)
{
    are_pixels_out_of_bounds(x, y);
    screen->pixels[y][x] = true;
}

// Funzione per controllare lo stato di un pixel
bool chip8_screen_is_set(struct chip8_screen *screen, int x, int y)
{
    are_pixels_out_of_bounds(x, y);
    return screen->pixels[y][x];
}

// Funzione per disegnare un intero sprite
bool chip8_screen_draw_sprite(struct chip8_screen *screen, int x, int y, const char *sprite, int num_bytes)
{
    bool pixel_changed = false;

    for (int ly = 0; ly < num_bytes; ly++)
    {
        // Gives the byte of the sprite
        char c = sprite[ly];
        for (int lx = 0; lx < 8; lx++)
            // Checking if the bit is set, if not skip to the next one
            if ((c & (0x80 >> lx)) != 0)
            {
                if (screen->pixels[(ly + y) % CHIP8_HEIGTH][(lx + x) % CHIP8_WIDTH])
                    pixel_changed = true;

                // We have to add ly and lx because we're at the bit level not the byte
                screen->pixels[(ly + y) % CHIP8_HEIGTH][(lx + x) % CHIP8_WIDTH] ^= true;
            }
    }
    return pixel_changed;
}

// Pulisci lo schermo
void chip8_screen_clear(struct chip8_screen *screen)
{
    memset(screen->pixels, false, sizeof(screen->pixels));
}
