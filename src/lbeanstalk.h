#ifndef LBEANSTALK_H
#define LBEANSTALK_H

#include "beanstalk.h"
#include "lbeanstalk_aux.h"

#define LBEANSTALK_VERSION "lua-beanstalk 0.0.1"
#define LBEANSTALK_COPYRIGHT "Copyright (c) 2017, Zaher Marzuq"
#define LBEANSTALK_DESCRIPTION "beanstalk binding for Lua"


/* beanstalk.client */
typedef struct {
  int fd;
  const char *host;
  int port;
  float timeout;
} lbeanstalk_client;

/* beanstalk.job */
typedef struct {
  BSJ *job;
  lbeanstalk_client *client;
} lbeanstalk_job;

struct {
  const char *name;
  int id;
} lbeanstalk_status_map[] = {
  { "STATUS_OK", BS_STATUS_OK },
  { "STATUS_FAIL", BS_STATUS_FAIL },
  { "STATUS_EXPECTED_CRLF", BS_STATUS_EXPECTED_CRLF },
  { "STATUS_JOB_TOO_BIG", BS_STATUS_JOB_TOO_BIG },
  { "STATUS_DRAINING", BS_STATUS_DRAINING },
  { "STATUS_TIMED_OUT", BS_STATUS_TIMED_OUT },
  { "STATUS_NOT_FOUND", BS_STATUS_NOT_FOUND },
  { "STATUS_DEADLINE_SOON", BS_STATUS_DEADLINE_SOON },
  { "STATUS_BURIED", BS_STATUS_BURIED },
  { "STATUS_NOT_IGNORED", BS_STATUS_NOT_IGNORED },
  { NULL, 1 }
};

LUALIB_API int lbeanstalk_version(lua_State *L);
LUALIB_API int lbeanstalk_status_text(lua_State *L);

void lbeanstalk_push_status_map(lua_State *L);
void lbeanstalk_push_job(lua_State *L, lbeanstalk_client *client, BSJ *job);

lbeanstalk_job *lbeanstalk_job_check(lua_State *L, int index);
LUALIB_API int lbeanstalk_job_new(lua_State *L);
LUALIB_API int lbeanstalk_job_free(lua_State *L);
LUALIB_API int lbeanstalk_job_index(lua_State *L);
LUALIB_API int lbeanstalk_job_delete(lua_State *L);
LUALIB_API int lbeanstalk_job_release(lua_State *L);
LUALIB_API int lbeanstalk_job_bury(lua_State *L);
LUALIB_API int lbeanstalk_job_touch(lua_State *L);

lbeanstalk_client *lbeanstalk_client_check(lua_State *L, int index);
LUALIB_API int lbeanstalk_client_new(lua_State *L);
LUALIB_API int lbeanstalk_client_free(lua_State *L);
LUALIB_API int lbeanstalk_client_index(lua_State *L);
LUALIB_API int lbeanstalk_client_use(lua_State *L);
LUALIB_API int lbeanstalk_client_watch(lua_State *L);
LUALIB_API int lbeanstalk_client_ignore(lua_State *L);
LUALIB_API int lbeanstalk_client_put(lua_State *L);
LUALIB_API int lbeanstalk_client_reserve(lua_State *L);
LUALIB_API int lbeanstalk_client_peek(lua_State *L);
LUALIB_API int lbeanstalk_client_peek_ready(lua_State *L);
LUALIB_API int lbeanstalk_client_peek_delayed(lua_State *L);
LUALIB_API int lbeanstalk_client_peek_buried(lua_State *L);
LUALIB_API int lbeanstalk_client_kick(lua_State *L);
LUALIB_API int lbeanstalk_client_list_tube_used(lua_State *L);
LUALIB_API int lbeanstalk_client_list_tubes(lua_State *L);
LUALIB_API int lbeanstalk_client_list_tubes_watched(lua_State *L);
LUALIB_API int lbeanstalk_client_stats(lua_State *L);
LUALIB_API int lbeanstalk_client_stats_job(lua_State *L);
LUALIB_API int lbeanstalk_client_stats_tube(lua_State *L);

static luaL_Reg lbeanstalk_job_methods[] = {
  { "__gc", lbeanstalk_job_free },
  { "__index", lbeanstalk_job_index },
  { "delete", lbeanstalk_job_delete },
  { "release", lbeanstalk_job_release },
  { "bury", lbeanstalk_job_bury },
  { "touch", lbeanstalk_job_touch },
  { NULL, NULL }
};

static luaL_Reg lbeanstalk_client_methods[] = {
  { "__gc", lbeanstalk_client_free },
  { "__index", lbeanstalk_client_index },
  { "disconnect", lbeanstalk_client_free },
  { "use", lbeanstalk_client_use },
  { "watch", lbeanstalk_client_watch },
  { "ignore", lbeanstalk_client_ignore },
  { "put", lbeanstalk_client_put },
  { "reserve", lbeanstalk_client_reserve },
  { "peek", lbeanstalk_client_peek },
  { "peek_ready", lbeanstalk_client_peek_ready },
  { "peek_delayed", lbeanstalk_client_peek_delayed },
  { "peek_buried", lbeanstalk_client_peek_buried },
  { "kick", lbeanstalk_client_kick },
  { "list_tube_used", lbeanstalk_client_list_tube_used },
  { "list_tubes", lbeanstalk_client_list_tubes },
  { "list_tubes_watched", lbeanstalk_client_list_tubes_watched },
  { "stats", lbeanstalk_client_stats },
  { "stats_job", lbeanstalk_client_stats_job },
  { "stats_tube", lbeanstalk_client_stats_tube },
  { NULL, NULL }
};

static const luaL_Reg lbeanstalk_funcs[] = {
  { "version", lbeanstalk_version },
  { "status_text", lbeanstalk_status_text },
  { "client", lbeanstalk_client_new },
  { NULL, NULL }
};

#endif

