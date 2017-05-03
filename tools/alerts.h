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

  #define ERRORV(...)\
  {\
      fprintf(stderr, YELLOW "PID %d:  " ERR UL "@%d" UN_UL "  ", getpid(), __LINE__);\
      fprintf(stderr, __VA_ARGS__);\
      putc   ('\n', stderr);\
      fprintf(stderr, "\nERRNO: %s\n", strerror(errno));\
      putc   ('\n', stderr);\
      exit   (EXIT_FAILURE);\
  }

  #ifdef DEBUG
    #define PRINT(...)\
    {\
      fprintf(stdout, YELLOW "PID %d:  " NORM, getpid());\
      fprintf(stdout, __VA_ARGS__);\
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

  #define ERRORV(...)\
  {\
    fprintf(stderr, ERR UL "@%d" UN_UL "  ", __LINE__);\
    fprintf(stderr, __VA_ARGS__);\
    putc   ('\n', stderr);\
    fprintf(stderr, "\nERRNO: %s\n", strerror(errno));\
    putc   ('\n', stderr);\
    exit   (EXIT_FAILURE);\
  }

  #ifdef DEBUG
    #define PRINT(...)\
    {\
      fprintf(stdout, LBLUE);\
      fprintf(stdout, __VA_ARGS__);\
      fprintf(stdout, NORM "\n");\
    }
  #else  // DEBUG
    #define PRINT(...)
  #endif // DEGUG
#endif // MSGPID


#define ERRTEST(...) \
    if ((__VA_ARGS__) == -1) \
        ERRORV("Code line: [" #__VA_ARGS__ "]")

#endif // ALERTS_H
