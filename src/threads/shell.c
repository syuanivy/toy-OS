/*
 * shell.c
 *
 *  Created on: May 10, 2015
 *      Author: rreeves
 */
 
 #include <stdio.h>
 #include <string.h>

void run_shell() {
  printf("\nStarting the osOS shell...\n");
  
  char input[100];
  while (true) {
    
    memset(input, 0, 100);
    int index = 0;
    
    uart_puts("\nosO$ "); 
    char inputc = uart_getc();
    
    while (inputc != 13) {
      uart_putc(inputc);
      input[index++] = inputc;
      
      inputc = uart_getc();
    }
    
    input[index] = '\0';
    
    if (!strcmp(input, "help")) {
      printf("\nts - thread status - show running threads and their run times");
      printf("\nrun <func> - launch a thread function and wait for its completion");
      printf("\nbg <func> - launch a command in the background");
      printf("\nshutdown - shutdown the operating system");
    }
    else if (!strcmp(input, "ts")) {
        printf("\nTODO - ts command");
    }
    else if (
      input[0] == 'r'
      && input[1] == 'u'
      && input[2] == 'n'
      && input[3] == ' '
    ) {
      printf("\nTODO - run command");
    }
    else if (
      input[0] == 'b'
      && input[1] == 'g'
      && input[2] == ' '
    ) {
      printf("\nTODO - bg command");
    }
    else if (strcmp(input, "shutdown") == 0) {
      break;
    }
    else {
      printf("\nUnknown command. Enter 'help' for list of commands.");
    }
  }
  
  printf("\nGoodbye");
}