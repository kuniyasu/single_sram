/*
 * single_sram.h
 *
 *  Created on: 2014/01/04
 *      Author: Kuniyasu
 */

#ifndef SINGLE_SRAM_H_
#define SINGLE_SRAM_H_

#include <systemc.h>

class PIN{};
class TLM2LT{};
class TLM2AT{};

#define TR_NAME(_name) (std::string(name())+"_"+std::string(_name)).c_str()
#define PIN_NAME(bname,_name_) (std::string(bname)+"_"+std::string(_name_)).c_str()

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>class sram_if;
template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>class sram_signal;

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>class sram_base_core;
template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>class sram_core;
template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE>class sram_wrapper;

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE>class sram_base_port;
template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE>class sram_base_export;
template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE>class sram_port;
//template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE>class sram_export;

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>
class sram_if:public sc_interface{
public:
	static const bool ACTIVE_CS =  false;
	static const bool IDLE_CS   =  true;

	typedef sc_uint<ADWIDTH> addr_type;
	typedef sc_uint<DTWIDTH> data_type;

	virtual void sram_write(const addr_type& addr, const data_type& wdata) = 0;
	virtual data_type sram_read(const addr_type& addr) = 0;

};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>
class sram_signal{
public:
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sc_signal<bool> ncs;
	sc_signal<bool> nwe;
	sc_signal<addr_type> addr;
	sc_signal<data_type> wdata;
	sc_signal<data_type> rdata;

	sram_signal(const char* name=sc_gen_unique_name("sram_signal")):
	ncs(PIN_NAME(name,"ncs")),
	nwe(PIN_NAME(name,"nwe")),
	addr(PIN_NAME(name,"addr")),
	wdata(PIN_NAME(name,"wdata")),
	rdata(PIN_NAME(name,"rdata")){}

};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE=PIN>
class sram_base_port{
public:
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sc_out<bool> ncs;
	sc_out<bool> nwe;
	sc_out<addr_type> addr;
	sc_out<data_type> wdata;
	sc_in<data_type> rdata;

	sram_base_port(const char* name=sc_gen_unique_name("sram_base_port")):
	ncs(PIN_NAME(name,"ncs")),
	nwe(PIN_NAME(name,"nwe")),
	addr(PIN_NAME(name,"addr")),
	wdata(PIN_NAME(name,"wdata")),
	rdata(PIN_NAME(name,"rdata")){}

	template<class P> void bind(P& p){
		ncs(p.ncs);
		nwe(p.nwe);
		addr(p.addr);
		wdata(p.wdata);
		rdata(p.rdata);
	}

	template<class P> void operator()(P& p){
		bind(p);
	}
};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE=PIN>
class sram_base_export{
public:

	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sc_in<bool> ncs;
	sc_in<bool> nwe;
	sc_in<addr_type> addr;
	sc_in<data_type> wdata;
	sc_out<data_type> rdata;

	sram_base_export(const char* name=sc_gen_unique_name("sram_base_export")):
	ncs(PIN_NAME(name,"ncs")),
	nwe(PIN_NAME(name,"nwe")),
	addr(PIN_NAME(name,"addr")),
	wdata(PIN_NAME(name,"wdata")),
	rdata(PIN_NAME(name,"rdata")){}

	template<class P> void bind(P& p){
		ncs(p.ncs);
		nwe(p.nwe);
		addr(p.addr);
		wdata(p.wdata);
		rdata(p.rdata);
	}

	template<class P> void operator()(P& p){
		bind(p);
	}
};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE=PIN>
class sram_port:public sc_module, public sram_base_port<ADWIDTH,DTWIDTH,SIZE,MODE>, public sram_if<ADWIDTH,DTWIDTH,SIZE>{
public:
	typedef sram_base_port<ADWIDTH,DTWIDTH,SIZE,MODE> base_type;

	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sc_in<bool> clk;
	sc_in<bool> nrst;

	SC_HAS_PROCESS(sram_port);
	sram_port(const sc_module_name name=sc_gen_unique_name("sram_base_port")):sc_module(name), base_type(name),
	clk(PIN_NAME(name,"clk")),
	nrst(PIN_NAME(name,"nrst")){
		end_module();
	}

	virtual void reset(){
		base_type::ncs.write(true);
	}

	virtual void sram_write(const addr_type& addr, const data_type& wdata){
		base_type::ncs.write(true);
		base_type::addr.write(addr);
		base_type::nwe.write(false);
		wait();
		base_type::ncs.write(false);
	}

	virtual data_type sram_read(const addr_type& addr){
		base_type::ncs.write(true);
		base_type::addr.write(addr);
		base_type::nwe.write(false);
		wait();
		base_type::ncs.write(false);
		return base_type::rdata.read();
	}

	template<class P> void bind(P& p){
		base_type::ncs(p.ncs);
		base_type::nwe(p.nwe);
		base_type::addr(p.addr);
		base_type::wdata(p.wdata);
		base_type::rdata(p.rdata);
	}

	template<class P> void operator()(P& p){
		bind(p);
	}
};


template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>
class sram_base_core:public sram_if<ADWIDTH,DTWIDTH,SIZE>{
public:
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef sram_base_export<ADWIDTH,DTWIDTH,SIZE,PIN> base_type;

	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	data_type memcell[SIZE];

	sram_base_core(const char* name=sc_gen_unique_name("sram_base_core")){}

	virtual void sram_write(const addr_type& addr, const data_type& wdata) {
		memcell[addr] = wdata;
	}

	virtual data_type sram_read(const addr_type& addr) {
		return memcell[addr];
	}

	data_type& operator[](const addr_type& addr){
		return memcell[addr];
	}
};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>class sram_core:public sc_module, public sram_base_core<ADWIDTH,DTWIDTH,SIZE>, public sram_base_export<ADWIDTH,DTWIDTH,SIZE,PIN>{
public:
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef sram_base_core<ADWIDTH,DTWIDTH,SIZE> base_type;
	typedef sram_base_export<ADWIDTH,DTWIDTH,SIZE,PIN> base_port_type;

	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sc_in<bool> clk;

	data_type memcell[SIZE];

	SC_HAS_PROCESS(sram_core);
	sram_core(const sc_module_name name=sc_gen_unique_name("sram_core")):sc_module(name),base_type(name),base_port_type(name){
		SC_CTHREAD(main_thread, clk.pos());

		end_module();
	}

	void main_thread(){
		while( true ){
			do{ wait(); }while( base_port_type::ncs.read() == false);

			if( base_port_type::nwe.read() == false ){
				base_type::sram_write(base_port_type::addr.read(),base_port_type::wdata.read());
			}else{
				base_port_type::rdata.write( base_type::sram_read(base_port_type::addr.read()) );
			}
		}
	}


	template<class P> void bind(P& p){
		base_port_type::ncs(p.ncs);
		base_port_type::nwe(p.nwe);
		base_port_type::addr(p.addr);
		base_port_type::wdata(p.wdata);
		base_port_type::rdata(p.rdata);
	}

	template<class P> void operator()(P& p){
		bind(p);
	}
};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE, class MODE=PIN>class sram_wrapper{
public:
	sc_in<bool> clk;

	sram_signal<ADWIDTH,DTWIDTH,SIZE> ex_port;
	sram_core<ADWIDTH,DTWIDTH,SIZE> macro;

	sram_wrapper(const char* name=sc_gen_unique_name("sram_wrapper")):
	clk(PIN_NAME(name,"clk")),
	ex_port(PIN_NAME(name,"ex_port")),
	macro(PIN_NAME(name,"macro")){
		macro.clk(clk);
		macro(ex_port);
	}
};


template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>
class sram_base_export<ADWIDTH,DTWIDTH,SIZE,TLM2LT>:public sc_export<sram_if<ADWIDTH,DTWIDTH,SIZE> >{
public:

	typedef sc_export<sram_if<ADWIDTH,DTWIDTH,SIZE> > base_type;
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;

	sram_base_export(const char* name=sc_gen_unique_name("sram_base_export")):base_type(name){}

};

template<unsigned int ADWIDTH, unsigned int DTWIDTH, unsigned int SIZE>
class sram_wrapper<ADWIDTH,DTWIDTH,SIZE,TLM2LT>:public sram_base_core<ADWIDTH,DTWIDTH,SIZE>{
public:
	typedef sram_if<ADWIDTH,DTWIDTH,SIZE> if_type;
	typedef typename if_type::addr_type addr_type;
	typedef typename if_type::data_type data_type;
	typedef sram_base_export<ADWIDTH,DTWIDTH,SIZE,TLM2LT> base_type;

	sc_in<bool> clk;

	sram_base_export<ADWIDTH,DTWIDTH,SIZE,TLM2LT> ex_port;

	sram_wrapper(const char* name=sc_gen_unique_name("sram_wrapper")):clk(PIN_NAME(name,"clk")), base_type(name){
		ex_port(*this);
	}
};


// TEST wrapper
template<class MODE>class sram32{
public:
	typedef sram_base_port<32U,32U,1024,MODE> base_port;
	typedef sram_port<32U,32U,1024,MODE> port;
	typedef sram_base_export<32U,32U,1024,MODE> base_export;

	typedef sram_wrapper<32U,32U,1024,MODE> wrapper;
};

#endif /* SINGLE_SRAM_H_ */
