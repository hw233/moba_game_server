#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lua_wrapper.h"
#include "../netbus/service.h"
#include "../netbus/session.h"
#include "../netbus/proto_man.h"
#include "../netbus/service_man.h"
#include "../utils/logger.h"

#include "google/protobuf/message.h"
using namespace google::protobuf;

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

#include "service_export_to_lua.h"

#define SERVICE_FUNCTION_MAPPING "service_function_mapping"

static
void init_service_function_map(lua_State* L) {
	lua_pushstring(L, SERVICE_FUNCTION_MAPPING);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);
}

static unsigned int s_function_ref_id = 0;
static unsigned int
save_service_function(lua_State* L, int lo, int def)
{
	// function at lo
	if (!lua_isfunction(L, lo)) return 0;

	s_function_ref_id++;

	lua_pushstring(L, SERVICE_FUNCTION_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: fun ... refid_fun */
	lua_pushinteger(L, s_function_ref_id);                      /* stack: fun ... refid_fun refid */
	lua_pushvalue(L, lo);                                       /* stack: fun ... refid_fun refid fun */

	lua_rawset(L, -3);                  /* refid_fun[refid] = fun, stack: fun ... refid_ptr */
	lua_pop(L, 1);                                              /* stack: fun ... */

	return s_function_ref_id;

	// lua_pushvalue(L, lo);                                           /* stack: ... func */
	// return luaL_ref(L, LUA_REGISTRYINDEX);
}

static void
get_service_function(lua_State* L, int refid)
{
	lua_pushstring(L, SERVICE_FUNCTION_MAPPING);
	lua_rawget(L, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(L, refid);                                  /* stack: ... refid_fun refid */
	lua_rawget(L, -2);                                          /* stack: ... refid_fun fun */
	lua_remove(L, -2);                                          /* stack: ... fun */
}

static bool
push_service_function(int nHandler)
{
	get_service_function(lua_wrapper::lua_state(), nHandler);                  /* L: ... func */
	if (!lua_isfunction(lua_wrapper::lua_state(), -1))
	{
		log_error("[LUA ERROR] function refid '%d' does not reference a Lua function", nHandler);
		lua_pop(lua_wrapper::lua_state(), 1);
		return false;
	}
	return true;
}

static int
exe_function(int numArgs)
{
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(lua_wrapper::lua_state(), functionIndex))
	{
		log_error("value at stack [%d] is not function", functionIndex);
		lua_pop(lua_wrapper::lua_state(), numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(lua_wrapper::lua_state(), "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
	if (!lua_isfunction(lua_wrapper::lua_state(), -1))
	{
		lua_pop(lua_wrapper::lua_state(), 1);                                            /* L: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(lua_wrapper::lua_state(), functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = 0;
	error = lua_pcall(lua_wrapper::lua_state(), numArgs, 1, traceback);                  /* L: ... [G] ret */
	if (error)
	{
		if (traceback == 0)
		{
			log_error("[LUA ERROR] %s", lua_tostring(lua_wrapper::lua_state(), -1));        /* L: ... error */
			lua_pop(lua_wrapper::lua_state(), 1); // remove error message from stack
		}
		else                                                            /* L: ... G error */
		{
			lua_pop(lua_wrapper::lua_state(), 2); // remove __G__TRACKBACK__ and error message from stack
		}
		return 0;
	}

	// get return value
	int ret = 0;
	if (lua_isnumber(lua_wrapper::lua_state(), -1))
	{
		ret = (int)lua_tointeger(lua_wrapper::lua_state(), -1);
	}
	else if (lua_isboolean(lua_wrapper::lua_state(), -1))
	{
		ret = (int)lua_toboolean(lua_wrapper::lua_state(), -1);
	}
	// remove return value from stack
	lua_pop(lua_wrapper::lua_state(), 1);                                                /* L: ... [G] */

	if (traceback)
	{
		lua_pop(lua_wrapper::lua_state(), 1); // remove __G__TRACKBACK__ from stack      /* L: ... */
	}

	return ret;
}

static int
execute_service_function(int nHandler, int numArgs) {
	int ret = 0;
	if (push_service_function(nHandler))                                /* L: ... arg1 arg2 ... func */
	{
		if (numArgs > 0)
		{
			lua_insert(lua_wrapper::lua_state(), -(numArgs + 1));                        /* L: ... func arg1 arg2 ... */
		}
		ret = exe_function(numArgs);
	}
	lua_settop(lua_wrapper::lua_state(), 0);
	return ret;
}

class lua_service : public service {
public:
	unsigned int lua_recv_cmd_handler;
	unsigned int lua_disconnect_handler;
	unsigned int lua_connect_handler;
	unsigned int lua_recv_raw_handler;

public:
	virtual bool on_session_recv_raw_cmd(session* s, struct raw_cmd* msg);
	virtual bool on_session_recv_cmd(session* s, struct cmd_msg* msg);
	virtual void on_session_disconnect(session* s, int stype);
	virtual void on_session_connect(session* s, int stype);
};


void
push_proto_message_tolua(const Message* message) {
	lua_State* state = lua_wrapper::lua_state();
	if (!message) {
		// printf("PushProtobuf2LuaTable failed, message is NULL");
		return;
	}
	const Reflection* reflection = message->GetReflection();

	// 顶层table
	lua_newtable(state);

	const Descriptor* descriptor = message->GetDescriptor();
	for (int32_t index = 0; index < descriptor->field_count(); ++index) {
		const FieldDescriptor* fd = descriptor->field(index);
		const std::string& name = fd->lowercase_name();

		// key
		lua_pushstring(state, name.c_str());

		bool bReapeted = fd->is_repeated();

		if (bReapeted) {
			// repeated这层的table
			lua_newtable(state);
			int size = reflection->FieldSize(*message, fd);
			for (int i = 0; i < size; ++i) {
				char str[32] = { 0 };
				switch (fd->cpp_type()) {
				case FieldDescriptor::CPPTYPE_DOUBLE:
					lua_pushnumber(state, reflection->GetRepeatedDouble(*message, fd, i));
					break;
				case FieldDescriptor::CPPTYPE_FLOAT:
					lua_pushnumber(state, (double)reflection->GetRepeatedFloat(*message, fd, i));
					break;
				case FieldDescriptor::CPPTYPE_INT64:
					sprintf(str, "%lld", (long long)reflection->GetRepeatedInt64(*message, fd, i));
					lua_pushstring(state, str);
					break;
				case FieldDescriptor::CPPTYPE_UINT64:

					sprintf(str, "%llu", (unsigned long long)reflection->GetRepeatedUInt64(*message, fd, i));
					lua_pushstring(state, str);
					break;
				case FieldDescriptor::CPPTYPE_ENUM: // 与int32一样处理
					lua_pushinteger(state, reflection->GetRepeatedEnum(*message, fd, i)->number());
					break;
				case FieldDescriptor::CPPTYPE_INT32:
					lua_pushinteger(state, reflection->GetRepeatedInt32(*message, fd, i));
					break;
				case FieldDescriptor::CPPTYPE_UINT32:
					lua_pushinteger(state, reflection->GetRepeatedUInt32(*message, fd, i));
					break;
				case FieldDescriptor::CPPTYPE_STRING:
				{
					std::string value = reflection->GetRepeatedString(*message, fd, i);
					lua_pushlstring(state, value.c_str(), value.size());
				}
				break;
				case FieldDescriptor::CPPTYPE_BOOL:
					lua_pushboolean(state, reflection->GetRepeatedBool(*message, fd, i));
					break;
				case FieldDescriptor::CPPTYPE_MESSAGE:
					push_proto_message_tolua(&(reflection->GetRepeatedMessage(*message, fd, i)));
					break;
				default:
					break;
				}

				lua_rawseti(state, -2, i + 1); // lua's index start at 1
			}

		}
		else {
			char str[32] = { 0 };
			switch (fd->cpp_type()) {

			case FieldDescriptor::CPPTYPE_DOUBLE:
				lua_pushnumber(state, reflection->GetDouble(*message, fd));
				break;
			case FieldDescriptor::CPPTYPE_FLOAT:
				lua_pushnumber(state, (double)reflection->GetFloat(*message, fd));
				break;
			case FieldDescriptor::CPPTYPE_INT64:

				sprintf(str, "%lld", (long long)reflection->GetInt64(*message, fd));
				lua_pushstring(state, str);
				break;
			case FieldDescriptor::CPPTYPE_UINT64:

				sprintf(str, "%llu", (unsigned long long)reflection->GetUInt64(*message, fd));
				lua_pushstring(state, str);
				break;
			case FieldDescriptor::CPPTYPE_ENUM: // 与int32一样处理
				lua_pushinteger(state, (int)reflection->GetEnum(*message, fd)->number());
				break;
			case FieldDescriptor::CPPTYPE_INT32:
				lua_pushinteger(state, reflection->GetInt32(*message, fd));
				break;
			case FieldDescriptor::CPPTYPE_UINT32:
				lua_pushinteger(state, reflection->GetUInt32(*message, fd));
				break;
			case FieldDescriptor::CPPTYPE_STRING:
			{
				std::string value = reflection->GetString(*message, fd);
				lua_pushlstring(state, value.c_str(), value.size());
			}
			break;
			case FieldDescriptor::CPPTYPE_BOOL:
				lua_pushboolean(state, reflection->GetBool(*message, fd));
				break;
			case FieldDescriptor::CPPTYPE_MESSAGE:
				push_proto_message_tolua(&(reflection->GetMessage(*message, fd)));
				break;
			default:
				break;
			}
		}

		lua_rawset(state, -3);
	}
}
// protobuf: message key, value --> lua table
// json: json string 传给lua
// {1: stype, 2: ctype, 3: utag, 4: body_table_or_str}
bool
lua_service::on_session_recv_cmd(session* s, struct cmd_msg* msg) {
	tolua_pushuserdata(lua_wrapper::lua_state(), (void*)s);
	int index = 1;

	lua_newtable(lua_wrapper::lua_state());
	lua_pushinteger(lua_wrapper::lua_state(), msg->stype);
	lua_rawseti(lua_wrapper::lua_state(), -2, index);          /* table[index] = value, L: table */
	++index;

	lua_pushinteger(lua_wrapper::lua_state(), msg->ctype);
	lua_rawseti(lua_wrapper::lua_state(), -2, index);          /* table[index] = value, L: table */
	++index;

	lua_pushinteger(lua_wrapper::lua_state(), msg->utag);
	lua_rawseti(lua_wrapper::lua_state(), -2, index);          /* table[index] = value, L: table */
	++index;

	if (!msg->body) {
		lua_pushnil(lua_wrapper::lua_state());
		lua_rawseti(lua_wrapper::lua_state(), -2, index);          /* table[index] = value, L: table */
		++index;
	}
	else {
		if (proto_man::proto_type() == PROTO_JSON) {
			lua_pushstring(lua_wrapper::lua_state(), (char*)msg->body);
		}
		else { // protobuf
			push_proto_message_tolua((Message*)msg->body);
		}
		lua_rawseti(lua_wrapper::lua_state(), -2, index);          /* table[index] = value, L: table */
		++index;
	}

	execute_service_function(this->lua_recv_cmd_handler, 2);
	return true;
}

bool
lua_service::on_session_recv_raw_cmd(session* s, struct raw_cmd* raw) {
	tolua_pushuserdata(lua_wrapper::lua_state(), (void*)s);
	tolua_pushuserdata(lua_wrapper::lua_state(), (void*)raw);

	execute_service_function(this->lua_recv_raw_handler, 2);
	return true;
}

void
lua_service::on_session_disconnect(session* s, int stype) {
	tolua_pushuserdata(lua_wrapper::lua_state(), (void*)s);
	lua_pushinteger(lua_wrapper::lua_state(), stype);
	execute_service_function(this->lua_disconnect_handler, 2);
}

void
lua_service::on_session_connect(session* s, int stype) {
	tolua_pushuserdata(lua_wrapper::lua_state(), (void*)s);
	lua_pushinteger(lua_wrapper::lua_state(), stype);
	if (this->lua_connect_handler)
	{
		execute_service_function(this->lua_connect_handler, 2);
	}
}

static int
lua_register_service(lua_State* tolua_S) {
	int stype = (int)tolua_tonumber(tolua_S, 1, 0);
	bool ret = false;
	// table
	if (!lua_istable(tolua_S, 2)) {
		goto lua_failed;
	}

	unsigned int lua_recv_cmd_handler;
	unsigned int lua_disconnect_handler;
	unsigned int lua_connect_handler;

	lua_getfield(tolua_S, 2, "on_session_recv_cmd");
	lua_getfield(tolua_S, 2, "on_session_disconnect");
	lua_getfield(tolua_S, 2, "on_session_connect");
	// stack 3 on_session_recv_cmd , 4on_session_disconnect
	lua_recv_cmd_handler = save_service_function(tolua_S, 3, 0);
	lua_disconnect_handler = save_service_function(tolua_S, 4, 0);
	lua_connect_handler = save_service_function(tolua_S, 5, 0);

	if (lua_recv_cmd_handler == 0 || lua_disconnect_handler == 0) {
		goto lua_failed;
	}

	// register service
	lua_service* s = new lua_service();
	s->using_raw_cmd = false;
	s->lua_disconnect_handler = lua_disconnect_handler;
	s->lua_recv_cmd_handler = lua_recv_cmd_handler;
	s->lua_recv_raw_handler = 0;
	s->lua_connect_handler = lua_connect_handler;
	ret = service_man::register_service(stype, s);
	// end 

lua_failed:
	lua_pushboolean(tolua_S, ret ? 1 : 0);
	return 1;
}

static int
lua_register_raw_service(lua_State* tolua_S) {
	int stype = (int)tolua_tonumber(tolua_S, 1, 0);
	bool ret = false;
	// table
	if (!lua_istable(tolua_S, 2)) {
		goto lua_failed;
	}

	unsigned int lua_recv_raw_handler;
	unsigned int lua_disconnect_handler;
	unsigned int lua_connect_handler;


	lua_getfield(tolua_S, 2, "on_session_recv_raw_cmd");
	lua_getfield(tolua_S, 2, "on_session_disconnect");
	lua_getfield(tolua_S, 2, "on_session_connect");

	// stack 3 on_session_recv_cmd , 4on_session_disconnect
	lua_recv_raw_handler = save_service_function(tolua_S, 3, 0);
	lua_disconnect_handler = save_service_function(tolua_S, 4, 0);
	lua_connect_handler = save_service_function(tolua_S, 5, 0);

	if (lua_recv_raw_handler == 0 || lua_disconnect_handler == 0) {
		goto lua_failed;
	}

	// register service
	lua_service* s = new lua_service();
	s->using_raw_cmd = true;
	s->lua_disconnect_handler = lua_disconnect_handler;
	s->lua_recv_cmd_handler = 0;
	s->lua_recv_raw_handler = lua_recv_raw_handler;
	s->lua_connect_handler = lua_connect_handler;

	ret = service_man::register_service(stype, s);
	// end 

lua_failed:
	lua_pushboolean(tolua_S, ret ? 1 : 0);
	return 1;
}

int
register_service_export(lua_State* tolua_S) {
	init_service_function_map(tolua_S);

	lua_getglobal(tolua_S, "_G");
	if (lua_istable(tolua_S, -1)) {
		tolua_open(tolua_S);
		tolua_module(tolua_S, "Service", 0);
		tolua_beginmodule(tolua_S, "Service");

		tolua_function(tolua_S, "register", lua_register_service);
		tolua_function(tolua_S, "register_with_raw", lua_register_raw_service);

		tolua_endmodule(tolua_S);
	}
	lua_pop(tolua_S, 1);

	return 0;
}