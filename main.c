#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/************  COMP201 ASSIGNMENT 1  ************/

//ENTER YOUR NAME HERE - Beyda Dur

#define MAX_SEQ_LEN 200 // maximum sequence length

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "calculate-tf") == 0) // analyze frequencies
    {
        
        FILE *fp = fopen("sequences.txt", "r"); // read the sequences.txt file
        if (fp == NULL) 
        {
            perror("Error opening file");
            return(-1);
        }

        // count the number of healthy and cancerous sequences
        int healthyCount = 0;
        int cancerousCount = 0;
        char line[MAX_SEQ_LEN];
        while (fgets(line, MAX_SEQ_LEN, fp))
        {
            if (line[0] == '0')
            {
                healthyCount++;
            }
            else if (line[0] == '1')
            {
                cancerousCount++;
            }
    
            line[strcspn(line, "\n")] = '\0'; // Add null terminator at the end of the line
        }   
        rewind(fp); // reset file pointer to beginning

        // count the number of occurrences of the input word in healthy and cancerous sequences.
        char word[15];
        strcpy(word, argv[2]); // get the word from the command line argument
        
        int healthyWordCount = 0;
        int cancerousWordCount = 0;
        while (fgets(line, MAX_SEQ_LEN, fp))
        {
            if (strstr(line, "0\t") != NULL)
            {
                if(strstr(line, word) != NULL) // word found in healthy sequence
                {
                    healthyWordCount++; 
                }
            }
            else if (strstr(line, "1\t") != NULL)
            {
                if (strstr(line, word) != NULL) // word found in cancerous sequence
                {
                    cancerousWordCount++;
                }
            }
        }

        fclose(fp);

        // Calculate tf scores
        float tfHealthy = (float)healthyWordCount / healthyCount;
        float tfCancerous = (float)cancerousWordCount / cancerousCount;

        if (tfHealthy > tfCancerous) // classify word as healthy or cancerous
        {
            printf("Total healthy: %i\nWord in healthy: %i\nCalculated tf value for this word: %.6f\nTotal cancerous: %i\nWord in cancerous: %i\nCalculated tf value for this word: %.6f\nThis word is mostly used for healthy sequences.\n", 
            healthyCount, healthyWordCount, tfHealthy, cancerousCount, cancerousWordCount, tfCancerous);
        }
        else if (tfCancerous > tfHealthy)
        {
            printf("Total healthy: %i\nWord in healthy: %i\nCalculated tf value for this word: %.6f\nTotal cancerous: %i\nWord in cancerous: %i\nCalculated tf value for this word: %.6f\nThis word is mostly used for cancerous sequences.\n", 
            healthyCount, healthyWordCount, tfHealthy, cancerousCount, cancerousWordCount, tfCancerous);
        }
        else if (tfHealthy == 0 && tfCancerous == 0)
        {
            printf("This word doesn’t occur in the text!");
        }
    }
    
    else if (strcmp(argv[1], "predict") == 0) { // cancerous sequence detection

        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        char *pch;
        int numSeqs = 0;
        int healthySeqs = 0, cancerousSeqs = 0;
        int healthyWordCounts[8] = {0}, cancerousWordCounts[8] = {0};
        float healthyTf[8] = {0}, cancerousTf[8] = {0};
        float healthyIdf[8] = {0}, cancerousIdf[8] = {0};
        float healthyTfidf = 0, cancerousTfidf = 0;
        int seqClass;
        int i, j;

        FILE *fp = fopen("sequences.txt", "r"); // open sequences file
        if (fp == NULL) {
            perror("Error opening file");
            return(-1);
        }

        while ((read = getline(&line, &len, fp)) != -1) { // read sequences file line by line
            pch = strtok(line, "\t\n"); // tokenize line
            seqClass = atoi(pch); // get class of sequence, convert pch to an integer
            numSeqs++; 

            if (seqClass == 0) {
                healthySeqs++; // count healthy number of sequences
            }
            else if (seqClass == 1) {
                cancerousSeqs++; // count cancerous number of sequences
            }

            pch = strtok(NULL, "\t\n"); // get gene sequence
            char *word; // tokenize gene sequence into words
            
            for (i = 0; i < 8; i++) {
                word = argv[i+2];
                int wordLen = strlen(word);
                int seqLen = strlen(pch);

                for (j = 0; j <= seqLen - wordLen; j++) { // count word occurrences in sequence
                    if (strncmp(word, pch+j, wordLen) == 0) {
                        if (seqClass == 0) {
                            healthyWordCounts[i]++; // count word occurrences in healthy sequence
                        }
                        else if (seqClass == 1) {
                            cancerousWordCounts[i]++; // count word occurrences in cancerous sequence
                        }
                    }
                }

                if (seqClass == 0) { // calculate tf
                    healthyTf[i] = (float)healthyWordCounts[i] / (float)healthySeqs;
                }
                else if (seqClass == 1) {
                    cancerousTf[i] = (float)cancerousWordCounts[i] / (float)cancerousSeqs;
                }
            }
        }
        
        for (i = 0; i < 8; i++) // calculate idf 
        {
            int seqsWithWord = healthyWordCounts[i] + cancerousWordCounts[i];
            if (seqsWithWord > 0)
            {
                healthyIdf[i] = log((float)numSeqs / (float)seqsWithWord);
                cancerousIdf[i] = log((float)numSeqs / (float)seqsWithWord);
            }
        }
        
        for (i = 0; i < 8; i++) // calculate tf-idf
        {
            healthyTfidf += healthyTf[i] * healthyIdf[i];
            cancerousTfidf += cancerousTf[i] * cancerousIdf[i];
        }

        if (healthyTfidf > cancerousTfidf) // classify sequence as healthy or cancerous
        {
            printf("The given sequence is predicted to be healthy.\n");
        }
        else
        {
            printf("The given sequence is predicted to be cancerous.\n");
        }

        for (i = 0; i < 8; i++)
        {
            printf("Word: %s\n", argv[i+2]);
            printf("Number of healthy sequences found = %d\n", healthyWordCounts[i]);
            printf("Number of cancerous sequences found = %d\n\n", cancerousWordCounts[i]);
            printf("Healthy tf = %.6f\n", healthyTf[i]);
            printf("Cancerous tf = %.6f\n", cancerousTf[i]);
            printf("IDF: ln( (30000 + 30000) / (%d) ) = %.6f\n\n", healthyWordCounts[i] + cancerousWordCounts[i], healthyIdf[i]);
            printf("Healthy tf-idf for word %s = %.6f\n", argv[i+2], healthyTf[i] * healthyIdf[i]);
            printf("Cancerous tf-idf for word %s = %.6f\n\n\n", argv[i+2], cancerousTf[i] * cancerousIdf[i]);
        }
        printf("Sum of Healthy tf-idf score = %.6f\n", healthyTfidf);
        printf("Sum of Cancerous tf-idf score =  %.6f\n\n\n", cancerousTfidf);
    }
        
    else
    {
        printf("Wrong Function!\n");
    }

    return 0;
}

