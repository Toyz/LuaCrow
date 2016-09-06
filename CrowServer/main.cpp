#include "crow\crow.h"
#include <string>
#include "EdUrlParser.h"

#include "lua\LuaBridge.h"
extern "C" {
	# include "lua.h"
	# include "lualib.h"
	# include "lauxlib.h"
}

crow::SimpleApp app;
lua_State* L;

void print(std::string s) {
	CROW_LOG_INFO << s;
}

void handler(std::string route, std::string funcName) {
	std::string r = route;
	app.route_dynamic(std::move(r))
	([funcName] (const crow::request& req, crow::response& res) {
		luabridge::LuaRef doFunc = luabridge::getGlobal(L, funcName.c_str());
		luabridge::LuaRef v(L);
		v = luabridge::newTable(L);

		EdUrlParser* url = EdUrlParser::parseUrl(req.raw_url);
		vector<query_kv_t> kvs;
		int num = EdUrlParser::parseKeyValueList(&kvs, url->query);
		for (int i = 0; i<num; i++) {
			v[kvs[i].key] = kvs[i].val;
		}

		std::string data = doFunc(v);

		res.write(data);
		res.end();
	});
}

int main() {
	L = luaL_newstate();
	luaL_openlibs(L);

	luabridge::getGlobalNamespace(L).
		beginNamespace("router")
		.addFunction("handle", handler)
		.endNamespace();

	/*luabridge::getGlobalNamespace(L).
		beginClass<crow::query_string>("query_string")
		.addConstructor <void(*) (const std::string&)>()
		.addConstructor <void(*) (const  crow::query_string&)>()
		.addFunction("get", &crow::query_string::get)
		.addFunction("get_list", &crow::query_string::get_list)
		.endClass();*/

	luabridge::getGlobalNamespace(L).addFunction("log", print);

	luaL_dofile(L, "test.lua");

	luabridge::LuaRef init = luabridge::getGlobal(L, "init");
	init();

	app.port(8080).multithreaded(15).run();

	return 0;
}