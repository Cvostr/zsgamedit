#include "headers/Misc.h"
#include <sys/time.h>
#include <ctime>
#include <cstdlib>

#define RANDOM_VARIANTS 70

char rstring_palette[RANDOM_VARIANTS + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&*(";


unsigned int current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    return (te.tv_sec - 1500000000) * 1000000 + te.tv_usec;
}

void genRandomString(std::string* result, unsigned int len){
    srand (current_timestamp());
    unsigned int _len = len + rand() % 3;
    for(unsigned int digit_i = 0; digit_i < _len; digit_i ++){//Iterate over all chars in string

        int v = rand() % RANDOM_VARIANTS; //Get random value

        result->push_back(rstring_palette[v]); //Push symbol at random value
     }
}
