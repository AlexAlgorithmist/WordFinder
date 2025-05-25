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
        printf(num2charRU(map[x - 1][y]));
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

  // rules
  printf("Правила и суть игры:\n - Нужно найти такую точку на карте, чтобы поставив туда определённную букву, получилось слово наибольшей длинны, проходящие через эту точку.\n - Слово может идти только вверх-вниз ил вправо-влево (не по диагонали), а так же не должно проходить через оду и туже координату дважды (проще, без самопересечений), но могут изгибаться под прямым углом (не только 1 раз).\n - Если обе введённые координаты '0', то вы пропускаете свой ход.\n");

  printf("Читаю слова из файла...\n");
  char* charsEN = (char*)malloc(26*sizeof(char));
  for (unsigned int i = 0; i < 26; ++i) charsEN[i] = i+1;
  charsEN[25] = 27;
  unsigned int countCharsEN = 26;
  char* charsRU = (char*)malloc(33*sizeof(char));
  for (int i = 0; i < 33; ++i) if (i < 25) {charsRU[i] = i+1;} else {charsRU[i] = i+2;}
  unsigned int countCharsRU = 33;
  char** words;
  unsigned int countGroups = 0;
  AddWordsFromFile(&words, "wordsRU24C.txt", &countGroups);
  --countGroups;
  printf("Собираю дерево...\n");
  void* triesWords;
  createTries(&triesWords, words, countGroups, countCharsRU+1);
  printf("Всё готово!\n");

  unsigned int sizex;
  unsigned int sizey;
  printf("Введите высоту кары: ");
  scanf("%i", &sizex);
  printf("Введите шерину кары: ");
  scanf("%i", &sizey);

  char** map = (char**)malloc(sizex*sizeof(char*));
  for (unsigned int x1 = 0; x1 < sizex; ++x1) {
    map[x1] = (char*)malloc(sizey*sizeof(char));
    for (unsigned int y1 = 0; y1 < sizey; ++y1) {
      map[x1][y1] = 0;
    }
  }

  unsigned int countPlayers = 2;
  unsigned int turn = 0;
  unsigned int maxScore = 0;
  unsigned int *scores = (unsigned int*)malloc(countPlayers * sizeof(unsigned int));
  for (unsigned int i = 0; i < countPlayers; ++i) scores[i] = 0;
  unsigned int *players = (unsigned int*)malloc(countPlayers * sizeof(unsigned int));
  // 0 - player; 1 - 'findMaxWord'; 2 - 'findMaxWordTrie'; 3 - 'predict'.
  players[0] = 0;
  printf("Введите вид бота (1, 2 или 3, но 3 не работает, а 2 просто быстрее 1): ");
  scanf("%i", &players[1]);

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

  //unsigned int freeSpaces = countFree(sizex, sizey, map) - 1;
  int exitcodeExclude;
  char* thisWord = (char*)malloc(maxWordLength);

  setRandomWord(sizex, sizey, &map, words, countGroups);
  printMap(sizex, sizey, map);
  for (unsigned int y = 0; y < sizey; ++y) thisWord[y] = map[sizex/2][y];
  excludeWord(&exitcodeExclude, thisWord, sizey, &words, countGroups);
  excludeWordTrie(&exitcodeExclude, thisWord, sizey, &triesWords);

  //printf("Hi!\nПривет!\n");
  //printf("こんにちは!\n");

  //for (unsigned int i = 0; i < freeSpaces; ++i) {
  while (countFree(sizex, sizey, map) > 0) {
    int validated = 0;
    printf("Сейчас ходит игрок под номером %i.\n", turn + 1);
    printf("Его счёт: %i (%i от максимального).\n", scores[turn], maxScore - scores[turn]);
    if (players[turn] == 0) {
      printf("Введите пожалуйста в таком виде: 'x y <символ> <слово>' (слово длинной не более %i)\n", maxWordLength);
      //scanf("%i", &bestPosX);
      //scanf("%i", &bestPosY);
      //scanf("%[^\n]", bestChar);
      //scanf("%d %d %c %c", &bestPosX, &bestPosY, &bestChar, bestWord);
      //printf("\n%u\n", bestChar);
      //printf("\n%u\n", bestWord[0]);
      //printf("\n%u\n", bestWord[1]);
      //printf("\n%u\n", bestWord[2]);
      char symbol[5] = {0};
      char word[256] = {0};
      int success = 0;
      while (!success) {
        while (scanf("%i %i %4s %255s", &bestPosX, &bestPosY, symbol, word) != 4) printf("Введите пожалуйста в таком виде: 'x y <символ> <слово>' (слово длинной не более %i)\n", maxWordLength);
        if (bestPosX == 0 || bestPosY == 0) {
          if (bestPosX == bestPosY) {
            validated = 0;
            printf("Вы пропускаете свой ход.\n");
            break;
          }
          printf("Данная точка находится вне карты.\nПовторите попытку, взяв другую координату\n");
          continue;
        }
        bestPosX -= 1;
        bestPosY -= 1;
        if (bestPosX > sizex || bestPosY > sizey) {
          printf("Данная точка находится вне карты.\nПовторите попытку, взяв другую координату\n");
          continue;
        }
        if (map[bestPosX][bestPosY]) {
          printf("По данным координатам уже есть символ (%s).\nПовторите попытку, взяв другую координату\n", num2charRU(map[bestPosX][bestPosY]));
          continue;
        }
        bestLen = chars2wordRU(&bestWord, word);
        int exitcodeSeek;
        if (!seekWordTrie(&exitcodeSeek, bestWord, bestLen, &triesWords)) {
          printf("Такого слова не найдено либо уже было использовано.\nПродолжить не смотря на это? [+/-] ");
          char res;
          scanf("%1s", &res);
          if (res != "+"[0]) {
            printf("Тогда повторите свою попытку, выбрав другое слово.\n");
            continue;
          }
          printf("Хорошо, продолжаю игру.\n");
        }
        bestChar = char2numRU(symbol);
        map[bestPosX][bestPosY] = bestChar;
        int exitcodeValid;
        isValidWord(&exitcodeValid, bestWord, bestLen, bestPosX, bestPosY, sizex, sizey, map);
        map[bestPosX][bestPosY] = 0;
        if (exitcodeValid != 1) {
          printf("Такое слово не было найдено на карте (невозможно его составить).\nПродолжить не смотря на это? [+/-] ");
          char res;
          scanf("%1s", &res);
          if (res != "+"[0]) {
            printf("Тогда повторите свою попытку, выбрав другое слово.\n");
            continue;
          }
          printf("Хорошо, продолжаю игру.\n");
        }
        validated = 1;
        success = 1;
      }
    } else if (players[turn] == 1) {
      start = clock();
      findMaxWord(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRU, words, countGroups, maxWordLength);
      stop = clock();
    } else if (players[turn] == 2) {
      start = clock();
      findMaxWordTrie(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRU, triesWords, maxWordLength);
      stop = clock();
    } else if (players[turn] == 3) {
      start = clock();
      predict(&bestLen, &bestWord, &bestPosX, &bestPosY, &bestChar, &bestPathX, &bestPathY, &exitcodeWord, sizex, sizey, map, charsRU, countCharsRU, words, countGroups, maxWordLength, 0, 0);
      stop = clock();
    }
    if (players[turn] > 0) {
      double tim = (double)(stop - start) / CLOCKS_PER_SEC;
      if (stop - start > worstTime) worstTime = stop - start;
      //printf("It took %2ih %2im %2is %3ims.\nResults are:\n", (int)(tim/3600), (int)(tim/60)%60, (int)(tim)%60, (int)(tim*1000.0F)%1000);

      if (exitcodeWord == 1) {
        validated = 1;
        printf("Я поставлю на (%i;%i) букву '", bestPosX + 1, bestPosY + 1);
        //printf(num2charEN(bestChar));
        printf(num2charRU(bestChar));
        printf("' и получу слово '");
        //for (unsigned int j = 0; j < bestLen; ++j) printf(num2charEN(bestWord[j]));
        for (unsigned int j = 0; j < bestLen; ++j) printf(num2charRU(bestWord[j]));
        printf("'.\n");
      } else {
        printf("Не нашёл ничего :( (exitcode=%2i).\n", exitcodeWord);
      }
    }
    if (validated) {
      //printf("%i, %i: %i\n", bestPosX, bestPosY, bestChar);
      map[bestPosX][bestPosY] = bestChar;
      for (unsigned int j = 0; j < bestLen; ++j) thisWord[j] = bestWord[j];
      excludeWord(&exitcodeExclude, thisWord, bestLen, &words, countGroups);
      //if (exitcodeExclude != 1) printf("Something went wrong while excluding word.\n");
      excludeWordTrie(&exitcodeExclude, thisWord, bestLen, &triesWords);
      //if (exitcodeExclude != 1) printf("Something went wrong while excluding word (exitcode=%i).\n", exitcodeExclude);
      scores[turn] += bestLen;
      printf("Его новый счёт: %i.\n", scores[turn], maxScore - scores[turn]);
      if (scores[turn] > maxScore) maxScore = scores[turn];
      if (bestLen > highestLen) highestLen = bestLen;
      validated = 0;
    }
    bestLen = 0;
    turn = (turn + 1) % countPlayers;

    printMap(sizex, sizey, map);
  }

  unsigned int maxPlayer, secondPlayer;
  unsigned int secondScore = 0;
  for (unsigned int i = 0; i < countPlayers; ++i) {
    if (scores[i] == maxScore) {
      maxPlayer = i;
    }
    if (scores[i] > secondScore && scores[i] < maxScore) {
      secondPlayer = i;
      secondScore = scores[i];
    }
  }
  printf("Игрок под номер %i побеждает со счётом %i (%i отрыва от 2 места)\n", maxPlayer + 1, maxScore, maxScore - secondScore);

  double worstTimeSeconds = worstTime / CLOCKS_PER_SEC;
  //printf("Worst time was: %2ih %2im %2is %3ims (%i clocks).\n", (int)(worstTimeSeconds/3600), (int)(worstTimeSeconds/60)%60, (int)(worstTimeSeconds)%60, (int)(worstTimeSeconds*1000.0F)%1000, worstTime);
  //printf("Max ever found length: %i.\n", highestLen);

  free(bestWord);
  free(bestPathX);
  free(bestPathY);
}


int main() {
  char *locale = setlocale(LC_ALL, "Russian_Russia.65001");
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
  srand((unsigned int)time(NULL));
  game();
}