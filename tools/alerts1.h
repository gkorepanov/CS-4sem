#ifndef ALERTS_H
#define ALERTS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "colors.h"

// Messages
#define OK     BOLD GREEN  "[OK]  "    NORM
#define ERR    BOLD RED    "[ERROR]  " NORM
#define EXIT   BOLD YELLOW "[EXIT]  "  NORM
#define SPACE              "      "
#define LINEUP             "\033[A"

#ifdef  MSGPID
  #define ERROR(...)\
  {\
    fprintf(stderr, YELLOW "PID %d:  " ERR UL "@%d" UN_UL "  ", getpid(), __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    putc   ('\n', stderr);\
    exit   (EXIT_FAILURE);\
  }

  #ifdef DEBUG
    #define PRINT(...)\
    {\
      fprintf(stderr, YELLOW "PID %d:  " NORM, getpid());\
      fprintf(stderr, __VA_ARGS__);\
      putc   ('\n', stderr);\
    }
  #else  // DEBUG
    #define PRINT(...)
  #endif // DEGUG

#else  // MSGPID
  #define ERROR(...)\
  {\
    fprintf(stderr, ERR UL "@%d" UN_UL "  ", __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    putc   ('\n', stderr);\
    exit   (EXIT_FAILURE);\
  }

  #ifdef DEBUG
    #define PRINT(...)\
    {\
      fprintf(stderr, LBLUE);\
      fprintf(stderr, __VA_ARGS__);\
      fprintf(stderr, NORM "\n");\
    }
  #else  // DEBUG
    #define PRINT(...)
  #endif // DEGUG
#endif // MSGPID

#endif // ALERTS_H
