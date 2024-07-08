#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define MAXTOKENCOUNT 20
#define MAXTOKENLENGTH 20
#define MAXLINELENGTH 400

struct cell{
int position;
int adjcount;
int mined;
int covered;
int flagged;
};
typedef struct cell cell;

cell **board;

int rows;
int cols;
int mines;

int neighborcount = 8;
int rowneighbors[] = {-1, -1, +0, +1, +1, +1, +0, -1};
int colneighbors[] = {+0, +1, +1, +1, +0, -1, -1, -1};

void coverall(){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      board[i][j].covered = 1;
    }
  }
}

void uncoverall(){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      board[i][j].covered = 0;
    }
  }
}

void init_cell(cell *c, int p){
  c->position = p;
  c->adjcount = 0;
  c->mined = 0;
  c->covered = 1;
  c->flagged = 0;
}

int get_random(int range){
  return ((int)floor((float)range * rand() / RAND_MAX)) % range;
}

void place_mines(){
  for(int i = 0; i < mines; i++){
    int r = i / cols;
    int c = i % cols;
    board[r][c].mined = 1;
  }
  int shuffles = 5000;
  for(int i = 0; i < shuffles; i++){
    int r1 = get_random(rows);
    int c1 = get_random(cols);
    int r2 = get_random(rows);
    int c2 = get_random(cols);

    int temp = board[r1][c1].mined;
    board[r1][c1].mined = board[r2][c2].mined;
    board[r2][c2].mined = temp;
  }
}

void find_adjcount(){
  for(int r = 0; r < rows; r++){
    for(int c = 0; c < cols; c++){
      int minecount = 0;
      for(int d = 0; d < neighborcount; d++){
        int rn = r + rowneighbors[d];
        int cn = c + colneighbors[d];
        if(0 <= rn && rn < rows && 0 <= cn && cn < cols && board[rn][cn].mined == 1){
          minecount++;
        }
      }
      board[r][c].adjcount = minecount;
    }
  }
}

void command_new(int rows, int cols, int mines){
  board = (cell **)malloc(sizeof(cell *) * rows);
  for(int i = 0; i < rows; i++){
    board[i] = (cell *)malloc(sizeof(cell) * cols);
  }

  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      int p = i * cols + j;
      init_cell(&board[i][j], p);
    }
  }

  place_mines();
  find_adjcount();
}

void display_cell(cell *c){
  if(c->flagged == 1){
    printf("%2s", "P");
  }
  else if(c->covered == 1){
    printf("%2s", "/");
  }
  else if(c->mined == 1){
    printf("%2s", "*");
  }
  else if(c->adjcount > 0){
    printf("%2d", c->adjcount);
  }
  else{
    printf("%2s", ".");
  }
}

void command_show(){
  for(int i = 0; i < rows; i++){
    for(int j = 0; j < cols; j++){
      display_cell(&board[i][j]);
    }
    printf("\n");
  }
}

void command_flag(int r, int c){
  if(board[r][c].flagged == 0){
    board[r][c].flagged = 1;
  }
  else{
    printf("The cell is already flagged\n");
  }
}

void command_unflag(int r, int c){
  if(board[r][c].flagged == 1){
    board[r][c].flagged = 0;
  }
  else{
    printf("The cell is already unflagged\n");
  }
}

void uncover_recursive(int r, int c){
  board[r][c].covered = 0;
  if(board[r][c].adjcount == 0){
    for(int d = 0; d < neighborcount; d++){
      int rn = r + rowneighbors[d];
      int cn = c + colneighbors[d];
      if(0 <= rn && rn < rows && 0 <= cn && cn < cols && board[rn][cn].covered == 1){
        uncover_recursive(rn, cn);
      }
    }
  }
}

void get_line(char line[], int maxlinelen){
  fgets(line, maxlinelen, stdin);
  line[strlen(line) - 1] = '\0';
  //printf("You entered %s\n", line);
}

void get_tokens(char line[], char tokens[][MAXTOKENLENGTH], int *count){
  char linecopy[MAXLINELENGTH];
  strcpy(linecopy, line);
  
  char *tok = strtok(linecopy, " ");
  while(tok != NULL){
    //printf("Current Token = %s\n", tok);
    strcpy(tokens[*count], tok);
    (*count)++;
    tok = strtok(NULL, " ");
  }
  //printf("Total Tokens = %d\n", *count);
}

int process_command(char tokens[][MAXTOKENLENGTH], int *tokencount){
  char command[MAXTOKENLENGTH];
  strcpy(command, tokens[0]);
  
  if(strcmp(command, "quit") == 0){
    printf("Bye!\n");
    return 0;
  }
  else if(strcmp(command, "new") == 0){
    rows = atoi(tokens[1]);
    cols = atoi(tokens[2]);
    mines = atoi(tokens[3]);
    command_new(rows, cols, mines);
    printf("New Board Command\n");
  }
  else if(strcmp(command, "show") == 0){
    command_show();
    printf("Show Board Command\n");
  }
  else if(strcmp(command, "flag") == 0){
    int row, col;

    row = atoi(tokens[1]);
    col = atoi(tokens[2]);
    command_flag(row, col);
    printf("Flag Cell Command\n");
  }
  else if(strcmp(command, "unflag") == 0){
    int row, col;

    row = atoi(tokens[1]);
    col = atoi(tokens[2]);
    command_unflag(row, col);
    printf("Unflag Cell Command\n");
  }
  else if(strcmp(command, "uncover") == 0){
    int row, col;

    row = atoi(tokens[1]);
    col = atoi(tokens[2]);
    uncover_recursive(row, col);
    printf("Uncover Cell Command\n");
  }
  return 1;
}

void rungame(){
  char line[MAXLINELENGTH];
  char tokens[MAXTOKENCOUNT][MAXTOKENLENGTH];
  int tokencount = 0;

  while(1){
    printf(">>");
    get_line(line, MAXLINELENGTH);
    get_tokens(line, tokens, &tokencount);
    int result = process_command(tokens, &tokencount);
    if(result == 0){
      break;
    }
    tokencount = 0;
  }
}

int main(void){
  srand(time(0));
  rungame();
  return 0;
}