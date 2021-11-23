local Stype = require("Stype")
local Cmd = require("Cmd")
local game_mgr = require("logic_server/game_mgr")
local Respones = require("Respones")

local logic_service_hanlders = {}
logic_service_hanlders[Cmd.eLoginLogicReq] = game_mgr.login_logic_server
logic_service_hanlders[Cmd.eUserLostConn] = game_mgr.on_player_disconnect

-- {stype, ctype, utag, body}
function on_logic_recv_cmd(s, msg)
	print(msg[1], msg[2], msg[3], msg[4])
	if logic_service_hanlders[msg[2]] then
		logic_service_hanlders[msg[2]](s, msg)
	end
end

function on_gateway_disconnect(s, stype) 
	print("logic service disconnect with gateway")
	game_mgr.on_gateway_disconnect(s)
end

function on_gateway_connect(s, stype)
	print("gateway connect to Logic")
	game_mgr.on_gateway_connect(s)
end

local logic_service = {
	on_session_recv_cmd = on_logic_recv_cmd,
	on_session_disconnect = on_gateway_disconnect,
	on_session_connect = on_gateway_connect,
}

return logic_service