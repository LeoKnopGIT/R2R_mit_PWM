// INIT -> Alle Initfunktionen werden hier deklariert
#ifndef INIT_H
#define INIT_H

void initMC(void);
void initPWM(int pwm_anzahl, int pwm_on);
void initR2R(void);
void initSPI(void);
void initUART(void);
void initTIMER(void);
#endif