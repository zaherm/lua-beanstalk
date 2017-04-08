#include "lbeanstalk.h"
LUALIB_API int lbeanstalk_version(lua_State *L) {
  int major, minor, patch;
  bs_version(&major, &minor, &patch);
  char buf[16];
  sprintf(buf, "%d.%d.%d", major, minor, patch);
  lua_pushstring(L, buf);
  return 1;
}

LUALIB_API int lbeanstalk_status_text(lua_State *L) {
  int status = luaL_checkint(L, 1);
  const char *text = bs_status_text(status);
  lua_pushstring(L, text);
  return 1;
}

void lbeanstalk_push_status_map(lua_State *L) {

}

void lbeanstalk_push_job(lua_State *L, lbeanstalk_client *client, BSJ *job) {
  lbeanstalk_job *j = (lbeanstalk_job *) lua_newuserdata(L, sizeof(lbeanstalk_job));
  j->job = job;
  j->client = client;
  lbeanstalk_setmeta(L, "beanstalk.job");
}

/* beanstalk.job */
lbeanstalk_job *lbeanstalk_job_check(lua_State *L, int index) {
  lbeanstalk_job *j = (lbeanstalk_job *) luaL_checkudata(L, index, "beanstalk.job");
  luaL_argcheck(L, j != NULL, index, "beanstalk.job expected");
  return j;
}

LUALIB_API int lbeanstalk_job_free(lua_State *L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
  if(self->job != NULL) {
    bs_free_job(self->job);
    self->job = NULL;
  }
  return 0;
}

LUALIB_API int lbeanstalk_job_index(lua_State* L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
	const char* key = luaL_checkstring(L, 2);
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, key);
  /* return if the key present in the metatable */
	if(!lua_isnil(L, -1)) {
    return 1;
  }

	lua_settop(L, 2);
  if(strcmp(key, "id") == 0) {
    lua_pushinteger(L, self->job->id);
    return 1;
  }
  if(strcmp(key, "data") == 0) {
    lua_pushlstring(L, self->job->data, self->job->size);
    return 1;
  }
	return 0;
}

LUALIB_API int lbeanstalk_job_delete(lua_State *L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
  int ret = bs_delete(self->client->fd, self->job->id);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);
  return 2;
}  

LUALIB_API int lbeanstalk_job_release(lua_State *L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
  /* defaults are priority 0, delay of 0 */
  uint32_t priority = luaL_optint(L, 2, 0);
  uint32_t delay = luaL_optint(L, 3, 0);
  int ret = bs_release(self->client->fd, self->job->id, priority, delay);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);  
  return 2;
}

LUALIB_API int lbeanstalk_job_bury(lua_State *L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
  /* defaults are priority 0 */
  uint32_t priority = luaL_optint(L, 2, 0);
  int ret = bs_bury(self->client->fd, self->job->id, priority);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);  
  return 2;
}

LUALIB_API int lbeanstalk_job_touch(lua_State *L) {
  lbeanstalk_job *self = lbeanstalk_job_check(L, 1);
  int ret = bs_touch(self->client->fd, self->job->id);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);  
  return 2;
}

/* beanstalk.client */
lbeanstalk_client *lbeanstalk_client_check(lua_State *L, int index) {
  lbeanstalk_client *c = (lbeanstalk_client*) luaL_checkudata(L, index, "beanstalk.client");
  luaL_argcheck(L, c != NULL, index, "beanstalk.client expected");
  return c;
}

LUALIB_API int lbeanstalk_client_new(lua_State *L) {
  const char *host = luaL_checkstring(L, 1);
  int port = luaL_checkint(L, 2);
  float timeout = luaL_optnumber(L, 3, 0);
  int fd;
  if(timeout > 0) {
    fd = bs_connect_with_timeout(host, port, timeout);
  }
  else {
    fd = bs_connect(host, port);
  }

  if(fd != BS_STATUS_FAIL) {
    lbeanstalk_client *client = (lbeanstalk_client *) lua_newuserdata(L, sizeof(lbeanstalk_client));
    client->fd = fd;
    client->host = host;
    client->port = port;
    client->timeout = timeout;
    lbeanstalk_setmeta(L, "beanstalk.client");
  }
  else {
    lbeanstalk_error(L, "connection failed");
  }

  return 1;
}

LUALIB_API int lbeanstalk_client_index(lua_State* L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
	const char* key = luaL_checkstring(L, 2);
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, key);
	if(!lua_isnil(L, -1)) {
    return 1;
  }

  if(!strcmp(key, "addr")) {
    char addr[50];
    sprintf(addr, "%s:%d", self->host, self->port);
    lua_pushstring(L, addr);
    return 1;
  }

  return 0;
}

LUALIB_API int lbeanstalk_client_free(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  if(self->fd) {
    bs_disconnect(self->fd);
    self->fd = 0;
  }
  return 0;
}

LUALIB_API int lbeanstalk_client_use(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  const char *tube = luaL_checkstring(L, 2);
  int ret = bs_use(self->fd, tube);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);
  return 2;
}

LUALIB_API int lbeanstalk_client_watch(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  const char *tube = luaL_checkstring(L, 2);
  int ret = bs_watch(self->fd, tube);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);
  return 2;
}

LUALIB_API int lbeanstalk_client_ignore(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  const char *tube = luaL_checkstring(L, 2);
  int ret = bs_ignore(self->fd, tube);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);
  return 2;
}

LUALIB_API int lbeanstalk_client_put(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  size_t bytes;
  const char *data = luaL_checklstring(L, 2, &bytes);
  /* defaults are priority 0, delay of 0 and ttr of 120 seconds */
  uint32_t priority = luaL_optint(L, 3, 0);
  uint32_t delay = luaL_optint(L, 4, 0);
  uint32_t ttr = luaL_optnumber(L, 5, 120);
  int64_t ret = bs_put(self->fd, priority, delay, ttr, data, bytes);
  lua_pushboolean(L, ret >= 0);
  lua_pushinteger(L, ret);
  return 2;
}

LUALIB_API int lbeanstalk_client_reserve(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  uint32_t ttl = luaL_optnumber(L, 2, 0);
  int ret;
  BSJ *job;
  if(ttl > 0) {
    ret = bs_reserve_with_timeout(self->fd, ttl, &job);
  }
  else {
    ret = bs_reserve(self->fd, &job);
  }
  lua_pushboolean(L, ret == BS_STATUS_OK);
  if(ret == BS_STATUS_OK) {
    lbeanstalk_push_job(L, self, job);
  }
  else {
    lua_pushinteger(L, ret);
  }
  return 2;
}

LUALIB_API int lbeanstalk_client_peek(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  int64_t id = luaL_checkint(L, 2);
  BSJ *job;  
  int ret = bs_peek(self->fd, id, &job);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  if(ret == BS_STATUS_OK) {
    lbeanstalk_push_job(L, self, job);
  }
  else {
    lua_pushinteger(L, ret);
  }
  return 2;
}

LUALIB_API int lbeanstalk_client_peek_ready(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  BSJ *job;  
  int ret = bs_peek_ready(self->fd, &job);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  if(ret == BS_STATUS_OK) {
    lbeanstalk_push_job(L, self, job);
  }
  else {
    lua_pushinteger(L, ret);
  }
  return 2;
}

LUALIB_API int lbeanstalk_client_peek_delayed(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  BSJ *job;  
  int ret = bs_peek_delayed(self->fd, &job);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  if(ret == BS_STATUS_OK) {
    lbeanstalk_push_job(L, self, job);
  }
  else {
    lua_pushinteger(L, ret);
  }
  return 2;
}

LUALIB_API int lbeanstalk_client_peek_buried(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  BSJ *job;  
  int ret = bs_peek_buried(self->fd, &job);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  if(ret == BS_STATUS_OK) {
    lbeanstalk_push_job(L, self, job);
  }
  else {
    lua_pushinteger(L, ret);
  }
  return 2;
}

LUALIB_API int lbeanstalk_client_kick(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  int bound = luaL_checkint(L, 2);
  int ret = bs_kick(self->fd, bound);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushinteger(L, ret);
  return 2;
}

LUALIB_API int lbeanstalk_client_list_tube_used(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  char *yaml;
  int ret = bs_list_tube_used(self->fd, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}

LUALIB_API int lbeanstalk_client_list_tubes(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  char *yaml;
  int ret = bs_list_tubes(self->fd, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}

LUALIB_API int lbeanstalk_client_list_tubes_watched(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  char *yaml;
  int ret = bs_list_tubes_watched(self->fd, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}

LUALIB_API int lbeanstalk_client_stats(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  char *yaml;
  int ret = bs_stats(self->fd, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}

LUALIB_API int lbeanstalk_client_stats_job(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  int64_t id = luaL_checkint(L, 2);
  char *yaml;
  int ret = bs_stats_job(self->fd, id, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}


LUALIB_API int lbeanstalk_client_stats_tube(lua_State *L) {
  lbeanstalk_client *self = lbeanstalk_client_check(L, 1);
  const char *tube = luaL_checkstring(L, 2);
  char *yaml;
  int ret = bs_stats_tube(self->fd, tube, &yaml);
  lua_pushboolean(L, ret == BS_STATUS_OK);
  lua_pushstring(L, yaml);
  free(yaml);
  return 2;
}



LUALIB_API int luaopen_beanstalk(lua_State *L) {
  lua_newtable(L);
  lbeanstalk_createmeta(L, "beanstalk.client", lbeanstalk_client_methods);
  lbeanstalk_createmeta(L, "beanstalk.job", lbeanstalk_job_methods);
  /* push status map */
  for(int i = 0; lbeanstalk_status_map[i].name != NULL; i++) {
    lua_pushinteger(L, lbeanstalk_status_map[i].id);
    lua_setfield(L, -2, lbeanstalk_status_map[i].name);
  }

  luaL_setfuncs(L, lbeanstalk_funcs, 0);
  lua_pushliteral(L, LBEANSTALK_VERSION);
  lua_setfield(L, -2, "_VERSION");
  lua_pushliteral(L, LBEANSTALK_COPYRIGHT);
  lua_setfield(L, -2, "_COPYRIGHT");
  lua_pushliteral(L, LBEANSTALK_DESCRIPTION);
  lua_setfield(L, -2, "_DESCRIPTION");
  return 1;
}

