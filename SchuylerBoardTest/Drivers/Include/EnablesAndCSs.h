/*
 * Enables.h
 *
 * Created: 6/29/2023 1:13:07 PM
 *  Author: greg
 */ 


#ifndef ENABLES_H_
#define ENABLES_H_

void enableBuck014(void); // nomnally Cond, Init, Spare 1
void disableBuck014(void);

void enableBuck2(void);
void disableBuck2(void);

void enableBuck3(void);
void disableBuck3(void);

void enableBuck5(void);
void disableBuck5(void);

void toggleHeartBeat();
void heartBeatOn();
void heartBeatOff();

void powerUpWickSensor(void);
void powerDownWickSensor(void);

void selectFRAM();
void deSelectFRAM();

void selectOpticsDAC();
void deSelectOpticsDAC();

void selectADC2518();
void deSelectADC2518();

void selectBuckCtrlDAC();
void deSelectBuckCtrlDAC();

void selectFlash();
void deSelectFlash();

void selectSDcard();
void deSelectSDcard();

void selectRTC();
void deSelectRTC();

#endif /* ENABLES_H_ */
