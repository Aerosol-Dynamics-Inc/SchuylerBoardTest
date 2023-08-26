/*
 * NPA700.h
 *
 * Created: 8/4/2023 6:27:58 PM
 *  Author: greg
 */ 


#ifndef NPA700_H_
#define NPA700_H_

void initPressureSensors(void);
void readABSPress(void);
void readDiffPress(void);

void readAbs_CH(void);
void readDiff_CH(void);
void readU47(void);



#endif /* NPA700_H_ */