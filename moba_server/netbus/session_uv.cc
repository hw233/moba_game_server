#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

#include "uv.h"
#include "session.h"
#include "session_uv.h"

#include "../utils/cache_alloc.h"

#define SESSION_CACHE_CAPACITY 6000
#define WQ_CACHE_CAPACITY 4096
struct cache_allocer* session_allocer = NULL;
static cache_allocer* wr_allocer = NULL;
void init_session_allocer()
{
	if (session_allocer == NULL)
	{
		session_allocer = create_cache_allocer(SESSION_CACHE_CAPACITY, sizeof(uv_session));
	}

	if (wr_allocer == NULL)
	{
		wr_allocer = create_cache_allocer(WQ_CACHE_CAPACITY, sizeof(uv_write_t));
	}
}

extern "C" {
	static void after_write(uv_write_t* req, int status)
	{
		if (status == 0)
		{
			printf("write success\n");
		}
		//free(req);
		cache_free(wr_allocer, req);
	}

	static void on_close(uv_handle_t* handle)
	{
		printf("close client\n");
		uv_session* s = (uv_session*)handle->data;
		uv_session::destroy(s);
	}

	static void on_shutdown(uv_shutdown_t* req, int status)
	{
		uv_close((uv_handle_t*)req->handle, on_close);
	}
}

uv_session* uv_session::create()
{
	//uv_session* uv_s = new uv_session();
	uv_session* uv_s = (uv_session*)cache_alloc(session_allocer, sizeof(uv_session));
	uv_s->uv_session::uv_session();
	uv_s->init();
	return uv_s;
}

void uv_session::destroy(uv_session* s)
{
	s->exit();
	//delete s;
	s->uv_session::~uv_session();
	cache_free(session_allocer, s);
}

void uv_session::init()
{
	memset(this->c_address, 0, sizeof(this->c_address));
	this->c_port = 0;
	this->recved = 0;
	this->is_shutdown = false;
}

void uv_session::exit()
{

}

void uv_session::close()
{
	if (this->is_shutdown)
	{
		return;
	}

	this->is_shutdown = true;
	uv_shutdown_t* reg = &this->shutdown;
	memset(reg, 0, sizeof(uv_shutdown_t));
	uv_shutdown(reg,(uv_stream_t*)&this->tcp_handle, on_shutdown);
}

void uv_session::send_data(unsigned char* body, int len)
{
	//uv_write_t* w_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_write_t* w_req = (uv_write_t*)cache_alloc(wr_allocer, sizeof(uv_write_t));
	uv_buf_t w_buf;

	w_buf = uv_buf_init((char*)body, len);
	uv_write(w_req, (uv_stream_t*)&this->tcp_handle, &w_buf, 1, after_write);
}

const char* uv_session::get_address(int* port)
{
	*port = this->c_port;
	return this->c_address;
}