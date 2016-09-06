#include "crow/crow.h"
#include <string>
#include <memory>
#include "EdUrlParser.h"

#include "lua/LuaBridge.h"
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

void handler(std::string route, luabridge::LuaRef funcName) {
	app.route_dynamic(std::move(route))
	([=] (const crow::request& req, crow::response& res) {
		//crow::response r(std::move(res));

		luabridge::LuaRef v(L);
		v = luabridge::newTable(L);

		EdUrlParser* url = EdUrlParser::parseUrl(req.raw_url);
		vector<query_kv_t> kvs;
		int num = EdUrlParser::parseKeyValueList(&kvs, url->query);
		for (int i = 0; i<num; i++) {
			v[kvs[i].key] = kvs[i].val;
		}
		
		std::string data = funcName(v);

		res.set_header("Content-Type", "text/html");
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

	luabridge::getGlobalNamespace(L).addFunction("log", print);

	luaL_dofile(L, "main.lua");

	luabridge::LuaRef init = luabridge::getGlobal(L, "init");
	init();

	app.port(8080).multithreaded(15).run();

	return 0;
}