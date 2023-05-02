#define _POSIX_C_SOURCE 199309L
#include "chip8.h"

int msleep(long msec);

// Questa è la lista di tutti i tasti che un chip-8 riconosce
const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
    SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f};

int main(int argc, char **argv)
{
    // Check per l'argomento
    if (argc < 2)
    {
        printf("Devi inserire il nome del file da caricare\n");
        exit(EXIT_FAILURE);
    }

    // Apertura file
    const char *filename = argv[1];
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "[ERROR] Usage: %s \"game path\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Calcolo dimensione file
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    printf("File dim: %ld byte\n", size);

    // Lettura file in un'array dinamico
    char *fileData = (char *) malloc(size);
    if(fileData == NULL)
    {
        fprintf(stderr, "[ERROR] Cannot malloc()\n");
        exit(EXIT_FAILURE);
    }
    fread(fileData, 1, size, file);

    // Dichiariamo il nostro chip
    struct chip8 chip; 

    // Procedura di inizializzazione chip (caricare i font e azzerare la memoria)
    chip8_init(&chip);

    // Carichiamo il programma nel nostro chip-8 e modifichiamo il PC
    chip8_load(&chip, fileData, size);

    // Abbiniamo la mappatura della tastiera con quella che abbiamo scelto
    chip8_keyboard_set_map(&chip.keyboard, keyboard_map);

    fclose(file);

    printf("Caricato programma: %s\n", filename);

    SDL_Init(SDL_INIT_EVERYTHING);

    // SDL_Window è una struttura che contiene tutte le informazioni riguardanti la finestra
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,                  // Nome finestra
        SDL_WINDOWPOS_UNDEFINED,                // Posizione iniziale sull'asse delle x
        SDL_WINDOWPOS_UNDEFINED,                // Posizione iniziale sull'asse delle y
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER,  // Dimensione larghezza scalata
        CHIP8_HEIGTH * CHIP8_WINDOW_MULTIPLIER, // Dimensione altezza e altezza
        SDL_WINDOW_SHOWN                        // Rende la finestra visibile
    );

    // SDL_Renderer è una struttura che ci permette di renderizzare colori e pixel su una finestra
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    while (1)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) // Prende un evento dallo stack di eventi e lo salva all'interno di event
        {
            switch (event.type)
            {

            // Evento per quando l'utente preme un pulsante
            case SDL_KEYDOWN:
            {
                char key = event.key.keysym.sym;
                int virtualKey = chip8_keyboard_map(&chip.keyboard, key);
                // Se esiste
                if (virtualKey != -1)
                    chip8_keyboard_down(&chip.keyboard, virtualKey);
            }
            break;

            // Evento per quando l'utente lascia un pulsante
            case SDL_KEYUP:
            {
                char key = event.key.keysym.sym;
                int virtualKey = chip8_keyboard_map(&chip.keyboard, key);
                // Se esiste
                if (virtualKey != -1)
                    chip8_keyboard_up(&chip.keyboard, virtualKey);
            }
            break;
            
            // Se l'utente preme il pulsante per uscire beh esce
            case SDL_QUIT:
                goto out;
            break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);       // Imposta il "pennello" nero
        SDL_RenderClear(renderer);                          // Colora la finestra di nero
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // Imposta il "pennello" bianco

        for (int x = 0; x < CHIP8_WIDTH; x++)
            for (int y = 0; y < CHIP8_HEIGTH; y++)
                // Se il pixel e' stato impostato allora lo mostriamo
                if (chip8_screen_is_set(&chip.screen, x, y))
                {
                    SDL_Rect r;                        // Letteralmente un rettangolo
                    r.x = x * CHIP8_WINDOW_MULTIPLIER; // Posizione del rettangolo sull'asse x
                    r.y = y * CHIP8_WINDOW_MULTIPLIER; // Posizione del rettangolo sull'asse y
                    r.w = CHIP8_WINDOW_MULTIPLIER;     // Larghezza del rettangolo
                    r.h = CHIP8_WINDOW_MULTIPLIER;     // Altezza del rettangolo
                    SDL_RenderFillRect(renderer, &r);  // Disegna e RIEMPIE il rettangolo
                }

        SDL_RenderPresent(renderer); // Effettua un "refresh" dello schermo mostrando le modifiche che abbiamo apportato

        if (chip.registers.delay_timer > 0)
        {
            msleep(30);
            chip.registers.delay_timer -= 1;
        }
        
        if (chip.registers.sound_timer > 0)
        {
            // TODO: find linux Beep() function
            chip.registers.sound_timer = 0;
        }

        // Legge il prossimo opcode dalla memoria
        unsigned short opcode = chip8_memory_get_short(&chip.memory, chip.registers.program_counter);

        // Incrementa il program counter (PC)
        chip.registers.program_counter += 2;

        // Esegue l'istruzione
        chip8_exec(&chip, opcode);
    }

out:
    SDL_DestroyWindow(window);
    return 0;
}


/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}