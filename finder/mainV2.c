#include "finderV2.c"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


unsigned int readAsInt(FILE *fptr, unsigned int *globalPos) {
//  unsigned char ch1, ch2, ch3, ch4;
//  ch1 = fgetc(fptr);
//  ch2 = fgetc(fptr);
//  ch3 = fgetc(fptr);
//  ch4 = fgetc(fptr);
//  printf("%3i %3i %3i %3i\n", ch1, ch2, ch3, ch4);
//  return (unsigned char)ch1 * 16777216 + (unsigned char)ch2 * 65536 + (unsigned char)ch3 * 256 + (unsigned char)ch4;
//  return fgetc(fptr) << 24 | fgetc(fptr) << 16 | fgetc(fptr) << 8 | fgetc(fptr);
  unsigned int res = 0;
  res |= fgetc(fptr) << 24;
  ++(*globalPos);
  fseek(fptr, *globalPos, 0);
  res |= fgetc(fptr) << 16;
  ++(*globalPos);
  fseek(fptr, *globalPos, 0);
  res |= fgetc(fptr) << 8;
  ++(*globalPos);
  fseek(fptr, *globalPos, 0);
  res |= fgetc(fptr);
  ++(*globalPos);
  fseek(fptr, *globalPos, 0);
  return res;
}


void AddWordsFromFile(char*** words, char* fileName, unsigned int* countGroups) {
  FILE *fptr = fopen(fileName, "rb");
  if (fptr == NULL) {
    printf("File could not be opened.");
    fclose(fptr);
    return;
  }
  unsigned int globalPos = 0;
  *countGroups = readAsInt(fptr, &globalPos);
  //globalPos += 4;
  //printf("%i\n", *countGroups);
  (*words) = (char**)malloc((*countGroups) * sizeof(char*));
  for (unsigned int groupN = 0; groupN < *countGroups; ++groupN) {
    fseek(fptr, globalPos, 0);
    //printf("globalPos=%6i: %6i (diff=%6i)\n", globalPos, ftell(fptr), globalPos - ftell(fptr));
    unsigned int countWords = readAsInt(fptr, &globalPos);
    unsigned int lenWords = readAsInt(fptr, &globalPos);
    //globalPos += 8;
    printf("group=%2i: %6i %2i (%8i)\n", groupN, countWords, lenWords, countWords * lenWords * sizeof(char) + 8);
    (*words)[groupN] = (char*)malloc(countWords * lenWords * sizeof(char) + 8);
    //printf("%i ", (*words));
    //printf("%i ", (*words)[groupN]);
    writePointerAsInt(&countWords, (*words)[groupN]);
    writePointerAsInt(&lenWords, (*words)[groupN] + 4);
    for (unsigned int i = 8; i < countWords * lenWords + 8; ++i) {
      (*words)[groupN][i] = fgetc(fptr);
      ++globalPos;
      //printf(num2charEN((*words)[groupN][i]));
      fseek(fptr, globalPos, 0);
    };
    //printf("\n");
    //globalPos += countWords * lenWords;
    //printf("globalPos=%8i: %8i (diff=%8i)\n", globalPos, ftell(fptr), globalPos - ftell(fptr));
  }
  fclose(fptr);
}


void printMap(unsigned int sizex, unsigned int sizey, char** map) {
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y]) {
        printf("\033[92m%s\033[m ", num2charEN(map[x][y]));
      } else {
        printf("\033[91m*\033[m ");
      }
    }
    printf("\n");
  }
}


void test1() {
  clock_t start, stop;

  unsigned int sizex = 7;
  unsigned int sizey = 7;
  char* charsEN = (char*)malloc(26*sizeof(char));
  for (unsigned int i = 0; i < 26; ++i) charsEN[i] = i+1;
  charsEN[25] = 27;
  unsigned int countCharsEN = 26;

  char** map = (char**)malloc(sizex*sizeof(char*));
  for (unsigned int x1 = 0; x1 < sizex; ++x1) {
    map[x1] = (char*)malloc(sizey*sizeof(char));
    for (unsigned int y1 = 0; y1 < sizey; ++y1) {
      map[x1][y1] = 0;
    }
  }
  ///*For EN (5x5)*/map[2][0] = 8; map[2][1] = 9; map[2][2] = 7; map[2][3] = 8; map[2][4] = 20; map[1][1] = 20; map[1][2] = 8;
  /*For EN (7x7)*/map[3][0] = 15; map[3][1] = 21; map[3][2] = 20; map[3][3] = 7; map[3][4] = 18; map[3][5] = 15; map[3][6] = 23;

  char** words;
  unsigned int countGroups = 0;
  AddWordsFromFile(&words, "wordsEN24C.txt", &countGroups);
  --countGroups;
  //for(;;);
  //for (unsigned int groupN = 0; groupN < countGroups; ++groupN) {
  //  printf("%i ", words);
  //  printf("%i ", words[groupN]);
  //  printf(num2charEN(words[groupN][8]));
  //}
  int exitcode;
  char* thisWord = (char*)malloc(24);
  thisWord[0] = 15; thisWord[1] = 21; thisWord[2] = 20; thisWord[3] = 7; thisWord[4] = 18; thisWord[5] = 15; thisWord[6] = 23;
  excludeWord(&exitcode, thisWord, 7, &words, countGroups);

  unsigned int maxWordLength = 25;
  unsigned int bestLen = 0;
  char* bestWord = (char*)malloc(maxWordLength * sizeof(char));
  unsigned int bestPosX = 0;
  unsigned int bestPosY = 0;
  char bestChar = 0;
  unsigned int* bestPathX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* bestPathY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  char exitcodeWord;

  printf("Begin...\n");
  start = clock();
  findMaxWord(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsEN, countCharsEN, words, countGroups, maxWordLength);
  stop = clock();
  double tim = (double)(stop - start) / CLOCKS_PER_SEC;
  printf("It took %2ih %2im %2is %3ims\nResults are:\n", (int)(tim/3600), (int)(tim/60)%60, (int)(tim)%60, (int)(tim*1000.0F)%1000);

  printf("%i\n", bestLen);
  printf("%i %i %s (%2i)\n", bestPosX, bestPosY, num2charEN(bestChar), bestChar);
  for (unsigned int i = 0; i < bestLen; ++i) printf(num2charEN(bestWord[i]));
  printf(" (");
  for (unsigned int i = 0; i < bestLen; ++i) printf("%2i,", bestWord[i]);
  printf(")\n");
  for (unsigned int i = 0; i < bestLen; ++i) printf("(%i;%i);", bestPathX[i], bestPathY[i]);
  printf("\n");

  printMap(sizex, sizey, map);

  free(bestWord);
  free(bestPathX);
  free(bestPathY);
}


void test2() {
  clock_t start, stop, startAll, stopAll;

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
  /*For EN (5x5) took 29.578 s on 'findMaxWord'*/map[2][0] = 8; map[2][1] = 9; map[2][2] = 7; map[2][3] = 8; map[2][4] = 20; map[1][1] = 20; map[1][2] = 8;
  ///*For EN (7x7)*/map[3][0] = 15; map[3][1] = 21; map[3][2] = 20; map[3][3] = 7; map[3][4] = 18; map[3][5] = 15; map[3][6] = 23;
  ///*For RU*/map[2][0] = 20; map[2][1] = 6; map[2][2] = 19; map[2][3] = 1; map[2][4] = 3;  // тесав

  printf("Reading from a file...\n");
  char** words;
  unsigned int countGroups = 0;
  AddWordsFromFile(&words, "wordsEN24C.txt", &countGroups);
  --countGroups;
  printf("Word from a file are read!\n");
  printf("Creating Tries...\n");
  void* triesWords;
  createTries(&triesWords, words, countGroups, countCharsEN+1);
  printf("Tries created!\n");
  //showTrie(triesWords, 3);
  int exitcodeExclude;
  char* thisWord = (char*)malloc(24);
  thisWord[0] = 15; thisWord[1] = 21; thisWord[2] = 20; thisWord[3] = 7; thisWord[4] = 18; thisWord[5] = 15; thisWord[6] = 23;
  excludeWord(&exitcodeExclude, thisWord, 7, &words, countGroups);
  excludeWordTrie(&exitcodeExclude, thisWord, 7, &triesWords);
  printf("exitcodeExclude=%i\n", exitcodeExclude);
  unsigned int space = unusedSpace(triesWords);
  printf("count=%i (mem=%ib -> %fMb)\n", space, space*15, (double)space*15.0F/1024.0F/1024.0F);
  // sizeof(struct TrieNode) = 15
  //for (;;);
  //return;

  unsigned int maxWordLength = 30;
  unsigned int bestLen = 0;
  char* bestWord = (char*)malloc(maxWordLength * sizeof(char));
  unsigned int bestPosX = 0;
  unsigned int bestPosY = 0;
  char bestChar = 0;
  unsigned int* bestPathX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* bestPathY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  char exitcodeWord;
  unsigned int score = 0;

  unsigned int freeSpaces = 1;//(sizex - 1) * sizey;

  wprintf(L"Hi!\nПривет!\n");
  wprintf(L"こんにちは!\n");

  unsigned int timeAll = 0;

  startAll = clock();
  for (unsigned int i = 0; i < freeSpaces; ++i) {
    printf("Begin...\n");
    start = clock();
    //findMaxWord(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsEN, countCharsEN, words, countGroups, maxWordLength);
    // ^ score = 97 in 30.624s
    //findMaxWordTrie(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsEN, countCharsEN, triesWords, maxWordLength);
    // ^ score = 86 in 2.802s
    //predict(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsEN, countCharsEN, words, countGroups, maxWordLength, 0, 0);
    // ^ score = ? in ?s
    stop = clock();
    timeAll += stop - start;
    double tim = (double)(stop - start) / CLOCKS_PER_SEC;
    printf("It took %2ih %2im %2is %3ims.\nResults are:\n", (int)(tim/3600), (int)(tim/60)%60, (int)(tim)%60, (int)(tim*1000.0F)%1000);

    if (exitcodeWord == 1) {
      printf("%i\n", bestLen);
      printf("%i %i ", bestPosX, bestPosY);
      printf(num2charEN(bestChar));
      //wprintf(num2charRU(bestChar));
      printf(" (%2i)\n", bestChar);
      for (unsigned int j = 0; j < bestLen; ++j) printf(num2charEN(bestWord[j]));
      //for (unsigned int j = 0; j < bestLen; ++j) wprintf(num2charRU(bestWord[j]));
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
    } else {
      printf("Haven`t found anything (exitcode=%2i).\n", exitcodeWord);
    }
    bestLen = 0;

    printMap(sizex, sizey, map);
  }
  stopAll = clock();
  double timAll = (double)(stopAll - startAll) / CLOCKS_PER_SEC;
  printf("All it took %2ih %2im %2is %3ims (with GUI).\n", (int)(timAll/3600), (int)(timAll/60)%60, (int)(timAll)%60, (int)(timAll*1000.0F)%1000);
  double timeAllSeconds = (double)timeAll / CLOCKS_PER_SEC;
  printf("All it took %2ih %2im %2is %3ims (%i clocks, only computing).\n", (int)(timeAllSeconds/3600), (int)(timeAllSeconds/60)%60, (int)(timeAllSeconds)%60, (int)(timeAllSeconds*1000.0F)%1000, timeAll);
  printf("Score: %i\n", score);

  free(bestWord);
  free(bestPathX);
  free(bestPathY);
}


int main() {
  char *locale = setlocale(LC_ALL, "");
  test2();
}