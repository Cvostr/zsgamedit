#include "headers/Misc.h"

#include <ctime>
#include <cstdlib>

char rstring_palette[69] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$^&";

void genRandomString(std::string* result, unsigned int len){
    srand (time(NULL));
    for(unsigned int digit_i = 0; digit_i < len; digit_i ++){//Iterate over all chars in string

        int v = rand() % 69; //Get random value

        result->push_back(rstring_palette[v]); //Push symbol at random value
     }
}
