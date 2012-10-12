#define HAS_CURSES
#define AVR

#if define(HAS_CURSES)&&define(AVR)
# error AVRとHAS_CURSESが両方共defineされています
#endif

#include <stdlib.h>
#include <time.h>
#include <unistd.h>


//PC用
#if define HAS_CURSES
#include <stdio.h>
#include <signal.h>
#include <ncurses.h>
#endif

typedef struct{
 unsigned int LED_height;
 unsigned int LED_width;
 unsigned int LCD_height;
 unsigned int LCD_width;
}Tetris_Hard_Info;

#if define HAS_CURSES
typedef struct{
 WINDOW *LCD;
 WINDOW *realLCD;
 WINDOW *LED;
}Tetris_Ncurses_Display;
#endif

typedef struct{
 unsigned int block_down_time;
 unsigned int map_x;
 unsigned int map_y;
}Tetris_Data_List;

typedef struct{
 int block_x;
 int block_y;
 int next_block_no;
 int using_block[4][4];

 unsigned int deletedLine;
 unsigned int score;

 int generateFlag;
 int endFlag;
 int flame;
 int map[16][10];
}Tetris_Data;

typedef struct{
 Tetris_Hard_Info *hard;
#if define HAS_CURSES
 Tetris_Ncurses_Display *display;
#endif
 Tetris_Data_List *tetris;
 Tetris_Data *data;
}TetrisWorld;

//初期化系
void initialize(TetrisWorld *thisData);
void setHardStatus(Tetris_Hard_Info *thisHard);
void setTetrisData(TetrisWorld *thisData);
void initData(TetrisWorld *thisData);
#if define HAS_CURSES
void setNcurses(TetrisWorld *thisData);
#endif

void loop(TetrisWorld *thisData);
void draw(TetrisWorld *thisData);
void set_block(int block_no,int block[4][4]);
void generate_block(TetrisWorld *thisData);
void move_block(TetrisWorld *thisData);
int conflict(TetrisWorld *thisData);
int gameover(TetrisWorld *thisData);
void signalhandler(int sig);
void set_map_block(TetrisWorld *thisData);
void deleteLine(TetrisWorld *thisData);
void rotateblock(TetrisWorld *thisData,int mode);
void drawNextBlock(TetrisWorld *thisData);

int main(void){
 TetrisWorld *this=(TetrisWorld *)malloc(sizeof(TetrisWorld));
 this->hard=(Tetris_Hard_Info *)malloc(sizeof(Tetris_Hard_Info));
 this->tetris=(Tetris_Data_List *)malloc(sizeof(Tetris_Data_List));
 this->data=(Tetris_Data *)malloc(sizeof(Tetris_Data));
#if define HAS_CURSES
 this->display=(Tetris_Ncurses_Display *)malloc(sizeof(Tetris_Ncurses_Display));
#endif
 initialize(this);
 while(!this->data->endFlag){
  this->data->flame++;
  loop(this);
  usleep((1000/60)*1000);
 }
 endwin();

 return 0;
}

void initialize(TetrisWorld *thisData){
 setHardStatus(thisData->hard);
 setNcurses(thisData);
 setTetrisData(thisData);
 initData(thisData);
}

void setHardStatus(Tetris_Hard_Info *thisHard){
 thisHard->LED_height=16;
 thisHard->LED_width=16;
 thisHard->LCD_height=2;
 thisHard->LCD_width=16;
}

#if define HAS_CURSES
void setNcurses(TetrisWorld *thisData){
 WINDOW *LCD,*rLCD,*LED;
 int i;

 if(signal(SIGINT,signalhandler)==SIG_ERR){
  fprintf(stderr,"SIGNALHANDLER ERROR\n");
  exit(EXIT_FAILURE);
 }

 initscr();
 cbreak();
 noecho();
 nodelay(stdscr,TRUE);
 keypad(stdscr,TRUE);
 start_color();
 use_default_colors();
 init_pair(1,COLOR_RED,-1);
 init_pair(2,COLOR_GREEN,-1);
 init_pair(3,COLOR_YELLOW,-1);

 LED=newwin(thisData->hard->LED_height,thisData->hard->LED_width,5,2);
 LCD=newwin(thisData->hard->LCD_height+2,thisData->hard->LCD_width+2,0,0);
 rLCD=subwin(LCD,thisData->hard->LCD_height,thisData->hard->LCD_width,1,1);
 if(LED==NULL||LCD==NULL||rLCD==NULL){
  endwin();
  fprintf(stderr,"LCD IS NULL\n");
  exit(EXIT_FAILURE);
 }

 //LED
 keypad(LED,TRUE);
 nodelay(LED,TRUE);

 box(LCD,'|','-');
 wrefresh(LCD);

 wattron(LED,COLOR_PAIR(3));
 for(i=0;i<(int)thisData->hard->LED_height;i++){
  mvwaddch(LED,i,10,'X');
 }
 wattroff(LED,COLOR_PAIR(3));
 wrefresh(LED);

 thisData->display->LED=LED;
 thisData->display->realLCD=rLCD;
 thisData->display->LCD=LCD;
}
#endif

void setTetrisData(TetrisWorld *thisData){
 thisData->tetris->block_down_time=60;
 thisData->tetris->map_y=16;
 thisData->tetris->map_x=10;
}

void initData(TetrisWorld *thisData){
 int i,j;
 thisData->data->next_block_no=rand()%7;
 thisData->data->generateFlag=1;
 thisData->data->endFlag=0;
 thisData->data->flame=0;

 thisData->data->deletedLine=0;
 thisData->data->score=0;

 for(i=0;i<16;i++){
  for(j=0;j<10;j++){
   thisData->data->map[i][j]=0;
  }
 }
}

void set_block(int block_no,int block[4][4]){
 static unsigned int bList[7][4][4]={
  {{0,0,0,0},
   {0,1,1,0},
   {0,1,1,0},
   {0,0,0,0}},
  {{0,1,0,0},
   {0,1,0,0},
   {0,1,0,0},
   {0,1,0,0}},
  {{0,1,1,0},
   {0,1,0,0},
   {0,1,0,0},
   {0,0,0,0}},
  {{0,1,1,0},
   {0,0,1,0},
   {0,0,1,0},
   {0,0,0,0}},
  {{0,0,1,0},
   {0,1,1,0},
   {0,1,0,0},
   {0,0,0,0}},
  {{0,1,0,0},
   {0,1,1,0},
   {0,0,1,0},
   {0,0,0,0}},
  {{0,0,0,0},
   {0,0,1,0},
   {0,1,1,1},
   {0,0,0,0}}
 };

 int i,j;
 
 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   block[i][j]=bList[block_no][i][j];
  }
 }
}

void loop(TetrisWorld *thisData){
 if(thisData->data->generateFlag){
  generate_block(thisData);
 }else{
  move_block(thisData);
 }
 draw(thisData);
}

void move_block(TetrisWorld *thisData){
 //入力
 int ch;
 int tmp;
 int move_x=0;
 int move_y;
 int down=0;
 int rotateFlag=0;
#if define HAS_CURSES
 ch=wgetch(thisData->display->LED);
 switch(ch){
  case KEY_LEFT:
  case 'h':
   move_x=-1;
   break;
  case KEY_RIGHT:
  case 'l':
   move_x=1;
   break;
  case KEY_DOWN:
  case 'j':
   down=1;
   break;
  case KEY_UP:
  case 'k':
  case ' ':
   rotateFlag=1;
  default:
   break;
 }
#endif

#if define AVR

#endif

#if define AVR
#endif

 if(rotateFlag!=0){
  rotateblock(thisData,rotateFlag);
  tmp=conflict(thisData);
  if(tmp){
   rotateblock(thisData,-rotateFlag);
  }
 }

 if(down||thisData->data->flame%thisData->tetris->block_down_time==0){
  move_y=1;
  thisData->data->flame=0;
 }else{
  move_y=0;
 }

 thisData->data->block_x+=move_x;
 tmp=conflict(thisData);
 if(tmp){
  thisData->data->block_x-=move_x;
  move_x=0;
 }
 if(move_y){
  thisData->data->block_y+=move_y;
  tmp=conflict(thisData);
  if(tmp){
   thisData->data->block_y-=1;
   if(!move_x){
    set_map_block(thisData);
    deleteLine(thisData);
    thisData->data->generateFlag=1;
   }
  }
 }
}

void signalhandler(int sig){
 endwin();
 fprintf(stderr,"SIG:%d\n",sig);
 exit(EXIT_FAILURE);
}

void generate_block(TetrisWorld *thisData){
 thisData->data->flame=0;
 thisData->data->generateFlag=0;
 thisData->data->block_x=3;
 thisData->data->block_y=0;
 set_block(thisData->data->next_block_no,thisData->data->using_block);
 if(conflict(thisData)){
  gameover(thisData);
 }

 thisData->data->next_block_no=rand()%7;
 drawNextBlock(thisData);
}

int conflict(TetrisWorld *thisData){
 int i,j;
 int tmp;
 int res=0;

 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   tmp=(
     (thisData->data->block_y+i<0)||
     (thisData->data->block_y+i>(int)thisData->tetris->map_y-1)||
     (thisData->data->block_x+j<0)||
     (thisData->data->block_x+j>(int)thisData->tetris->map_x-1)
     );
   if(!tmp){
    tmp|=(thisData->data->map[thisData->data->block_y+i][thisData->data->block_x+j]?2:0);
   }
   tmp=(tmp&&thisData->data->using_block[i][j]);
   res|=tmp;
  }
 }
 return res;
}

void draw(TetrisWorld *thisData){
 int i,j;
 WINDOW *LED;

 LED=thisData->display->LED;

 for(i=0;i<(int)thisData->tetris->map_y;i++){
  wmove(LED,i,0);
  for(j=0;j<(int)thisData->tetris->map_x;j++){
   switch(thisData->data->map[i][j]){
    case 0:
     waddch(LED,' ');
     break;
    case 1:
     wattron(LED,COLOR_PAIR(1));
     waddch(LED,'X');
     wattroff(LED,COLOR_PAIR(1));
     break;
    default:
     waddch(LED,'?');
     break;
   }
  }
 }

 wattron(LED,COLOR_PAIR(2));
 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   if(thisData->data->block_y+i<0||
      thisData->data->block_x+j<0||
      thisData->data->block_y+i>(int)thisData->tetris->map_y-1||
      thisData->data->block_x+j>(int)thisData->tetris->map_x-1){
    continue;
   }
   if(thisData->data->using_block[i][j]){
    mvwaddch(LED,
             thisData->data->block_y+i,
             thisData->data->block_x+j,
             'X');
   }
  }
 }
 wattroff(LED,COLOR_PAIR(2));
 wrefresh(LED);
}

void drawNextBlock(TetrisWorld *thisData){
 int dummy[4][4];
 int i,j;

 set_block(thisData->data->next_block_no,dummy);
 wattron(thisData->display->LED,COLOR_PAIR(1));
 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   if(dummy[i][j]){
    mvwaddch(thisData->display->LED,1+i,11+j,'X');
   }else{
    mvwaddch(thisData->display->LED,1+i,11+j,' ');
   }
  }
 }
 wattroff(thisData->display->LED,COLOR_PAIR(1));
}

void deleteLine(TetrisWorld *thisData){
#define MAP_Y thisData->tetris->map_y
#define MAP_X thisData->tetris->map_x
#define MAP thisData->data->map
 int i,j,k;
 int flag;
 //int dLine;

 for(i=0;i<(int)MAP_Y;i++){
  flag=MAP[MAP_Y-1-i][0];
  for(j=1;j<(int)MAP_X;j++){
   flag&=MAP[MAP_Y-1-i][j];
  }
  if(flag){
   for(k=i;k<(int)MAP_Y-1;k++){
    for(j=0;j<(int)MAP_X;j++){
     MAP[MAP_Y-1-k][j]=MAP[MAP_Y-1-k-1][j];
    }
   }
   for(j=0;j<(int)MAP_X-1;j++){
    MAP[0][j]=0;
   }
   i--;
  }
 }
#undef MAP_Y
#undef MAP_X
#undef MAP
}

void rotateblock(TetrisWorld *thisData,int mode){
#define BLOCK thisData->data->using_block
 int i,j;
 int tmp[4][4];

 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   if(mode<0){
    tmp[i][j]=BLOCK[j][3-i];
   }else if(mode>0){
    tmp[i][j]=BLOCK[3-j][i];
   }
  }
 }

 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   BLOCK[i][j]=tmp[i][j];
  }
 }
#undef BLOCK
}

//ブロック固定
void set_map_block(TetrisWorld *thisData){
#define BLOCK_X thisData->data->block_x
#define BLOCK_Y thisData->data->block_y
#define BLOCK thisData->data->using_block
#define MAP thisData->data->map
 int i,j;
 for(i=0;i<4;i++){
  for(j=0;j<4;j++){
   MAP[BLOCK_Y+i][BLOCK_X+j]|=BLOCK[i][j];
  }
 }
#undef BLOCK_X
#undef BLOCK_Y
#undef BLOCK
#undef MAP
}

int gameover(TetrisWorld *thisData){
 endwin();
 exit(EXIT_FAILURE);
}
