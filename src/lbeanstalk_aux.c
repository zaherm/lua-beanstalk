#include "lbeanstalk_aux.h"

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM==501
LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}
#endif


void lbeanstalk_error(lua_State *L, const char *msg) {
  luaL_error(L, msg);
}

void lbeanstalk_setmeta(lua_State *L, const char *name) {
  luaL_getmetatable(L, name);
  lua_setmetatable(L, -2);
}

void lbeanstalk_createmeta(lua_State *L, const char *name, const luaL_Reg *methods) {
  luaL_newmetatable(L, name);
  luaL_setfuncs(L, methods, 0);
  lua_pushliteral(L, "__name");
  lua_setfield(L, -2, name);
  lua_pop(L, 1);
}

void lbeanstalk_assert(lua_State *L, int cond, const char *msg)  {
  if(!cond) {
    lbeanstalk_error(L, msg);
  }
}


