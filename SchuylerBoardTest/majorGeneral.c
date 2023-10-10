/*
 * majorGeneral.c
 *
 * Created: 9/28/2023 6:08:39 PM
 *  Author: rjweb
 */ 

#include	<avr/io.h>
#include	<avr/eeprom.h>
#include	<avr/pgmspace.h>
//#include 	<util/delay.h>
#include	<inttypes.h>
#include	<stdlib.h>
#include	<string.h>
#include	"majorGeneral.h"

const char majorGeneral[MAJORGENERAL_NUM_LINES][MAJORGENERAL_LINE_MAX] PROGMEM =  { //Array of strings in PROGMEM (Flash)
	"I am the very model of a modern Major-General\n",
	"I've information vegetable, animal, and mineral\n",
	"I know the kings of England, and I quote the fights\n",
	"Historical\n",
	"From Marathon to Waterloo, in order categorical\n",
	"I'm very well acquainted, too, with matters\n",
	"Mathematical\n",
	"I understand equations, both the simple and quadratical\n",
	"About binomial theorem I'm teeming with a lot o' news\n",
	"With many cheerful facts about the square of the\n",
	"Hypotenuse\n",
	"With many cheerful facts about the square of the\n",
	"Hypotenuse\n",
	"With many cheerful facts about the square of the\n",
	"Hypotenuse\n",
	"With many cheerful facts about the square of the\n",
	"Hypotepotenuse\n",
	"I'm very good at integral and differential calculus\n",
	"I know the scientific names of beings animalculous\n",
	"In short, in matters vegetable, animal, and mineral\n",
	"I am the very model of a modern Major-General\n",
	"In short, in matters vegetable, animal, and mineral\n",
	"He is the very model of a modern Major-General\n",
	"I know our mythic history, King Arthur's and Sir\n",
	"Caradoc's\n",
	"I answer hard acrostics, I've a pretty taste for\n",
	"Paradox\n",
	"I quote in elegiacs all the crimes of Heliogabalus\n",
	"In conics I can floor peculiarities parabolous\n",
	"I can tell undoubted Raphaels from Gerard Dows and\n",
	"Zoffanies\n",
	"I know the croaking chorus from the Frogs of\n",
	"Aristophanes\n",
	"Then I can hum a fugue of which I've heard the music's\n",
	"Din afore\n",
	"And whistle all the airs from that infernal nonsense\n",
	"Pinafore\n",
	"And whistle all the airs from that infernal nonsense\n",
	"Pinafore\n",
	"And whistle all the airs from that infernal nonsense\n",
	"Pinafore\n",
	"And whistle all the airs from that infernal nonsense\n",
	"Pinapinafore\n",
	"Then I can write a washing bill in Babylonic cuneiform\n",
	"And tell you ev'ry detail of Caractacus's uniform\n",
	"In short, in matters vegetable, animal, and mineral\n",
	"I am the very model of a modern Major-General\n",
	"In short, in matters vegetable, animal, and mineral\n",
	"He is the very model of a modern Major-General\n",
	"In fact, when I know what is meant by 'mamelon' and\n",
	"'Ravelin'\n",
	"When I can tell at sight a Mauser rifle from a javelin\n",
	"When such affairs as sorties and surprises I'm more\n",
	"Wary at\n",
	"And when I know precisely what is meant by\n",
	"'Commissariat'\n",
	"When I have learnt what progress has been made in\n",
	"Modern gunnery\n",
	"When I know more of tactics than a novice in a nunnery\n",
	"In short, when I've a smattering of elemental strategy\n",
	"You'll say a better Major-General had never sat a gee\n",
	"You'll say a better Major-General had never sat a gee\n",
	"You'll say a better Major-General had never sat a gee\n",
	"You'll say a better Major-General had never sat a sat a\n",
	"Gee\n",
	"For my military knowledge, though I'm plucky and\n",
	"Adventury\n",
	"Has only been brought down to the beginning of the\n",
	"Century\n",
	"But still, in matters vegetable, animal, and mineral\n",
	"I am the very model of a modern Major-General\n",
	"But still, in matters vegetable, animal, and mineral\n",
	"He is the very model of a modern Major-General\n",
    };
	
	uint8_t majorGeneral_max_line(void)
	{
		return MAJORGENERAL_LINE_MAX;
	}
	
	uint8_t majorGeneral_num_lines(void)
	{
		return MAJORGENERAL_NUM_LINES;
	}
	
	void majorGeneral_get_line(uint8_t linenum, char* buffer)
	{
		strcpy_P(buffer, majorGeneral[linenum]);
	}
	
	uint16_t majorGeneral_get_line_len(uint8_t linenum)
	{
		
		return strlen_P(majorGeneral[linenum]);
	}	