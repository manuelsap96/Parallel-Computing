#define _GNU_SOURCE  //senza questo mi da problemi
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <pthread.h>

const int NUM_THREADS = 16;

//global variables accessible from all threads
int pass_found;
char* final_result;
int size;
char** stringList;
int block_size;
char* salt = "F4";
char* pass;


void myReadFile(char* filename){
    //method used to read the passwords from a text file
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp==NULL){
        printf("Error! I can't open the text file!");
        exit(1);
    }
    char* word;
    char* file = malloc(10 * sizeof(char));

    stringList = (char**)malloc(sizeof(char*));

    int index = 0;
    while(fgets(file, 10, fp) != NULL){
        word = strtok(file, "\n");
        if (word != NULL) {
            size++;
            stringList = realloc(stringList, size * sizeof(char*));//reallocate memory to have another more slot
            stringList[size - 1] = (char *) malloc(10 * sizeof(char));//allocate memory for the actual word

            strcpy(stringList[index], word);
            index++;
        }
    }
    free(file);
    fclose(fp);

}

void* compute(void* ind){
    //method used from each thread to find the password in each word list block associated to that thread

    long th_index = (long)ind;
    struct crypt_data data; //needed by reentrant crypt (crypt_r) function

    data.initialized = 0; //to allow multiple iterations and be sure of using correct data


    char* to_find;

    to_find = strdup(crypt_r(pass, salt,&data));

    char* word;
    char* encrypt;
    int index;


    for(int i = 0; i < block_size; i++){//loop on the block size, based on the thread index this will work on its portion

        index = (block_size * th_index) + i;//index based on the thread index

        if(index >= size || pass_found == 1) { //the first condition is inserted because the last thread can overshoot the size of the password list
            free(to_find);
            return final_result;
        }
        word = (char*)malloc(sizeof(char)*10);

        strcpy(word, stringList[index]);

        encrypt = strdup(crypt_r(word, salt, &data));

        if (strcmp(to_find, encrypt) == 0) {
            printf("Password matched in position: %d\n", index +1);
            printf("Password decrypted: %s\n", word);
            pass_found = 1;

            free(encrypt);
            free(word);
            free(to_find);
            strcpy(final_result,"Passoword found!");

            return final_result;
        }

        free(word);
        free(encrypt);
    }
    free(to_find);

    return final_result;

}

char* findPasswordPar() {
    clock_t start, stop;
    double time_elapsed = 0.0;


    block_size = (size / NUM_THREADS) + 1;

    pthread_t thread_list[NUM_THREADS];

    pass_found = 0;
    final_result = (char*)malloc(40*sizeof(char));
    strcpy(final_result,"ERROR: PASSWORD NOT FOUND");

    char* result;
    start = clock();


    //creation threads and start
    for(long in = 0; in < NUM_THREADS; in++) {
        pthread_create(&thread_list[in], NULL, compute, (void*)in);
    }

    //wait for termination of each thread and collect results
    for(long thread = 0; thread < NUM_THREADS; thread++) {
       pthread_join(thread_list[thread], (void*)&result);
    }
    stop = clock();
    time_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;

    if(strcmp(result,"ERROR: PASSWORD NOT FOUND") != 0)
        printf("Time elapsed: %.2f ms\n", time_elapsed * 1000);

    return result;
}


int main(int argc, const char * argv[]) {

    char* password_to_find[3] = {"Manuel96","Paololep","carlo666"};
    //char* password_to_find[10] = {"freese93","feelin74","RaZeRis6","pan20139","cork1084","38993414","ybrf1721","vatino04","marioro9","carlo666"};

    myReadFile("password_dictionary.txt");

    for (int i = 0; i < 3; i++){
        pass = password_to_find[i];
        for (int j = 0; j < 3; j++){
            printf("%s\n", findPasswordPar());
            printf("-----------------------------------------\n");
        }
    }

    return 0;

}
