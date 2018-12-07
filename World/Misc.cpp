#include "headers/Misc.h"
#include <sys/time.h>
#include <ctime>
#include <cstdlib>

#define RANDOM_VARIANTS 70

char rstring_palette[RANDOM_VARIANTS + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&*(";


unsigned int current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    //long long milliseconds = te.tv_sec*1000LL + te.tv_usec; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return te.tv_sec * 1000 + te.tv_usec;
}

void genRandomString(std::string* result, unsigned int len){
    srand (current_timestamp());
    for(unsigned int digit_i = 0; digit_i < len; digit_i ++){//Iterate over all chars in string

        int v = rand() % RANDOM_VARIANTS; //Get random value

        result->push_back(rstring_palette[v]); //Push symbol at random value
     }
}
