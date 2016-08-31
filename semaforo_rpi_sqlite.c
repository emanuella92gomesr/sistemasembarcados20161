//gcc -o semaforo_sql semaforo_sql.c -lsqlite3 -std=c99 -l wiringPi

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sqlite3.h>

#define PA 7 // fisico 7
#define PB 0 // fisico 11
#define PC 2 // fisico 13
#define PD 3 // fisico 15
#define PE 21 // fisico 29
#define PF 22 // fisico 31
#define PG 23 // fisico 33

#define PCom1 1 // fisico 12
#define PBot 4 // fisico 16
#define PCom2 5 // fisico 18

#define VD_C 6 // fisico 22
#define AM_C 26 // fisico 32
#define VR_C 27 // fisico 36
#define VD_P 24 // fisico 35
#define VR_P 25 // fisico 37

void set_valor(int v);

sqlite3 *db;
char *err_msg = 0;

int count = -1, botao = 0, wait = 0;
int i = 0, d = 0, u = 0;

PI_THREAD (Task_1)
{
  (void) piHiPri(10);

  while(1){
    if(wait == 1){
      delay(5000);
      wait = 0;
    }

    if (digitalRead(PBot) == 0)
      botao = 1;

    delay(1);
  }
  
}

PI_THREAD (Task_2)
{
  (void) piHiPri(10); 
  
  while(1) {
    if(botao == 0){
      digitalWrite(VD_C, HIGH);
      digitalWrite(AM_C, LOW);
      digitalWrite(VR_C, LOW);
      digitalWrite(VD_P, LOW);
      digitalWrite(VR_P, HIGH);
    }
    else{
      int con = sqlite3_open("semaforo.db", &db);
      char *sql = "insert into semaforo(data) VALUES (datetime('now'));";
      con = sqlite3_exec(db, sql, 0, 0, &err_msg);

      if (con != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        break;
      }

      sqlite3_close(db);

      digitalWrite(VD_C, LOW);
      digitalWrite(AM_C, HIGH);
      delay(3000);

      digitalWrite(AM_C, LOW);
      digitalWrite(VR_P, LOW);
      digitalWrite(VR_C, HIGH);
      digitalWrite(VD_P, HIGH);

      count = 30;
      delay(count * 500);
      botao = 0;
      wait = 1;
     }

     delay(1);
  }  
}

PI_THREAD (Task_3)
{
  (void) piHiPri(10);

  while(1){
    while(count > -1){
      for (i = 0; i < 50; i++) { 
        d = count / 10;
        set_valor(d);
        digitalWrite(PCom2, HIGH);
        digitalWrite(PCom1, LOW);
        delay(5);

        u = count % 10;
        set_valor(u);
        digitalWrite(PCom1, HIGH);
        digitalWrite(PCom2, LOW);
        delay(5);
      }
 
      count--;
    }

    if(count == -1){
      set_valor(0);
      digitalWrite(PCom1, LOW);
      digitalWrite(PCom2, LOW);
      delay(1);
    }
  }
  
}

void set_valor(int v){
  if(v == 0){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, HIGH);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, LOW);
  }
  else if(v == 1){
    digitalWrite(PA, LOW);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, LOW);
    digitalWrite(PE, LOW);
    digitalWrite(PF, LOW);
    digitalWrite(PG, LOW);
  }
  else if(v == 2){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, LOW);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, HIGH);
    digitalWrite(PF, LOW);
    digitalWrite(PG, HIGH);
  }
  else if(v == 3){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, LOW);
    digitalWrite(PF, LOW);
    digitalWrite(PG, HIGH);
  }
  else if(v == 4){
    digitalWrite(PA, LOW);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, LOW);
    digitalWrite(PE, LOW);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, HIGH);
  }
  else if(v == 5){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, LOW);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, LOW);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, HIGH);
  }
  else if(v == 6){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, LOW);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, HIGH);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, HIGH);
  }
  else if(v == 7){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, LOW);
    digitalWrite(PE, LOW);
    digitalWrite(PF, LOW);
    digitalWrite(PG, LOW);
  }
  else if(v == 8){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, HIGH);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, HIGH);
  }
  else if(v == 9){
    digitalWrite(PA, HIGH);
    digitalWrite(PB, HIGH);
    digitalWrite(PC, HIGH);
    digitalWrite(PD, HIGH);
    digitalWrite(PE, LOW);
    digitalWrite(PF, HIGH);
    digitalWrite(PG, HIGH);
  }
}

int main (void)
{
  int rc = sqlite3_open("semaforo.db", &db);
    
  if (rc != SQLITE_OK) {        
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return 1;
  }

  char *sql = "create table if not exists semaforo(id integer primary key autoincrement, data DATE);";                                  
  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  
  char *sql2 = "insert into semaforo(data) VALUES (datetime('now'));";
  rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
    
  if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "SQL error: %s\n", err_msg);

    sqlite3_free(err_msg);        
    sqlite3_close(db);

    return 1;
  } 

  sqlite3_close(db);

  wiringPiSetup();
  
  pinMode(PA, OUTPUT);
  pinMode(PB, OUTPUT);
  pinMode(PC, OUTPUT);
  pinMode(PD, OUTPUT);
  pinMode(PE, OUTPUT);
  pinMode(PF, OUTPUT);
  pinMode(PG, OUTPUT);

  pinMode(PCom1, OUTPUT);
  pinMode(PCom2, OUTPUT);

  pinMode(PBot, INPUT);

  pinMode(VD_C, OUTPUT);
  pinMode(AM_C, OUTPUT);
  pinMode(VR_C, OUTPUT);
  pinMode(VD_P, OUTPUT);
  pinMode(VR_P, OUTPUT);

  pullUpDnControl(PBot,PUD_UP);

  piThreadCreate(Task_1);
  piThreadCreate(Task_2);
  piThreadCreate(Task_3);
 
  while(1){
           }

 return 0 ;
}