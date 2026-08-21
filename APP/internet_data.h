#ifndef INTERNET_DATA_H_
#define INTERNET_DATA_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp8266.h"


typedef struct {
    float deg;
    float max;
    float min;
    char dur[40];
} HavaGunu;

typedef struct {
    float dolar;
    char dolar_degisim[8]; // Örn: "+0.00" veya "-0.15" karakter dizisi hoca
    
    float euro;
    char euro_degisim[8];  // Örn: "+0.68" karakter dizisi
    
    float altin;
    char altin_degisim[8]; // Örn: "-0.78" karakter dizisi
    
    float gumus;
    char gumus_degisim[8]; // Örn: "-4.30" karakter dizisi
    
    HavaGunu hava[3];      // hava[0] = Bugün, hava[1] 
} PiyasaVerileri;

extern PiyasaVerileri veriler;



bool InternetData_Update(void);

#endif /* INTERNET_DATA_H_ */