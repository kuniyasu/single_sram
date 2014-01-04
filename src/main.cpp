/*
 * main.cpp
 *
 *  Created on: 2014/01/04
 *      Author: Kuniyasu
 */

#include "TOP.h"

TOP* top;
sc_trace_file* tf;

int sc_main(int argc, char* argv[]){
	tf = sc_create_vcd_trace_file("systemc");
	top = new TOP("top");
	top->set_trace(tf);

	sc_start();
	return 0;
}


