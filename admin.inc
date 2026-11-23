// ============================================================================
//  ASTRA PVP/RP  -  admin.inc
//  Niveis: 1 Ajudante | 2 Moderador | 3 Admin | 4 Gerente | 5 Dono
// ============================================================================
new const NomeAdmin[][] = {"Jogador", "Ajudante", "Moderador", "Administrador", "Gerente", "Dono"};

CMD:setadm(playerid, params[])
{
    // O primeiro dono deve ser definido via RCON: "rcon setadm <id> 5" (ver OnRconCommand)
    if(!IsAdmin(playerid, 5)) return 1;
    new id, nivel, s[96];
    if(sscanf(params, "ud", id, nivel) || nivel < 0 || nivel > 5) return Info(playerid, "Uso: /setadm [id] [0-5]");
    if(id == INVALID_PLAYER_ID || !Player[id][pLogado]) return Erro(playerid, "Jogador invalido.");
    Player[id][pAdmin] = nivel;
    SalvarConta(id);
    format(s, sizeof s, "%s agora e %s.", PName(id), NomeAdmin[nivel]);
    SendAdmins(s);
    return Info(id, s);
}

CMD:a(playerid, params[])
{
    if(!IsAdmin(playerid, 1)) return 1;
    if(isnull(params)) return Info(playerid, "Uso: /a [mensagem]");
    new s[160];
    format(s, sizeof s, "[ADM] %s %s: %s", NomeAdmin[Player[playerid][pAdmin]], PName(playerid), params);
    return SendAdmins(s);
}

CMD:kick(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    new id, motivo[64], s[160];
    if(sscanf(params, "us[64]", id, motivo)) return Info(playerid, "Uso: /kick [id] [motivo]");
    if(id == INVALID_PLAYER_ID) return Erro(playerid, "Jogador invalido.");
    if(Player[id][pAdmin] >= Player[playerid][pAdmin]) return Erro(playerid, "Voce nao pode punir alguem de nivel igual ou maior.");
    format(s, sizeof s, "[PUNICAO] %s foi expulso por %s. Motivo: %s", PName(id), PName(playerid), motivo);
    SendAll(COR_ERRO, s);
    return KickComDelay(id);
}

CMD:ban(playerid, params[])
{
    if(!IsAdmin(playerid, 3)) return 1;
    new id, motivo[64], s[160], arq[64];
    if(sscanf(params, "us[64]", id, motivo)) return Info(playerid, "Uso: /ban [id] [motivo]");
    if(id == INVALID_PLAYER_ID) return Erro(playerid, "Jogador invalido.");
    if(Player[id][pAdmin] >= Player[playerid][pAdmin]) return Erro(playerid, "Voce nao pode punir alguem de nivel igual ou maior.");
    format(arq, sizeof arq, PASTA_BANS, PName(id));
    dini_Create(arq);
    dini_Set(arq, "Motivo", motivo);
    dini_Set(arq, "Admin", PName(playerid));
    format(s, sizeof s, "[PUNICAO] %s foi banido por %s. Motivo: %s", PName(id), PName(playerid), motivo);
    SendAll(COR_ERRO, s);
    return KickComDelay(id);
}

CMD:unban(playerid, params[])
{
    if(!IsAdmin(playerid, 3)) return 1;
    new nome[MAX_PLAYER_NAME], arq[64];
    if(sscanf(params, "s[24]", nome)) return Info(playerid, "Uso: /unban [nome]");
    format(arq, sizeof arq, PASTA_BANS, nome);
    if(!dini_Exists(arq)) return Erro(playerid, "Esse nome nao esta banido.");
    dini_Remove(arq);
    return Info(playerid, "Jogador desbanido.");
}

CMD:mute(playerid, params[])
{
    if(!IsAdmin(playerid, 1)) return 1;
    new id, minutos, s[96];
    if(sscanf(params, "ud", id, minutos) || minutos < 0) return Info(playerid, "Uso: /mute [id] [minutos] (0 = desmutar)");
    if(id == INVALID_PLAYER_ID) return Erro(playerid, "Jogador invalido.");
    Player[id][pMute] = minutos;
    if(minutos) format(s, sizeof s, "%s foi mutado por %s (%d min).", PName(id), PName(playerid), minutos);
    else format(s, sizeof s, "%s foi desmutado por %s.", PName(id), PName(playerid));
    return SendAdmins(s);
}

CMD:ir(playerid, params[])
{
    if(!IsAdmin(playerid, 1)) return 1;
    new id, Float:x, Float:y, Float:z;
    if(sscanf(params, "u", id) || id == INVALID_PLAYER_ID) return Info(playerid, "Uso: /ir [id]");
    GetPlayerPos(id, x, y, z);
    SetPlayerInterior(playerid, GetPlayerInterior(id));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(id));
    return SetPlayerPos(playerid, x + 1.0, y, z);
}

CMD:trazer(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    new id, Float:x, Float:y, Float:z;
    if(sscanf(params, "u", id) || id == INVALID_PLAYER_ID) return Info(playerid, "Uso: /trazer [id]");
    GetPlayerPos(playerid, x, y, z);
    SetPlayerInterior(id, GetPlayerInterior(playerid));
    SetPlayerVirtualWorld(id, GetPlayerVirtualWorld(playerid));
    return SetPlayerPos(id, x + 1.0, y, z);
}

CMD:cv(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    new modelo, c1, c2, Float:x, Float:y, Float:z, Float:r;
    if(sscanf(params, "dD(1)D(1)", modelo, c1, c2) || modelo < 400 || modelo > 611) return Info(playerid, "Uso: /cv [modelo 400-611] [cor1] [cor2]");
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, r);
    new v = CreateVehicle(modelo, x + 2.0, y, z, r, c1, c2, -1);
    SetVehicleVirtualWorld(v, GetPlayerVirtualWorld(playerid));
    return PutPlayerInVehicle(playerid, v, 0);
}

CMD:dv(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    if(!IsPlayerInAnyVehicle(playerid)) return Erro(playerid, "Entre em um veiculo.");
    DestroyVehicle(GetPlayerVehicleID(playerid));
    return Info(playerid, "Veiculo removido.");
}

CMD:setskin(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    new id, skin;
    if(sscanf(params, "ud", id, skin) || skin < 0 || skin > 311 || id == INVALID_PLAYER_ID) return Info(playerid, "Uso: /setskin [id] [0-311]");
    Player[id][pSkin] = skin;
    return SetPlayerSkin(id, skin);
}

CMD:darmoney(playerid, params[])
{
    if(!IsAdmin(playerid, 4)) return 1;
    new id, v;
    if(sscanf(params, "ud", id, v) || id == INVALID_PLAYER_ID) return Info(playerid, "Uso: /darmoney [id] [valor]");
    DarDinheiro(id, v);
    return Info(playerid, "Dinheiro entregue.");
}

CMD:setlevel(playerid, params[])
{
    if(!IsAdmin(playerid, 4)) return 1;
    new id, v;
    if(sscanf(params, "ud", id, v) || id == INVALID_PLAYER_ID || v < 1) return Info(playerid, "Uso: /setlevel [id] [level]");
    Player[id][pLevel] = v;
    SetPlayerScore(id, v);
    return Info(playerid, "Level alterado.");
}

CMD:clima(playerid, params[])
{
    if(!IsAdmin(playerid, 3)) return 1;
    new c;
    if(sscanf(params, "d", c)) return Info(playerid, "Uso: /clima [id]");
    return SetWeather(c);
}

CMD:hora(playerid, params[])
{
    if(!IsAdmin(playerid, 3)) return 1;
    new h;
    if(sscanf(params, "d", h) || h < 0 || h > 23) return Info(playerid, "Uso: /hora [0-23]");
    for(new i = 0; i < MAX_PLAYERS; i++) if(IsPlayerConnected(i)) SetPlayerTime(i, h, 0);
    return 1;
}

CMD:limparchat(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    for(new l = 0; l < 40; l++) SendAll(COR_BRANCO, " ");
    return 1;
}

CMD:aviso(playerid, params[])
{
    if(!IsAdmin(playerid, 2)) return 1;
    if(isnull(params)) return Info(playerid, "Uso: /aviso [mensagem]");
    new s[160];
    format(s, sizeof s, "[AVISO] {FFFFFF}%s", params);
    return SendAll(COR_ADM, s);
}

CMD:tv(playerid, params[])
{
    if(!IsAdmin(playerid, 1)) return 1;
    new id;
    if(sscanf(params, "u", id) || id == INVALID_PLAYER_ID || id == playerid) return Info(playerid, "Uso: /tv [id]");
    TogglePlayerSpectating(playerid, 1);
    SetPlayerInterior(playerid, GetPlayerInterior(id));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(id));
    if(IsPlayerInAnyVehicle(id)) PlayerSpectateVehicle(playerid, GetPlayerVehicleID(id));
    else PlayerSpectatePlayer(playerid, id);
    return 1;
}

CMD:tvoff(playerid, params[])
{
    if(!IsAdmin(playerid, 1)) return 1;
    TogglePlayerSpectating(playerid, 0);
    return 1;
}
