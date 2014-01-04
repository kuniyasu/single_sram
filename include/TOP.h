/*
 * TOP.h
 *
 *  Created on: 2014/01/04
 *      Author: Kuniyasu
 */

#ifndef TOP_H_
#define TOP_H_

#include <systemc.h>
#include "single_sram.h"

#define ioMode PIN

class SRAM_DUT:public sc_module{
public:
	sc_in<bool> clk;
	sc_in<bool> nrst;

	sram32<ioMode>::port port;

	SC_HAS_PROCESS(SRAM_DUT);
	SRAM_DUT(const sc_module_name name=sc_gen_unique_name("SRAM_DUT")):sc_module(name){
		port.clk(clk);
		port.nrst(nrst);

		SC_CTHREAD(main_thread,clk.pos());
		async_reset_signal_is(nrst,false);

		end_module();
	}

	void main_thread(){
		port.reset();
		wait();

		wait();
		wait();
		wait();
		wait();
		wait();

		sc_stop();
	}
};

class TOP:public sc_module{
public:
	sc_clock clk;
	sc_signal<bool> nrst;

	SRAM_DUT sram_dut;
	sram32<ioMode>::wrapper memory;

	SC_HAS_PROCESS(TOP);

	TOP(const sc_module_name name):sc_module(name),
	clk("CLK", 25, SC_NS, 0.5, 5, SC_NS, true ),nrst("nrst"),
	sram_dut("sram_dut"),memory("memory")
	{
		sram_dut.clk(clk);
		sram_dut.nrst(nrst);
		sram_dut.port(memory.ex_port);
		memory.clk(clk);

		SC_THREAD(timeout_thread);
		end_module();
	}

	void timeout_thread(){
		nrst = false;
		wait(100,SC_NS);

		nrst = true;
		wait(10,SC_MS);

		sc_stop();
	}
};


#endif /* TOP_H_ */
