#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>


char* findPassword(char* pass, char* salt) {

    clock_t start, stop;
    double time_elapsed = 0.0;
    char* to_find;
    to_find = strdup(crypt(pass, salt)); //encrypt the pass to find and copy it in to_find allocating new memory
    printf("Password to find is %s, encrypted password to find is %s\n", pass, to_find);

    FILE *fp;
    // Read text file
    fp = fopen("password_dictionary.txt", "r");
    if(fp == NULL){
        printf("Error! I can't open the text file!");
        exit(1);
    }
    char* word;
    char* encrypt;
    char file[10];
    int i = 0;

    start = clock();
    while(fgets(file, 10, fp) != NULL){
        i++;
        // delete carriage return from the password to be encrypted
        word = strtok(file, "\n");
        if(word != NULL) {
            //encrypt the pass in the text file and copy it in encrypt allocating new memory
            encrypt = strdup(crypt(word, salt));

            if (strcmp(to_find, encrypt) == 0) {

                stop = clock();
                time_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
                printf("Password matched at position: %d\n", i);
                printf("Password decrypted is %s, password encrypted is %s\n", word,encrypt);
                printf("Time elapsed: %.2f ms\n", time_elapsed * 1000);
                free(to_find);
                free(encrypt);
                fclose(fp);
                return "OK";
            }
            free(encrypt);
        }

    }
    free(to_find);
    fclose(fp);
    return "ERROR: PASSWORD NOT FOUND";
}

int main(int argc, const char * argv[]) {
    char* password_to_find[3] = {"Manuel96","Paololep","carlo666"};
    //char* password_to_find[10] = {"freese93","feelin74","RaZeRis6","pan20139","cork1084","38993414","ybrf1721","vatino04","marioro9","carlo666"};
    char* pass;
    char* salt ="F4";
    for (int i = 0; i < 3; i++){
        pass = password_to_find[i];
        for (int j = 0; j < 3; j++){
            printf("%s\n", findPassword(pass, salt));
            printf("-----------------------------------------\n");
        }
    }


    return 0;

}
