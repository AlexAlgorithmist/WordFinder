#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>


char* num2charEN(int num) {
  switch (num) {
    case 1:
      return "a";
    case 2:
      return "b";
    case 3:
      return "c";
    case 4:
      return "d";
    case 5:
      return "e";
    case 6:
      return "f";
    case 7:
      return "g";
    case 8:
      return "h";
    case 9:
      return "i";
    case 10:
      return "j";
    case 11:
      return "k";
    case 12:
      return "l";
    case 13:
      return "m";
    case 14:
      return "n";
    case 15:
      return "o";
    case 16:
      return "p";
    case 17:
      return "q";
    case 18:
      return "r";
    case 19:
      return "s";
    case 20:
      return "t";
    case 21:
      return "u";
    case 22:
      return "v";
    case 23:
      return "w";
    case 24:
      return "x";
    case 25:
      return "y";
    case 27:
      return "z";
    default:
      return " ";
  }
}
wchar_t* num2charRU(int num) {
  switch (num) {
    case 1:
      return L"а";
    case 2:
      return L"б";
    case 3:
      return L"в";
    case 4:
      return L"г";
    case 5:
      return L"д";
    case 6:
      return L"е";
    case 7:
      return L"ё";
    case 8:
      return L"ж";
    case 9:
      return L"з";
    case 10:
      return L"и";
    case 11:
      return L"й";
    case 12:
      return L"к";
    case 13:
      return L"л";
    case 14:
      return L"м";
    case 15:
      return L"н";
    case 16:
      return L"о";
    case 17:
      return L"п";
    case 18:
      return L"р";
    case 19:
      return L"с";
    case 20:
      return L"т";
    case 21:
      return L"у";
    case 22:
      return L"ф";
    case 23:
      return L"х";
    case 24:
      return L"ц";
    case 25:
      return L"ч";
    case 27:
      return L"ш";
    case 28:
      return L"щ";
    case 29:
      return L"ъ";
    case 30:
      return L"ы";
    case 31:
      return L"ь";
    case 32:
      return L"э";
    case 33:
      return L"ю";
    case 34:
      return L"я";
    default:
      return L" ";
  }
}


struct TrieNode {
  struct TrieNode* children;
  unsigned int count;
  char hasChildren;
  struct TrieNode* parent;
};


void createTries(struct TrieNode** result, char** words, unsigned int countWordGroups, unsigned int alphabetLength) {
  *result = (struct TrieNode*)malloc(sizeof(struct TrieNode));
  if (*result == NULL) {
    fprintf(stderr, "\033[91m[E]\033[m Memory allocation failed for root trie!\n");
    exit(EXIT_FAILURE);
  }
  unsigned int maxWordLength = *(unsigned int*)(words[0] + 4);
  (*result)->children = (struct TrieNode*)malloc((maxWordLength + 1) * sizeof(struct TrieNode));
  if ((*result)->children == NULL) {
    fprintf(stderr, "\033[91m[E]\033[m Memory allocation failed for children!\n");
    exit(EXIT_FAILURE);
  }
  (*result)->count = maxWordLength + 1;
  (*result)->hasChildren = 1;
  (*result)->parent = NULL;
  for (unsigned int j = 0; j < (*result)->count; ++j) {
    (*result)->children[j].children = NULL;
    (*result)->children[j].count = 0;
    (*result)->children[j].hasChildren = 0;
    (*result)->children[j].parent = *result;
  }
  for (unsigned int groupId = 0; groupId < countWordGroups; ++groupId) {
    unsigned int wordsLength = *(unsigned int*)(words[groupId] + 4);
    unsigned int countWords = *(unsigned int*)words[groupId];
    struct TrieNode* groupNode = &((*result)->children[wordsLength]);
    char* newWords = words[groupId] + 8;
    printf("doing for: %2i\n", wordsLength);
    for (unsigned int wordId = 0; wordId < countWords; ++wordId) {
      char* word = &newWords[wordId * wordsLength];
      struct TrieNode* currentNode = groupNode;
      for (unsigned int k = 0; k < wordsLength; ++k) {
        unsigned char ch = word[k];
        if (currentNode->children == NULL) {
          currentNode->children = (struct TrieNode*)malloc((alphabetLength + 1) * sizeof(struct TrieNode));
          if (currentNode->children == NULL) {
            fprintf(stderr, "\033[91m[E]\033[m Memory allocation failed at depth %u!\n", k);
            exit(EXIT_FAILURE);
          }
          currentNode->count = alphabetLength + 1;
          currentNode->hasChildren = 1;
          for (unsigned int j = 0; j < currentNode->count; ++j) {
            currentNode->children[j].children = NULL;
            currentNode->children[j].count = 0;
            currentNode->children[j].hasChildren = 0;
            currentNode->children[j].parent = currentNode;
          }
        }
        currentNode = &(currentNode->children[ch]);
      }
      currentNode->hasChildren = 1;
    }
  }
}

unsigned int unusedSpace(struct TrieNode* node) {
  unsigned int result = 0;
  for (unsigned int j = 0; j < node->count; ++j) {
    if (node->children[j].hasChildren == 0) result += 1;
    else result += unusedSpace(&(node->children[j]));
  }
  return result;
}


void showTrieDeep(struct TrieNode trie, unsigned int value, unsigned int deep, unsigned long long mask, char next, unsigned int maxDeep) {
  if (trie.hasChildren == 0 || deep > maxDeep) {
    //printf("Nope.\n");
    return;
  } else {
    for (unsigned int i = 0; i < deep - 1 && deep > 0; ++i) {
      //printf("%i", i);
      //if ((mask >> i) % 2) printf("│ ");
      //else printf("  ");
      if ((mask >> i) % 2) printf("|   ");
      else printf("    ");
    }
    if (next) printf("+---%i\n", value);
    else  printf(">---%i\n", value);
  }
  for (unsigned int i = 0; i < trie.count; ++i) {
    char nextNext = 0;
    for  (unsigned int j = i + 1; j < trie.count; ++j) {
      //printf("%i, %i\n", j, trie.children[j].hasChildren);
      if (trie.children[j].hasChildren) {
        nextNext = 1;
        mask |= (unsigned long long)1 << deep;
        break;
      }
    }
    if (nextNext == 0) mask &= ~((unsigned long long)1 << deep);
    showTrieDeep(trie.children[i], i, deep+1, mask, nextNext, maxDeep);
  }
}
void showTrie(struct TrieNode* trie, unsigned int index) {
  struct TrieNode thisTrie = *(struct TrieNode*)trie;
  if (thisTrie.hasChildren == 0) {
    printf("Nope.\n");
    return;
  }
  if (thisTrie.count < index) {
    printf("Nope.\n");
    return;
  }
  showTrieDeep(thisTrie.children[index], index, 1, 0, 0, -1);
}
void backtraceTrie(struct TrieNode* trie, struct TrieNode* result) {
  struct TrieNode* thisTrie = trie;
  if (thisTrie->parent == NULL) {
    result = trie;
    return;
  }
  printf("%p <- %p\n", thisTrie->parent, thisTrie);
  showTrieDeep(*(thisTrie->parent), 0, 1, 0, 0, 2);
  backtraceTrie(thisTrie->parent, result);
}

void excludeWordTrie(int *exitcode, char* word, unsigned int thisWordLength, struct TrieNode** words) {
  *exitcode = 0;
  struct TrieNode* currentNode = &((*words)->children[thisWordLength]);
  for (unsigned int k = 0; k < thisWordLength; ++k) {
    if (currentNode->hasChildren == 0 && k < thisWordLength - 1) {
      *exitcode = k + 2;
      return;
    }
    currentNode = &(currentNode->children[word[k]]);
  }
  currentNode->hasChildren = 0;
  *exitcode = 1;
  return;
}


void writePointerAsInt(unsigned int* ptr, char* to) {
  unsigned char* ptr2 = (unsigned char*)ptr;
  to[0] = ptr2[0];
  to[1] = ptr2[1];
  to[2] = ptr2[2];
  to[3] = ptr2[3];
}


void findPath(char* exitcode, char* word, unsigned int wordLength, unsigned int sizex, unsigned int sizey, char** map, unsigned int addx, unsigned int addy, unsigned int** pathXRes, unsigned int** pathYRes) {
  unsigned int* posesX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));  // yeah, do not optimised, because why the heck do we need 4294967295 length map?
  unsigned int* posesY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));  // yeah, do not optimised, because why the heck do we need 4294967295 length map?
  unsigned int ind = 0;
  char** visited = (char**)malloc(sizex * sizeof(char*));
  for (unsigned int x = 0; x < sizex; ++x) {
    visited[x] = (char*)malloc(sizey * sizeof(char));
  }
  char first = word[0];

  // find all possitions, where the letter match first of the word.
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y] == first) {
        posesX[ind] = x;
        posesY[ind] = y;
        ++ind;
      }
    }
  }

  if (ind == 0) {
    *exitcode = 0;
    free(posesX); free(posesY); for (unsigned int x = 0; x < sizex; ++x) free(visited[x]); free(visited);
    return;
  }

  unsigned int queueSize = wordLength * wordLength;  // less than or equal to "sizex * sizey * sizex * sizey", but in reality just "wordLength * wordLength".
  unsigned int queueStart = 0;
  unsigned int queueEnd = 0;
  unsigned int* queueX = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  unsigned int* queueY = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  unsigned int* queueIdx = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  unsigned int** queuePathX = (unsigned int**)malloc(queueSize * sizeof(unsigned int*));
  unsigned int** queuePathY = (unsigned int**)malloc(queueSize * sizeof(unsigned int*));
  for (unsigned int i = 0; i < queueSize; ++i) {
    queuePathX[i] = (unsigned int*)malloc(wordLength * sizeof(unsigned int));
    queuePathY[i] = (unsigned int*)malloc(wordLength * sizeof(unsigned int));
  }
  --wordLength;  // to account that indexes start with 0.

  for (unsigned int i = 0; i < ind; ++i) {
    for (unsigned int x = 0; x < sizex; ++x) {
      for (unsigned int y = 0; y < sizey; ++y) {
        visited[x][y] = 0;
        if (map[x][y] == 0) {
          visited[x][y] = 1;
        }
      }
    }
    unsigned int x = posesX[i];
    unsigned int y = posesY[i];
    queueStart = 0;
    queueEnd = 0;
    // begin queue with first pos
    ++queueEnd;
    queueX[0] = x;
    queueY[0] = y;
    queueIdx[0] = 0;
    unsigned int thisX;
    unsigned int thisY;
    unsigned int thisIdx;
    unsigned int* thisPathX;
    unsigned int* thisPathY;
    while (queueStart != queueEnd) {
      thisX = queueX[queueStart];
      thisY = queueY[queueStart];
      if (visited[thisX][thisY]) {
        queueStart = (queueStart + 1) % queueSize;
        continue;
      }
      visited[thisX][thisY] = 1;
      thisIdx = queueIdx[queueStart];
      thisPathX = queuePathX[queueStart];
      thisPathY = queuePathY[queueStart];
      thisPathX[thisIdx] = thisX;
      thisPathY[thisIdx] = thisY;
      if (thisIdx == wordLength) {
        char add = 0;
        for (unsigned int j = 0; j <= wordLength; ++j) {
          if (thisPathX[j] == addx && thisPathY[j] == addy) {
            add = 1;
            break;
          }
        }
        if (add) {
          *exitcode = 1;
          *pathXRes = (unsigned int*)malloc((wordLength + 1) * sizeof(unsigned int));
          *pathYRes = (unsigned int*)malloc((wordLength + 1) * sizeof(unsigned int));
          //printf("\n");
          for (unsigned int j = 0; j <= wordLength; ++j) {
            //printf("(%i %i)\n", thisPathX[j], thisPathY[j]);
            (*pathXRes)[j] = thisPathX[j];
            (*pathYRes)[j] = thisPathY[j];
            //printf("(%i %i)\n", (*pathXRes)[j], (*pathYRes)[j]);
          }
          free(queueX); free(queueY); free(queueIdx); for (unsigned int j = 0; j < queueSize; ++j) {free(queuePathX[j]); free(queuePathY[j]);} free(queuePathX); free(queuePathY);
          free(posesX); free(posesY); for (unsigned int x = 0; x < sizex; ++x) free(visited[x]); free(visited);
          return;
        }
        continue;
      }
      ++thisIdx;
      unsigned int newx, newy;
      newx = thisX + 1;
      if (newx < sizex) {
        if (map[newx][thisY] == word[thisIdx]) {
          queueX[queueEnd] = newx;
          queueY[queueEnd] = thisY;
          queueIdx[queueEnd] = thisIdx;
          for (unsigned int j = 0; j < thisIdx; ++j) {
            queuePathX[queueEnd][j] = thisPathX[j];
            queuePathY[queueEnd][j] = thisPathY[j];
          }
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
      newy = thisY + 1;
      if (newy < sizey) {
        if (map[thisX][newy] == word[thisIdx]) {
          queueX[queueEnd] = thisX;
          queueY[queueEnd] = newy;
          queueIdx[queueEnd] = thisIdx;
          for (unsigned int j = 0; j < thisIdx; ++j) {
            queuePathX[queueEnd][j] = thisPathX[j];
            queuePathY[queueEnd][j] = thisPathY[j];
          }
          queueEnd = (queueEnd + 1) % queueSize;
          }
      }
      if (thisX > 0) {
        newx = thisX - 1;
        if (map[newx][thisY] == word[thisIdx]) {
          queueX[queueEnd] = newx;
          queueY[queueEnd] = thisY;
          queueIdx[queueEnd] = thisIdx;
          for (unsigned int j = 0; j < thisIdx; ++j) {
            queuePathX[queueEnd][j] = thisPathX[j];
            queuePathY[queueEnd][j] = thisPathY[j];
          }
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
      if (thisY > 0) {
        newy = thisY - 1;
        if (map[thisX][newy] == word[thisIdx]) {
          queueX[queueEnd] = thisX;
          queueY[queueEnd] = newy;
          queueIdx[queueEnd] = thisIdx;
          for (unsigned int j = 0; j < thisIdx; ++j) {
            queuePathX[queueEnd][j] = thisPathX[j];
            queuePathY[queueEnd][j] = thisPathY[j];
          }
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
    }
  }

  *exitcode = 0;
  free(queueX); free(queueY); free(queueIdx); for (unsigned int i = 0; i < queueSize; ++i) {free(queuePathX[i]); free(queuePathY[i]);} free(queuePathX); free(queuePathY);
  free(posesX); free(posesY); for (unsigned int x = 0; x < sizex; ++x) free(visited[x]); free(visited);
}


void findMaxPath(char* exitcode, unsigned int *result, unsigned int sizex, unsigned int sizey, char** map) {
  unsigned int* posesX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* posesY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int ind = 0;
  char** visited = (char**)malloc(sizex * sizeof(char*));
  for (unsigned int x = 0; x < sizex; ++x) {
    visited[x] = (char*)malloc(sizey * sizeof(char));
  }

  // find all possitions, where the letter match first of the word.
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y]) {
        posesX[ind] = x;
        posesY[ind] = y;
        ++ind;
      }
    }
  }

  if (ind == 0) {
    *exitcode = 0;
    free(posesX); free(posesY); for (unsigned int x = 0; x < sizex; ++x) free(visited[x]); free(visited);
    return;
  }
  unsigned int queueSize = sizex * sizey * sizex * sizey;  // we do not know max path, so "sizex * sizey * sizex * sizey"
  unsigned int queueStart = 0;
  unsigned int queueEnd = 0;
  unsigned int* queueX = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  unsigned int* queueY = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  unsigned int* queueLen = (unsigned int*)malloc(queueSize * sizeof(unsigned int));
  *result = 0;
  for (unsigned int i = 0; i < ind; ++i) {
    for (unsigned int x = 0; x < sizex; ++x) {
      for (unsigned int y = 0; y < sizey; ++y) {
        visited[x][y] = 0;
        if (map[x][y] == 0) {
          visited[x][y] = 1;
        }
      }
    }
    unsigned int x = posesX[i];
    unsigned int y = posesY[i];
    queueStart = 0;
    queueEnd = 0;
    // begin queue with first pos
    ++queueEnd;
    queueX[0] = x;
    queueY[0] = y;
    queueLen[0] = 1;
    unsigned int thisX;
    unsigned int thisY;
    unsigned int thisLen;
    while (queueStart != queueEnd) {
      thisX = queueX[queueStart];
      thisY = queueY[queueStart];
      if (visited[thisX][thisY]) {
        queueStart = (queueStart + 1) % queueSize;
        continue;
      }
      visited[thisX][thisY] = 1;
      thisLen = queueLen[queueStart];
      if (thisLen > *result) *result = thisLen;
      ++thisLen;
      unsigned int newx, newy;
      newx = thisX + 1;
      if (newx < sizex) {
        if (map[newx][thisY]) {
          queueX[queueEnd] = newx;
          queueY[queueEnd] = thisY;
          queueLen[queueEnd] = thisLen;
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
      newy = thisY + 1;
      if (newy < sizey) {
        if (map[thisX][newy]) {
          queueX[queueEnd] = thisX;
          queueY[queueEnd] = newy;
          queueLen[queueEnd] = thisLen;
          queueEnd = (queueEnd + 1) % queueSize;
          }
      }
      if (thisX > 0) {
        newx = thisX - 1;
        if (map[newx][thisY]) {
          queueX[queueEnd] = newx;
          queueY[queueEnd] = thisY;
          queueLen[queueEnd] = thisLen;
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
      if (thisY > 0) {
        newy = thisY - 1;
        if (map[thisX][newy]) {
          queueX[queueEnd] = thisX;
          queueY[queueEnd] = newy;
          queueLen[queueEnd] = thisLen;
          queueEnd = (queueEnd + 1) % queueSize;
        }
      }
    }
  }
  *exitcode = 1;
}


void findMaxWord(unsigned int* bestLen, char** bestWord, unsigned int* bestPosX, unsigned int* bestPosY, char* bestChar, unsigned int** bestPathX, unsigned int** bestPathY, char* exitcode, unsigned int sizex, unsigned int sizey, char** map /*similar to map[sizex][sizey]*/, char* alphabet, unsigned int alphabetLength, char** words, unsigned int countWordGroups, unsigned int maxWordLength) {
  // We assume that words is sorted by length from highest to least.
  // It fond only one of max-length word, there might be others.

  // Thouse as output shoud be defined as:
  // ({
  //   unsigned int bestLen = 0;
  //   char* bestWord = (char*)malloc(maxWordLength * sizeof(char));
  //   unsigned int bestPosX = 0;
  //   unsigned int bestPosY = 0;
  //   char bestChar = 0;
  //   unsigned int* bestPathX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  //   unsigned int* bestPathY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  //   char exitcode;
  //  })

  unsigned int* posesX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));  // yeah, do not optimised, because why the heck do we need 4294967295 length map?
  unsigned int* posesY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));  // yeah, do not optimised, because why the heck do we need 4294967295 length map?
  unsigned int ind = 0;

  // add all possition that are empty and have at least one non empty neighbor.
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y] == 0) {
        if (x+1 < sizex) {
          if (map[x+1][y]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (y+1 < sizey) {
          if (map[x][y+1]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (x > 0) {
          if (map[x-1][y]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (y > 0) {
          if (map[x][y-1]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
      }
    }
  }

  *exitcode = 0;
  for (unsigned int i = 0; i < ind; ++i) {
    unsigned int x = posesX[i];
    unsigned int y = posesY[i];
    map[x][y] = 255;
    char exitcodeMaxPath = 0;
    unsigned int maxPathLen = 0;
    findMaxPath(&exitcodeMaxPath, &maxPathLen, sizex, sizey, map);
    //++maxPathLen;
    //printf("%i\n", maxPathLen);
    //printf("i=%2i: %2i %2i\n", i, x, y);
    for (unsigned int j = 0; j < alphabetLength; ++j) {
      //printf("  %s:\n", num2charEN(alphabet[j]));
      map[x][y] = alphabet[j];
      for (unsigned int groupId = 0; groupId < countWordGroups; ++groupId) {
        //printf("    %i\n", groupId);
        unsigned int wordsLength = *(unsigned int*)(words[groupId] + 4);
        if (wordsLength <= *bestLen || wordsLength > maxPathLen) continue;
        unsigned int countWords = *(unsigned int*)words[groupId];
        char* newWords = words[groupId] + 8;
        unsigned int* pathXRes = NULL;
        unsigned int* pathYRes = NULL;
        char exitcodePath = 0;
        char* word;  // = (char*)malloc(wordsLength * sizeof(char));

        for (unsigned int wordId = 0; wordId < countWords; ++wordId) {
          //if (groupId == 21) printf("  %i: %i (%i)\n", wordId, wordId * wordsLength, wordsLength);
          word = &newWords[wordId * wordsLength];

          findPath(&exitcodePath, word, wordsLength, sizex, sizey, map, x, y, &pathXRes, &pathYRes);

          if (exitcodePath) {
            *exitcode = 1;
            *bestLen = wordsLength;
            printf("len=%2i, index=%5i:\n", wordsLength, wordId);
            for (unsigned int k = 0; k < wordsLength; ++k) {
              printf("%2i (%s) -> ", (*bestWord)[k], num2charEN((*bestWord)[k]));
              (*bestWord)[k] = word[k];
              printf("%2i (%s)\n", (*bestWord)[k], num2charEN((*bestWord)[k]));
            }
            *bestPosX = x;
            *bestPosY = y;
            *bestChar = alphabet[j];
            //if (pathXRes == NULL || pathYRes == NULL) {
            //  fprintf(stderr, "Указатели не инициализированы!\n");
            //  exit(EXIT_FAILURE);
            //}
            for (unsigned int k = 0; k < wordsLength; ++k) {
              //printf("%i %i\n", pathXRes[k], pathYRes[k]);
              (*bestPathX)[k] = pathXRes[k];
              (*bestPathY)[k] = pathYRes[k];
              //printf("%i %i\n\n", bestPathX[k], bestPathY[k]);
            }
            //printf("=======\n");
            free(pathXRes); free(pathYRes);
            break;
          }
        }
        //printf("-1 %i %i\n", wordsLength, *bestLen);
        if (wordsLength == *bestLen) break;
      }

      map[x][y] = 0;  // do not necessary but should be
    }
  }
  free(posesX); free(posesY);
}


void excludeWord(int *exitcode, char* word, unsigned int thisWordLength, char*** words, unsigned int countWordGroups) {
  *exitcode = 0;
  for (unsigned int groupId = 0; groupId < countWordGroups; ++groupId) {
    unsigned int wordsLength = *(unsigned int*)((*words)[groupId] + 4);
    if (wordsLength != thisWordLength) continue;
    unsigned int countWords = *(unsigned int*)(*words)[groupId];
    char* newWords = (*words)[groupId] + 8;
    char* newWord;
    for (unsigned int index = 0; index < countWords; ++index) {
      newWord = &newWords[index * wordsLength];
      char add = 1;
      for (unsigned int k = 0; k < thisWordLength; ++k) {
        if (word[k] != newWord[k]) {
          //printf("%s vs %s\n", num2charEN(word[k]), num2charEN(newWord[k]));
          add = 0;
          break;
        }
      }
      if (add) {
        printf("len=%2i, index=%5i:\n", wordsLength, index);
        --countWords;
        for (unsigned int k = 0; k < thisWordLength; ++k) {
          printf("%2i (%s) -> ", newWord[k], num2charEN(newWord[k]));
          newWord[k] = newWords[countWords * wordsLength + k];
          printf("%2i (%s)\n", newWord[k], num2charEN(newWord[k]));
        }
        //printf("%i ", sizeof(newWords));
        newWords = (char*)realloc(newWords - 8, countWords * wordsLength * sizeof(char) + 8);
        (*words)[groupId] = newWords;
        //printf("%i\n", sizeof((*words)[groupId]));
        //printf("%i ", countWords);
        writePointerAsInt(&countWords, (*words)[groupId]);
        //printf("%i\n", *(unsigned int*)(*words)[groupId]);
        break;
      }
    }
    *exitcode = 1;
    break;
  }
}


// Heh, there is nothing to divide into threads. This is useless.
void findMaxWordThread();


void findPathTrie(char* exitcode, struct TrieNode* words, unsigned int wordLength, unsigned int sizex, unsigned int sizey, char** map, unsigned int addx, unsigned int addy, unsigned int** pathXRes, unsigned int** pathYRes, char** wordRes) {
  unsigned int* posesX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* posesY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int ind = 0;

  // find all possitions with non-zero values (where letters are).
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y]) {
        posesX[ind] = x;
        posesY[ind] = y;
        ++ind;
      }
    }
  }

  if (ind == 0) {
    *exitcode = 0;
    free(posesX); free(posesY);
    return;
  }

  struct QueueElement {
    unsigned int x;
    unsigned int y;
    unsigned int idx;
    struct TrieNode* trie;
    unsigned int* pathX;
    unsigned int* pathY;
    char** visited;
  };

  struct Queue {
    unsigned int size;
    unsigned int start;
    unsigned int end;
    struct QueueElement* elements;
  };

  struct Queue queue;
  queue.size = sizex * sizey * sizex * sizey;  // wordLength * wordLength;  // Might be larger, needs testing.
  queue.start = 0;
  queue.end = 0;
  queue.elements = (struct QueueElement*)malloc(queue.size * sizeof(struct QueueElement));
  if (!queue.elements) {
    *exitcode = 5;
    goto cleanupSafe;
  }
  for (unsigned int i = 0; i < queue.size; ++i) {
    queue.elements[i].pathX = (unsigned int*)malloc(wordLength * sizeof(unsigned int));
    if (!queue.elements[i].pathX) {
      *exitcode = 5;
      goto cleanupSafe;
    }
    queue.elements[i].pathY = (unsigned int*)malloc(wordLength * sizeof(unsigned int));
    if (!queue.elements[i].pathY) {
      *exitcode = 5;
      goto cleanupSafe;
    }
    queue.elements[i].visited = malloc(sizex * sizeof(char*));
    if (!queue.elements[i].visited) {
      *exitcode = 5;
      goto cleanupSafe;
    }
    for (unsigned int x = 0; x < sizex; ++x) {
      queue.elements[i].visited[x] = malloc(sizey * sizeof(char));
      if (!queue.elements[i].visited[x]) {
        *exitcode = 5;
        goto cleanupSafe;
      }
    }
  }
  --wordLength;  // to account that indexes start with 0.

  for (unsigned int i = 0; i < ind; ++i) {
    //printf("Setuping new queue.\n");
    unsigned int x = posesX[i];
    unsigned int y = posesY[i];
    queue.start = 0;
    queue.end = 0;
    // begin queue with each position
    ++queue.end;
    queue.elements[0].x = x;
    queue.elements[0].y = y;
    queue.elements[0].idx = 0;
    queue.elements[0].trie = words;
    for (unsigned int x = 0; x < sizex; ++x) {
      for (unsigned int y = 0; y < sizey; ++y) {
        if (map[x][y]) {
          queue.elements[0].visited[x][y] = 0;
        } else {
          queue.elements[0].visited[x][y] = 1;
        }
      }
    }
    struct QueueElement thisElement;
    while (queue.start != queue.end) {
      thisElement = queue.elements[queue.start];
      if (thisElement.visited[thisElement.x][thisElement.y]) {
        queue.start = (queue.start + 1) % queue.size;
        continue;
      }
      thisElement.visited[thisElement.x][thisElement.y] = 1;
      thisElement.pathX[thisElement.idx] = thisElement.x;
      thisElement.pathY[thisElement.idx] = thisElement.y;
      if (thisElement.idx == wordLength) {
        char add = 0;
        //printf("Backtrace:\n");
        //backtraceTrie(thisElement.trie, (void*)0);
        //printf("\n");
        struct TrieNode* currentNode = words;
        //printf("j up to %i:\n", wordLength);
        for (unsigned int j = 0; j <= wordLength; ++j) {
          //printf("j=%i\n", j);
          if (thisElement.pathX[j] == addx && thisElement.pathY[j] == addy) {
            add = 1;
            //break;
          }
          ////if (wordLength == 2) showTrieDeep(*currentNode, wordLength, 1, 0, 0, -1);
          if (currentNode->hasChildren == 0 && j < wordLength) {
            add = 0;
            break;
          } else if ((currentNode->children[map[thisElement.pathX[j]][thisElement.pathY[j]]]).hasChildren == 0) {
            add = 0;
            break;
          }
          currentNode = &(currentNode->children[map[thisElement.pathX[j]][thisElement.pathY[j]]]);
        }
        if (add) {
          *exitcode = 1;
          *pathXRes = (unsigned int*)malloc((wordLength+1) * sizeof(unsigned int));
          *pathYRes = (unsigned int*)malloc((wordLength+1) * sizeof(unsigned int));
          //printf("\nlen=%i:\n", wordLength+1);
          for (unsigned int j = 0; j <= wordLength; ++j) {
            //printf("(%i %i) -> ", thisElement.pathX[j], thisElement.pathY[j]);
            (*pathXRes)[j] = thisElement.pathX[j];
            (*pathYRes)[j] = thisElement.pathY[j];
            //printf("(%i %i) char -> ", (*pathXRes)[j], (*pathYRes)[j]);
            (*wordRes)[j] = map[thisElement.pathX[j]][thisElement.pathY[j]];
            //printf("%s (%i)\n", num2charEN((*wordRes)[j]), (*wordRes)[j]);
          }
          goto cleanupSafe;
          return;
        }
        queue.start = (queue.start + 1) % queue.size;
        continue;
      }
      ++thisElement.idx;
      unsigned int newx, newy;
      newx = thisElement.x + 1;
      if (newx < sizex) {
        if (map[newx][thisElement.y] > (*thisElement.trie).count) {
          fprintf(stderr, "\033[91m[E]\033[m index out of range (%i > %i)!\n", map[newx][thisElement.y], (*thisElement.trie).count);
          *exitcode = 3;
          goto cleanupSafe;
          return;
        }
        if ((*thisElement.trie).children == NULL) {
          fprintf(stderr, "\033[91m[E]\033[m tryed to access trie`s childer while they were not initialized!\n");
          *exitcode = 4;
          goto cleanupSafe;
          return;
        }
        //printf("1 %i %i\n", (*thisElement.trie).children[map[newx][thisElement.y]].hasChildren, map[newx][thisElement.y]);
        if ((*thisElement.trie).children[map[newx][thisElement.y]].hasChildren && thisElement.visited[newx][thisElement.y] == 0) {
          queue.elements[queue.end].x = newx;
          queue.elements[queue.end].y = thisElement.y;
          queue.elements[queue.end].trie = &((*thisElement.trie).children[map[newx][thisElement.y]]);
          queue.elements[queue.end].idx = thisElement.idx;
          for (unsigned int j = 0; j < thisElement.idx; ++j) {
            queue.elements[queue.end].pathX[j] = thisElement.pathX[j];
            queue.elements[queue.end].pathY[j] = thisElement.pathY[j];
          }
          for (unsigned int x = 0; x < sizex; ++x) {
            for (unsigned int y = 0; y < sizey; ++y) {
              queue.elements[queue.end].visited[x][y] = thisElement.visited[x][y];
            }
          }
          queue.end = (queue.end + 1) % queue.size;
        }
      }
      newy = thisElement.y + 1;
      if (newy < sizey) {
        if (map[thisElement.x][newy] > (*thisElement.trie).count) {
          fprintf(stderr, "\033[91m[E]\033[m index out of range (%i > %i)!\n", map[thisElement.x][newy], (*thisElement.trie).count);
          *exitcode = 3;
          goto cleanupSafe;
          return;
        }
        if ((*thisElement.trie).children == NULL) {
          fprintf(stderr, "\033[91m[E]\033[m tryed to access trie`s childer while they were not initialized!\n");
          *exitcode = 4;
          goto cleanupSafe;
          return;
        }
        //printf("2 %i %i\n", (*thisElement.trie).children[map[newx][thisElement.y]].hasChildren, map[newx][thisElement.y]);
        if ((*thisElement.trie).children[map[thisElement.x][newy]].hasChildren && thisElement.visited[thisElement.x][newy] == 0) {
          queue.elements[queue.end].x = thisElement.x;
          queue.elements[queue.end].y = newy;
          queue.elements[queue.end].trie = &((*thisElement.trie).children[map[thisElement.x][newy]]);
          queue.elements[queue.end].idx = thisElement.idx;
          for (unsigned int j = 0; j < thisElement.idx; ++j) {
            queue.elements[queue.end].pathX[j] = thisElement.pathX[j];
            queue.elements[queue.end].pathY[j] = thisElement.pathY[j];
          }
          for (unsigned int x = 0; x < sizex; ++x) {
            for (unsigned int y = 0; y < sizey; ++y) {
              queue.elements[queue.end].visited[x][y] = thisElement.visited[x][y];
            }
          }
          queue.end = (queue.end + 1) % queue.size;
        }
      }
      if (thisElement.x > 0) {
        newx = thisElement.x - 1;
        if (map[newx][thisElement.y] > (*thisElement.trie).count) {
          fprintf(stderr, "\033[91m[E]\033[m index out of range (%i > %i)!\n", map[newx][thisElement.y], (*thisElement.trie).count);
          *exitcode = 3;
          goto cleanupSafe;
          return;
        }
        if ((*thisElement.trie).children == NULL) {
          fprintf(stderr, "\033[91m[E]\033[m tryed to access trie`s childer while they were not initialized!\n");
          *exitcode = 4;
          goto cleanupSafe;
          return;
        }
        //printf("3 %i %i\n", (*thisElement.trie).children[map[newx][thisElement.y]].hasChildren, map[newx][thisElement.y]);
        if ((*thisElement.trie).children[map[newx][thisElement.y]].hasChildren && thisElement.visited[newx][thisElement.y] == 0) {
          queue.elements[queue.end].x = newx;
          queue.elements[queue.end].y = thisElement.y;
          queue.elements[queue.end].trie = &((*thisElement.trie).children[map[newx][thisElement.y]]);
          queue.elements[queue.end].idx = thisElement.idx;
          for (unsigned int j = 0; j < thisElement.idx; ++j) {
            queue.elements[queue.end].pathX[j] = thisElement.pathX[j];
            queue.elements[queue.end].pathY[j] = thisElement.pathY[j];
          }
          for (unsigned int x = 0; x < sizex; ++x) {
            for (unsigned int y = 0; y < sizey; ++y) {
              queue.elements[queue.end].visited[x][y] = thisElement.visited[x][y];
            }
          }
          queue.end = (queue.end + 1) % queue.size;
        }
      }
      if (thisElement.y > 0) {
        newy = thisElement.y - 1;
        if (map[thisElement.x][newy] > (*thisElement.trie).count) {
          fprintf(stderr, "\033[91m[E]\033[m index out of range (%i > %i)!\n", map[thisElement.x][newy], (*thisElement.trie).count);
          *exitcode = 3;
          goto cleanupSafe;
          return;
        }
        if ((*thisElement.trie).children == NULL) {
          fprintf(stderr, "\033[91m[E]\033[m tryed to access trie`s childer while they were not initialized!\n");
          *exitcode = 4;
          goto cleanupSafe;
          return;
        }
        //printf("4 %i %i\n", (*thisElement.trie).children[map[newx][thisElement.y]].hasChildren, map[newx][thisElement.y]);
        if ((*thisElement.trie).children[map[thisElement.x][newy]].hasChildren && thisElement.visited[thisElement.x][newy] == 0) {
          queue.elements[queue.end].x = thisElement.x;
          queue.elements[queue.end].y = newy;
          queue.elements[queue.end].trie = &((*thisElement.trie).children[map[thisElement.x][newy]]);
          queue.elements[queue.end].idx = thisElement.idx;
          for (unsigned int j = 0; j < thisElement.idx; ++j) {
            queue.elements[queue.end].pathX[j] = thisElement.pathX[j];
            queue.elements[queue.end].pathY[j] = thisElement.pathY[j];
          }
          for (unsigned int x = 0; x < sizex; ++x) {
            for (unsigned int y = 0; y < sizey; ++y) {
              queue.elements[queue.end].visited[x][y] = thisElement.visited[x][y];
            }
          }
          queue.end = (queue.end + 1) % queue.size;
        }
      }
      if (queue.end == queue.start) {
        fprintf(stderr, "\033[91m[E]\033[m Queue run out!\n");
        *exitcode = 2;
        goto cleanupSafe;
        return;
      }
    }
  }
  *exitcode = 0;

  goto cleanupSafe;
  return;

  cleanup:
  for (unsigned int i = 0; i < queue.size; ++i) {
    free(queue.elements[i].pathX);
    free(queue.elements[i].pathY);
    for (unsigned int x = 0; x < sizex; ++x) free(queue.elements[i].visited[x]);
    free(queue.elements[i].visited);
  }
  free(queue.elements);
  return;
  cleanupSafe:
  //printf("Begin safe cleanup...\n");
  if (queue.elements) {
    for (unsigned int i = 0; i < queue.size; ++i) {
      if (queue.elements[i].pathX) free(queue.elements[i].pathX);
      if (queue.elements[i].pathY) free(queue.elements[i].pathY);
      if (queue.elements[i].visited) {
        for (unsigned int x = 0; x < sizex; ++x) {
          if (queue.elements[i].visited[x]) free(queue.elements[i].visited[x]);
        }
        free(queue.elements[i].visited);
      }
    }
    free(queue.elements);
  }
  //printf("Safely cleaned up!\n");
}



void findMaxWordTrie(unsigned int* bestLen, char** bestWord, unsigned int* bestPosX, unsigned int* bestPosY, char* bestChar, unsigned int** bestPathX, unsigned int** bestPathY, char* exitcode, unsigned int sizex, unsigned int sizey, char** map, char* alphabet, unsigned int alphabetLength, struct TrieNode* words, unsigned int maxWordLength) {
  // We assume that words is sorted by length from highest to least.
  // It fond only one of max-lengthed words, there might be others.

  unsigned int* posesX = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int* posesY = (unsigned int*)malloc(sizex * sizey * sizeof(unsigned int));
  unsigned int ind = 0;

  // add all possition that are empty and have at least one non empty neighbor, so that we can place a letter.
  for (unsigned int x = 0; x < sizex; ++x) {
    for (unsigned int y = 0; y < sizey; ++y) {
      if (map[x][y] == 0) {
        if (x+1 < sizex) {
          if (map[x+1][y]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (y+1 < sizey) {
          if (map[x][y+1]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (x > 0) {
          if (map[x-1][y]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
        if (y > 0) {
          if (map[x][y-1]) {
            char add = 1;
            for (int i = 0; i < ind && add; ++i) {
              if (x == posesX[i] && y == posesY[i]) add = 0;
            }
            if (add) {
              posesX[ind] = x;
              posesY[ind] = y;
              ++ind;
            }
          }
        }
      }
    }
  }

  *exitcode = 0;
  for (unsigned int i = 0; i < ind; ++i) {
    unsigned int x = posesX[i];
    unsigned int y = posesY[i];
    map[x][y] = 255;
    char exitcodeMaxPath = 0;
    unsigned int maxPathLen = 0;
    findMaxPath(&exitcodeMaxPath, &maxPathLen, sizex, sizey, map);
    //++maxPathLen;
    //++maxPathLen;
    //char** mapC = (char**)malloc(sizex*sizeof(char*));
    //for (unsigned int x1 = 0; x1 < sizex; ++x1) {
    //  map[x1] = (char*)malloc(sizey*sizeof(char));
    //}
    for (unsigned int j = 0; j < alphabetLength; ++j) {
      //for (unsigned int x1 = 0; x1 < sizex; ++x1) {
      //  for (unsigned int y1 = 0; y1 < sizey; ++y1) {
      //    mapC[x1][y1] = map[x1][y1];
      //  }
      //}
      map[x][y] = alphabet[j];
      for (unsigned int wordsLength = words->count - 1; wordsLength > 0; --wordsLength) {
        //printf("Trying for: %i.\n", wordsLength);
        if (wordsLength <= *bestLen || wordsLength > maxPathLen || words->children[wordsLength].hasChildren == 0) continue;
        //printf("Success: %i.\n", wordsLength);
        struct TrieNode* newWords = &(words->children[wordsLength]);
        unsigned int* pathXRes = NULL;
        unsigned int* pathYRes = NULL;
        char exitcodePath = 0;
        //printf("Entering with wordsLength=%i\n", wordsLength);
        findPathTrie(&exitcodePath, newWords, wordsLength, sizex, sizey, map, x, y, &pathXRes, &pathYRes, bestWord);
        //printf("exitcodePath=%i\n", exitcodePath);
        if (exitcodePath == 5) {
          fprintf(stderr, "\033[91m[E]\033[m something went wrong while making pointers.\n");
        }
        if (exitcodePath == 1) {
          *exitcode = 1;
          *bestLen = wordsLength;
          //printf("len=%2i: ", wordsLength);
          //for (unsigned int k = 0; k < wordsLength; ++k) {
          //  printf("%2i (%s) -> ", (*bestWord)[k], num2charEN((*bestWord)[k]));
          //  (*bestWord)[k] = word[k];
          //  printf("%2i (%s)\n", (*bestWord)[k], num2charEN((*bestWord)[k]));
          //}
          //for (unsigned int k = 0; k < wordsLength; ++k) printf("%s", num2charEN((*bestWord)[k]));
          //printf("\n");
          *bestPosX = x;
          //printf("1\n");
          *bestPosY = y;
          //printf("1\n");
          *bestChar = alphabet[j];
          //printf("1\n");
          for (unsigned int k = 0; k < wordsLength; ++k) {
            (*bestPathX)[k] = pathXRes[k];
            (*bestPathY)[k] = pathYRes[k];
          }
          //printf("1\n");
          //free(pathXRes);
          //printf("1\n"); free(pathYRes);
          //printf("1\n");
          break;
        }
      }
      //printf("Ended loop 1\n");
    }
    map[x][y] = 0;  // do not necessary but should be
  }
  //free(posesX); free(posesY);
  //*/
}
