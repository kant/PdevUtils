/*============================================================================
 Name        : env_scpiparser.cpp
 Author      : Wim
 Version     : V1.0
 Copyright   :
 Description : SCPI parser  for the environment sensor server
Copyright (c) 2020 Wim Beaumont
Copyright (c) 2013 Lachlan Gunn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.




*/
//============================================================================
#include <stdio.h>
#include "scpiparser.h"
#include "env_scpiparser.h"
#include <string.h>

struct scpi_parser_context ctx;
//extern int read_temperature(float *volt, int ch=0 );

int read_temperature(float *volt, int ch=0 ){

	switch (ch){
	case  0: *volt=0.1 ; break;
	case  1: *volt=1.1 ; break;
	case  2: *volt=0.1 ; break;
	default : *volt=0.0 ; break;
	}
	return 0;
}


scpi_error_t identify(struct scpi_parser_context* context,struct scpi_token* command);
scpi_error_t get_temperature(struct scpi_parser_context* context,struct scpi_token* command);
scpi_error_t get_temperature_ch(struct scpi_parser_context* context,struct scpi_token* command);
scpi_error_t get_luminosity(struct scpi_parser_context* context,struct scpi_token* command);
scpi_error_t get_humidity(struct scpi_parser_context* context,struct scpi_token* command);

void scpi_setup() {

	struct scpi_command* measure;
	struct scpi_command* meas_temp;

	/* First, initialise the parser. */
	scpi_init(&ctx);

	/*
	 * After initialising the parser, we set up the command tree.  Ours is
	 *
	 *  *IDN?         -> identify
	 *
	 *  :MEASure
	 *    :HUMIdity?   -> get humidity in % , float
	 *    :LUMInosity? -> get luminosity in LUX
	 *    :TEMPerature?  -> get temperature of ch 0
	 *    :TEMPerature?: CHX  -> get temperature of ch min (x,2)
	 */
	scpi_register_command(ctx.command_tree, SCPI_CL_SAMELEVEL, "*IDN?", 5,"*IDN?", 5, identify);
	measure = scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "MEASURE", 7, "MEAS", 4, NULL);
	meas_temp=scpi_register_command(measure, SCPI_CL_CHILD, "TEMPERATURE?", 12, "TEMP?", 5,get_temperature);
	scpi_register_command(meas_temp, SCPI_CL_CHILD, "CHANNEL", 6, "CH", 2,get_temperature_ch);
	scpi_register_command(measure, SCPI_CL_CHILD, "HUMIDITY?", 9, "HUMI?", 5,get_humidity);
	scpi_register_command(measure, SCPI_CL_CHILD, "LUMINOSITY?", 11, "LUMI?", 5,get_luminosity);



	//Serial.begin(9600);
}

/*
 * Respond to *IDN?
 */
scpi_error_t identify(struct scpi_parser_context* context,struct scpi_token* command) {
	scpi_free_tokens(command);

	add2result("EnvServ V1,0");
	return SCPI_SUCCESS;
}


/**
 * Read the temperature of ch 0
 */
scpi_error_t get_temperature(struct scpi_parser_context* context,struct scpi_token* command) {
	float temperature;

	read_temperature(&temperature);

	add2resultf(temperature);
	scpi_free_tokens(command);
	return SCPI_SUCCESS;
}



/**
 * Read the humidity
 */
scpi_error_t get_humidity(struct scpi_parser_context* context,struct scpi_token* command) {
	float humidity=23.4;


	add2resultf(humidity);
	scpi_free_tokens(command);
	return SCPI_SUCCESS;
}

/**
 * Read the luminosity
 */
scpi_error_t get_luminosity(struct scpi_parser_context* context,struct scpi_token* command) {
	float luminosity= 2323;


	add2resultf(luminosity);
	scpi_free_tokens(command);
	return SCPI_SUCCESS;
}


/**
 * Read the temperature of ch X
 */

scpi_error_t get_temperature_ch(struct scpi_parser_context* context,struct scpi_token* command) {
	struct scpi_token* args;
	struct scpi_numeric output_numeric;
	unsigned char chan;// hc

	args = command;

	while (args != NULL && args->type == 0) {
		args = args->next;
	}

	output_numeric = scpi_parse_numeric(args->value, args->length, 0, 0, 2);
	chan=(unsigned char)output_numeric.value;
	if( chan > 2 ) chan=2; // unsigned so can not  < 0

	 float temperature=0;
	read_temperature(&temperature,chan);

	add2resultf(temperature);

	scpi_free_tokens(command);

	return SCPI_SUCCESS;
}

void env_scpi_execute_command(const char* l_buf,int length){ scpi_execute_command(&ctx, l_buf,length);}
char* env_get_result() { return get_result();}

