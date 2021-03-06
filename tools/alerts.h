#ifndef ALERTS_H
#define ALERTS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "colors.h"

// Messages
#define OK     BOLD GREEN  "[OK]  "    NORM
#define ERR    BOLD RED    "[ERROR]  " NORM
#define EXIT   BOLD YELLOW "[EXIT]  "  NORM
#define SPACE              "      "
#define LINEUP             "\033[A"

char __ALERTS_BUFFER1[BUFSIZ],
     __ALERTS_BUFFER2[BUFSIZ];

#ifdef  MSGPID
  #define ERROR(...)\
  {\
    sprintf(__ALERTS_BUFFER1, YELLOW "PID %d:  " ERR UL "@%d" UN_UL "  ", getpid(), __LINE__);\
    sprintf(__ALERTS_BUFFER2, __VA_ARGS__);\
    fprintf(stderr, "%s%s\n", __ALERTS_BUFFER1, __ALERTS_BUFFER2);\
    exit   (EXIT_FAILURE);\
  }

  #define ERRORV(...)\
  {\
    sprintf(__ALERTS_BUFFER1, YELLOW "PID %d:  " ERR UL "@%d" UN_UL "  ", getpid(), __LINE__);\
    sprintf(__ALERTS_BUFFER2, __VA_ARGS__);\
    fprintf(stderr, "%s%s\n- %s\n", __ALERTS_BUFFER1, __ALERTS_BUFFER2, strerror(errno));\
    exit   (EXIT_FAILURE);\
  }

  #define PRINT(...)\
    {\
      sprintf(__ALERTS_BUFFER1, YELLOW "PID %d:  " NORM, getpid());\
      sprintf(__ALERTS_BUFFER2, __VA_ARGS__);\
      fprintf(stdout, "%s%s", __ALERTS_BUFFER1, __ALERTS_BUFFER2);\
      fflush(stdout);\
    }

#else  // MSGPID
  #define ERROR(...)\
  {\
    sprintf(__ALERTS_BUFFER1, ERR UL "@%d" UN_UL "  ", __LINE__);\
    sprintf(__ALERTS_BUFFER2, __VA_ARGS__);\
    fprintf(stderr, "%s%s\n", __ALERTS_BUFFER1, __ALERTS_BUFFER2);\
    exit   (EXIT_FAILURE);\
  }

  
  #define ERRORV(...)\
  {\
    sprintf(__ALERTS_BUFFER1, ERR UL "@%d" UN_UL "  ", __LINE__);\
    sprintf(__ALERTS_BUFFER2, __VA_ARGS__);\
    fprintf(stderr, "%s%s\n- %s\n", __ALERTS_BUFFER1, __ALERTS_BUFFER2, strerror(errno));\
    exit   (EXIT_FAILURE);\
  }

  #define PRINT(...)\
  {\
    sprintf(__ALERTS_BUFFER1, LGRAY);\
    sprintf(__ALERTS_BUFFER2,  __VA_ARGS__);\
    fprintf(stdout, "%s%s" NORM, __ALERTS_BUFFER1, __ALERTS_BUFFER2);\
    fflush(stdout);\
  }

#endif // MSGPID

#define PRINTLN(...) \
{\
  PRINT(__VA_ARGS__); \
  fprintf(stdout, "\n");\
  fflush(stdout);\
}


#ifdef DEBUG
    #define DBG_PRINT(...) PRINTLN(__VA_ARGS__)
#else  // DEBUG
    #define DBG_PRINT(...)
#endif // DEGUG


#define ERRTEST(...) \
    if ((__VA_ARGS__) == -1) \
        ERRORV("Code line: [" #__VA_ARGS__ "]")

#endif // ALERTS_H
