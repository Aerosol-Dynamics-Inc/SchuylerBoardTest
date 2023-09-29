/*
 * majorGeneral.h
 *
 * Created: 9/28/2023 6:37:32 PM
 *  Author: rjweb
 */ 


#ifndef MAJORGENERAL_H_
#define MAJORGENERAL_H_

#define MAJORGENERAL_NUM_LINES 73
#define MAJORGENERAL_LINE_MAX 62

uint8_t majorGeneral_max_line(void);
uint8_t majorGeneral_num_lines(void);
void majorGeneral_get_line(uint8_t linenum, char* buffer);



#endif /* MAJORGENERAL_H_ */