#include "headers/Misc.h"
#ifdef __linux__
#include <sys/time.h>
#endif
#include <ctime>
#include <cstdlib>

#include <SDL2/SDL.h>

#define RANDOM_VARIANTS 70

static char rstring_palette[RANDOM_VARIANTS + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&*(";


unsigned int current_timestamp() {
	unsigned int result = 0;
#ifdef __linux__
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
	result = (te.tv_sec - 1500000000) * 1000000 + te.tv_usec;
#endif
    return result;
}

int randomNumber(){
    srand (SDL_GetPerformanceCounter());
    return rand();
}
