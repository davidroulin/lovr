#include "api.h"
#include "graphics/animator.h"

int l_lovrAnimatorReset(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  lovrAnimatorReset(animator);
  return 0;
}

int l_lovrAnimatorUpdate(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  float dt = luax_checkfloat(L, 2);
  lovrAnimatorUpdate(animator, dt);
  return 0;
}

int l_lovrAnimatorGetAnimationCount(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  lua_pushnumber(L, lovrAnimatorGetAnimationCount(animator));
  return 1;
}

int l_lovrAnimatorGetAnimationNames(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  int animationCount = lovrAnimatorGetAnimationCount(animator);

  if (lua_istable(L, 2)) {
    lua_settop(L, 2);
  } else {
    lua_settop(L, 1);
    lua_createtable(L, animationCount, 0);
  }

  for (int i = 0; i < animationCount; i++) {
    lua_pushstring(L, lovrAnimatorGetAnimationName(animator, i));
    lua_rawseti(L, -2, i + 1);
  }

  return 1;
}

int l_lovrAnimatorPlay(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  lovrAnimatorPlay(animator, animation);
  return 0;
}

int l_lovrAnimatorStop(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  lovrAnimatorStop(animator, animation);
  return 0;
}

int l_lovrAnimatorPause(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  lovrAnimatorPause(animator, animation);
  return 0;
}

int l_lovrAnimatorResume(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  lovrAnimatorResume(animator, animation);
  return 0;
}

int l_lovrAnimatorSeek(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  float time = luax_checkfloat(L, 3);
  lovrAnimatorSeek(animator, animation, time);
  return 0;
}

int l_lovrAnimatorTell(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  float time = lovrAnimatorTell(animator, animation);
  lua_pushnumber(L, time);
  return 1;
}

int l_lovrAnimatorGetAlpha(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  float alpha = lovrAnimatorGetAlpha(animator, animation);
  lua_pushnumber(L, alpha);
  return 1;
}

int l_lovrAnimatorSetAlpha(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  float alpha = luax_checkfloat(L, 3);
  lovrAnimatorSetAlpha(animator, animation, alpha);
  return 0;
}

int l_lovrAnimatorGetDuration(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  float duration = lovrAnimatorGetDuration(animator, animation);
  lua_pushnumber(L, duration);
  return 1;
}

int l_lovrAnimatorIsPlaying(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  bool playing = lovrAnimatorIsPlaying(animator, animation);
  lua_pushboolean(L, playing);
  return 1;
}

int l_lovrAnimatorIsLooping(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  bool looping = lovrAnimatorIsLooping(animator, animation);
  lua_pushboolean(L, looping);
  return 1;
}

int l_lovrAnimatorSetLooping(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  bool looping = lua_toboolean(L, 3);
  lovrAnimatorSetLooping(animator, animation, looping);
  return 0;
}

int l_lovrAnimatorGetPriority(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  int priority = lovrAnimatorGetPriority(animator, animation);
  lua_pushinteger(L, priority);
  return 1;
}

int l_lovrAnimatorSetPriority(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  const char* animation = luaL_checkstring(L, 2);
  int priority = luaL_checkinteger(L, 3);
  lovrAnimatorSetPriority(animator, animation, priority);
  return 0;
}

int l_lovrAnimatorGetSpeed(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  if (lua_type(L, 2) == LUA_TSTRING) {
    const char* animation = luaL_checkstring(L, 2);
    float speed = lovrAnimatorGetSpeed(animator, animation);
    lua_pushnumber(L, speed);
  } else {
    float speed = lovrAnimatorGetSpeed(animator, NULL);
    lua_pushnumber(L, speed);
  }
  return 1;
}

int l_lovrAnimatorSetSpeed(lua_State* L) {
  Animator* animator = luax_checktype(L, 1, Animator);
  if (lua_type(L, 2) == LUA_TSTRING) {
    const char* animation = luaL_checkstring(L, 2);
    float speed = luax_checkfloat(L, 3);
    lovrAnimatorSetSpeed(animator, animation, speed);
  } else {
    float speed = luax_checkfloat(L, 2);
    lovrAnimatorSetSpeed(animator, NULL, speed);
  }
  return 0;
}

const luaL_Reg lovrAnimator[] = {
  { "reset", l_lovrAnimatorReset },
  { "update", l_lovrAnimatorUpdate },
  { "getAnimationCount", l_lovrAnimatorGetAnimationCount },
  { "getAnimationNames", l_lovrAnimatorGetAnimationNames },
  { "play", l_lovrAnimatorPlay },
  { "stop", l_lovrAnimatorStop },
  { "pause", l_lovrAnimatorPause },
  { "resume", l_lovrAnimatorResume },
  { "seek", l_lovrAnimatorSeek },
  { "tell", l_lovrAnimatorTell },
  { "getAlpha", l_lovrAnimatorGetAlpha },
  { "setAlpha", l_lovrAnimatorSetAlpha },
  { "getDuration", l_lovrAnimatorGetDuration },
  { "isPlaying", l_lovrAnimatorIsPlaying },
  { "isLooping", l_lovrAnimatorIsLooping },
  { "setLooping", l_lovrAnimatorSetLooping },
  { "getPriority", l_lovrAnimatorGetPriority },
  { "setPriority", l_lovrAnimatorSetPriority },
  { "getSpeed", l_lovrAnimatorGetSpeed },
  { "setSpeed", l_lovrAnimatorSetSpeed },
  { NULL, NULL }
};
