#include "finderV2.c"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


unsigned int readAsInt(FILE *fptr) {
  unsigned int res = 0;
  res |= fgetc(fptr) << 24;
  res |= fgetc(fptr) << 16;
  res |= fgetc(fptr) << 8;
  res |= fgetc(fptr);
  return res;
}


void AddWordsFromFile(char*** words, char* fileName, unsigned int* countGroups) {
  FILE *fptr = fopen(fileName, "rb");
  if (fptr == NULL) {
    printf("File could not be opened.");
    fclose(fptr);
    return;
  }
  *countGroups = readAsInt(fptr);
  (*words) = (char**)malloc((*countGroups) * sizeof(char*));
  for (unsigned int groupN = 0; groupN < *countGroups; ++groupN) {
    unsigned int countWords = readAsInt(fptr);
    unsigned int lenWords = readAsInt(fptr);
    //printf("group=%2i: %6i %2i (%8i)\n", groupN, countWords, lenWords, countWords * lenWords * sizeof(char) + 8);
    (*words)[groupN] = (char*)malloc(countWords * lenWords * sizeof(char) + 8);
    writePointerAsInt(&countWords, (*words)[groupN]);
    writePointerAsInt(&lenWords, (*words)[groupN] + 4);
    for (unsigned int i = 8; i < countWords * lenWords + 8; ++i) {
      (*words)[groupN][i] = fgetc(fptr);
    };
  }
  fclose(fptr);
}


void printMap(unsigned int sizex, unsigned int sizey, char** map) {
  for (unsigned int x = 0; x <= sizex; ++x) {
    if (x == 0) printf("+-");
    else if (x == sizex) printf("v ");
    else printf("| ");
    for (unsigned int y = 0; y < sizey; ++y) {
      if (x == 0) {
        if (y == sizey - 1) printf("> y");
        else printf("--");
        continue;
      }
      if (map[x - 1][y]) {
        printf("\033[92m");
        //printf(num2charEN(map[x - 1][y]));
        wprintf(num2charRU(map[x - 1][y]));
        printf("\033[m ");
      } else {
        printf("\033[91m*\033[m ");
      }
    }
    printf("\n");
  }
  printf("x\n");
}


void game() {
  clock_t start, stop;

  unsigned int sizex = 5;
  unsigned int sizey = 5;
  char* charsEN = (char*)malloc(26*sizeof(char));
  for (unsigned int i = 0; i < 26; ++i) charsEN[i] = i+1;
  charsEN[25] = 27;
  unsigned int countCharsEN = 26;
  char* charsRU = (char*)malloc(33*sizeof(char));
  for (int i = 0; i < 33; ++i) if (i < 25) {charsRU[i] = i+1;} else {charsRU[i] = i+2;}
  unsigned int countCharsRU = 33;

  char** map = (char**)malloc(sizex*sizeof(char*));
  for (unsigned int x1 = 0; x1 < sizex; ++x1) {
    map[x1] = (char*)malloc(sizey*sizeof(char));
    for (unsigned int y1 = 0; y1 < sizey; ++y1) {
      map[x1][y1] = 0;
    }
  }

  printf("Reading from a file...\n");
  char** words;
  unsigned int countGroups = 0;
  AddWordsFromFile(&words, "wordsRU24C.txt", &countGroups);
  --countGroups;
  printf("Word from a file are read!\n");
  printf("Creating Tries...\n");
  void* triesWords;
  createTries(&triesWords, words, countGroups, countCharsRU+1);
  printf("Tries created!\n");

  unsigned int maxWordLength = 30;
  unsigned int bestLen = 0;
  char* bestWord = (char*)malloc(maxWordLength * sizeof(char));
  unsigned int bestPosX = 0;
  unsigned int bestPosY = 0;
  char bestChar = 0;
  unsigned int* bestPathX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* bestPathY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  char exitcodeWord;
  unsigned int score, highestLen, worstTime;
  score = 0;
  highestLen = 0;
  worstTime = 0;

  unsigned int freeSpaces = (sizex - 1) * sizey;
  int exitcodeExclude;
  char* thisWord = (char*)malloc(maxWordLength);

  setRandomWord(sizex, sizey, &map, words, countGroups);
  printMap(sizex, sizey, map);
  for (unsigned int y = 0; y < sizey; ++y) thisWord[y] = map[sizex/2][y];
  excludeWord(&exitcodeExclude, thisWord, sizey, &words, countGroups);
  excludeWordTrie(&exitcodeExclude, thisWord, sizey, &triesWords);

  wprintf(L"Hi!\nПривет!\n");
  wprintf(L"こんにちは!\n");

  for (unsigned int i = 0; i < freeSpaces; ++i) {
    printf("Begin...\n");
    start = clock();
    //findMaxWord(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRU, words, countGroups, maxWordLength);
    findMaxWordTrie(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRU, triesWords, maxWordLength);
    //predict(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRUs, words, countGroups, maxWordLength, 0, 0);
    stop = clock();
    double tim = (double)(stop - start) / CLOCKS_PER_SEC;
    if (stop - start > worstTime) worstTime = stop - start;
    printf("It took %2ih %2im %2is %3ims.\nResults are:\n", (int)(tim/3600), (int)(tim/60)%60, (int)(tim)%60, (int)(tim*1000.0F)%1000);

    if (exitcodeWord == 1) {
      printf("%i\n", bestLen);
      printf("%i %i ", bestPosX, bestPosY);
      //printf(num2charEN(bestChar));
      wprintf(num2charRU(bestChar));
      printf(" (%2i)\n", bestChar);
      //for (unsigned int j = 0; j < bestLen; ++j) printf(num2charEN(bestWord[j]));
      for (unsigned int j = 0; j < bestLen; ++j) wprintf(num2charRU(bestWord[j]));
      printf(" (");
      for (unsigned int j = 0; j < bestLen; ++j) printf("%2i,", bestWord[j]);
      printf(")\n");
      for (unsigned int j = 0; j < bestLen; ++j) printf("(%i;%i);", bestPathX[j], bestPathY[j]);
      printf("\n");

      map[bestPosX][bestPosY] = bestChar;
      for (unsigned int j = 0; j < bestLen; ++j) thisWord[j] = bestWord[j];
      excludeWord(&exitcodeExclude, thisWord, bestLen, &words, countGroups);
      if (exitcodeExclude != 1) {
        printf("Something went wrong while excluding word.\n");
      }
      excludeWordTrie(&exitcodeExclude, thisWord, bestLen, &triesWords);
      if (exitcodeExclude != 1) {
        printf("Something went wrong while excluding word (exitcode=%i).\n", exitcodeExclude);
      }
      score += bestLen;
      if (bestLen > highestLen) highestLen = bestLen;
    } else {
      printf("Haven`t found anything (exitcode=%2i).\n", exitcodeWord);
    }
    bestLen = 0;

    printMap(sizex, sizey, map);
  }

  double worstTimeSeconds = (double)worstTime / CLOCKS_PER_SEC;
  printf("Worst time was: %2ih %2im %2is %3ims (%i clocks).\n", (int)(worstTimeSeconds/3600), (int)(worstTimeSeconds/60)%60, (int)(worstTimeSeconds)%60, (int)(worstTimeSeconds*1000.0F)%1000, worstTime);
  printf("Max ever found length: %i.\n", highestLen);

  free(bestWord);
  free(bestPathX);
  free(bestPathY);
}


int main() {
  char *locale = setlocale(LC_ALL, "Russian");
  game();
}