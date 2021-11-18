﻿#if USE_UNI_LUA
using LuaAPI = UniLua.Lua;
using RealStatePtr = UniLua.ILuaState;
using LuaCSFunction = UniLua.CSharpFunctionDelegate;
#else
using LuaAPI = XLua.LuaDLL.Lua;
using RealStatePtr = System.IntPtr;
using LuaCSFunction = XLua.LuaDLL.lua_CSFunction;
#endif

using XLua;
using System.Collections.Generic;


namespace XLua.CSObjectWrap
{
    using Utils = XLua.Utils;
    public class ugameWrap 
    {
        public static void __Register(RealStatePtr L)
        {
			ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			System.Type type = typeof(ugame);
			Utils.BeginObjectRegister(type, L, translator, 0, 3, 6, 6);
			
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "save_uinfo", _m_save_uinfo);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "save_edit_profile", _m_save_edit_profile);
			Utils.RegisterFunc(L, Utils.METHOD_IDX, "user_login_out", _m_user_login_out);
			
			
			Utils.RegisterFunc(L, Utils.GETTER_IDX, "unick", _g_get_unick);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "uface", _g_get_uface);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "usex", _g_get_usex);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "uvip", _g_get_uvip);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "is_guest", _g_get_is_guest);
            Utils.RegisterFunc(L, Utils.GETTER_IDX, "guest_key", _g_get_guest_key);
            
			Utils.RegisterFunc(L, Utils.SETTER_IDX, "unick", _s_set_unick);
            Utils.RegisterFunc(L, Utils.SETTER_IDX, "uface", _s_set_uface);
            Utils.RegisterFunc(L, Utils.SETTER_IDX, "usex", _s_set_usex);
            Utils.RegisterFunc(L, Utils.SETTER_IDX, "uvip", _s_set_uvip);
            Utils.RegisterFunc(L, Utils.SETTER_IDX, "is_guest", _s_set_is_guest);
            Utils.RegisterFunc(L, Utils.SETTER_IDX, "guest_key", _s_set_guest_key);
            
			
			Utils.EndObjectRegister(type, L, translator, null, null,
			    null, null, null);

		    Utils.BeginClassRegister(type, L, __CreateInstance, 1, 0, 0);
			
			
            
			
			
			
			Utils.EndClassRegister(type, L, translator);
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int __CreateInstance(RealStatePtr L)
        {
            
			try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
				if(LuaAPI.lua_gettop(L) == 1)
				{
					
					var gen_ret = new ugame();
					translator.Push(L, gen_ret);
                    
					return 1;
				}
				
			}
			catch(System.Exception gen_e) {
				return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
			}
            return LuaAPI.luaL_error(L, "invalid arguments to ugame constructor!");
            
        }
        
		
        
		
        
        
        
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_save_uinfo(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
            
            
			    int gen_param_count = LuaAPI.lua_gettop(L);
            
                if(gen_param_count == 4&& translator.Assignable<gprotocol.UserCenterInfo>(L, 2)&& LuaTypes.LUA_TBOOLEAN == LuaAPI.lua_type(L, 3)&& (LuaAPI.lua_isnil(L, 4) || LuaAPI.lua_type(L, 4) == LuaTypes.LUA_TSTRING)) 
                {
                    gprotocol.UserCenterInfo _uinfo = (gprotocol.UserCenterInfo)translator.GetObject(L, 2, typeof(gprotocol.UserCenterInfo));
                    bool _is_guest = LuaAPI.lua_toboolean(L, 3);
                    string _guest_key = LuaAPI.lua_tostring(L, 4);
                    
                    gen_to_be_invoked.save_uinfo( _uinfo, _is_guest, _guest_key );
                    
                    
                    
                    return 0;
                }
                if(gen_param_count == 3&& translator.Assignable<gprotocol.UserCenterInfo>(L, 2)&& LuaTypes.LUA_TBOOLEAN == LuaAPI.lua_type(L, 3)) 
                {
                    gprotocol.UserCenterInfo _uinfo = (gprotocol.UserCenterInfo)translator.GetObject(L, 2, typeof(gprotocol.UserCenterInfo));
                    bool _is_guest = LuaAPI.lua_toboolean(L, 3);
                    
                    gen_to_be_invoked.save_uinfo( _uinfo, _is_guest );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
            return LuaAPI.luaL_error(L, "invalid arguments to ugame.save_uinfo!");
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_save_edit_profile(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
            
            
                
                {
                    string _unick = LuaAPI.lua_tostring(L, 2);
                    int _uface = LuaAPI.xlua_tointeger(L, 3);
                    int _usex = LuaAPI.xlua_tointeger(L, 4);
                    
                    gen_to_be_invoked.save_edit_profile( _unick, _uface, _usex );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _m_user_login_out(RealStatePtr L)
        {
		    try {
            
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
            
            
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
            
            
                
                {
                    
                    gen_to_be_invoked.user_login_out(  );
                    
                    
                    
                    return 0;
                }
                
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            
        }
        
        
        
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_unick(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushstring(L, gen_to_be_invoked.unick);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_uface(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.xlua_pushinteger(L, gen_to_be_invoked.uface);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_usex(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.xlua_pushinteger(L, gen_to_be_invoked.usex);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_uvip(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.xlua_pushinteger(L, gen_to_be_invoked.uvip);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_is_guest(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushboolean(L, gen_to_be_invoked.is_guest);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _g_get_guest_key(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                LuaAPI.lua_pushstring(L, gen_to_be_invoked.guest_key);
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 1;
        }
        
        
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_unick(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.unick = LuaAPI.lua_tostring(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_uface(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.uface = LuaAPI.xlua_tointeger(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_usex(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.usex = LuaAPI.xlua_tointeger(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_uvip(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.uvip = LuaAPI.xlua_tointeger(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_is_guest(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.is_guest = LuaAPI.lua_toboolean(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
        [MonoPInvokeCallbackAttribute(typeof(LuaCSFunction))]
        static int _s_set_guest_key(RealStatePtr L)
        {
		    try {
                ObjectTranslator translator = ObjectTranslatorPool.Instance.Find(L);
			
                ugame gen_to_be_invoked = (ugame)translator.FastGetCSObj(L, 1);
                gen_to_be_invoked.guest_key = LuaAPI.lua_tostring(L, 2);
            
            } catch(System.Exception gen_e) {
                return LuaAPI.luaL_error(L, "c# exception:" + gen_e);
            }
            return 0;
        }
        
		
		
		
		
    }
}