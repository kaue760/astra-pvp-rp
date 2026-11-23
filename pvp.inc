// ============================================================================
//  ASTRA PVP/RP  -  pvp.inc
//  Arenas, kills, streaks, killfeed e ranking.
//  Spawns e arenas reaproveitados do GM PVP original.
// ============================================================================
#define MAX_ARENAS 9
#define LOBBY_X    381.341522
#define LOBBY_Y    -2074.084472
#define LOBBY_Z    7.835937
#define LOBBY_R    0.0

new const ArenaNome[MAX_ARENAS][] =
{
    "Desert", "Mata-Mata", "Ammu", "PT2", "Predio", "M4 Cidade", "M4 Delegacia", "Combat Shotgun", "Sniper"
};
// Arma, municao e interior de cada arena (edite livremente)
new const ArenaArma[MAX_ARENAS]     = {24, 24, 24, 24, 24, 31, 31, 27, 34};
new const ArenaMunicao[MAX_ARENAS]  = {500, 500, 500, 500, 500, 500, 500, 300, 100};
new const ArenaInterior[MAX_ARENAS] = {0, 0, 0, 0, 0, 0, 3, 0, 0};

// {arena, x, y, z, angulo}
new const Float:ArenaSpawn[][5] =
{
    {0.0, -2074.236816, -2526.435302, 31.066806, 0.0},

    {1.0, 2050.990966, 2726.808837, 10.820312, 357.628173},
    {1.0, 2010.089721, 2796.835937, 21.949539, 126.453735},
    {1.0, 1997.299194, 2743.120605, 10.671875, 263.417510},
    {1.0, 1874.744262, 2717.989501, 10.671875, 275.612518},
    {1.0, 1928.437744, 2652.193115, 10.820312, 333.654693},

    {2.0, 1991.892089, 2473.178955, 11.512232, 89.026252},
    {2.0, 1998.110717, 2448.820312, 10.820312, 3.465601},
    {2.0, 1987.674438, 2461.500732, 11.512229, 25.316019},

    {3.0, 2476.908203, -562.803955, 1501.104980, 357.154541},
    {3.0, 2476.723632, -539.952880, 1501.104980, 181.118728},

    {4.0, 1541.587158, -1466.943969, 63.859375, 91.194038},
    {4.0, 1499.350830, -1467.172119, 63.859375, 276.434051},

    {5.0, -1507.954345, 2577.420410, 55.835937, 266.477386},
    {5.0, -1557.935424, 2652.690917, 55.840278, 270.787872},
    {5.0, -1390.495239, 2655.999267, 55.974086, 199.536499},

    {6.0, 238.634078, 195.710968, 1008.171875, 190.699676},
    {6.0, 212.125228, 158.734970, 1003.029968, 272.380157},
    {6.0, 239.261291, 142.151824, 1003.023437, 11.471279},

    {7.0, -1915.102905, 623.766784, 145.320312, 27.985593},
    {7.0, -1989.822998, 624.070861, 145.320312, 308.620330},
    {7.0, -1989.708007, 697.687500, 145.318069, 218.987915},
    {7.0, -1914.823974, 697.296081, 145.320312, 99.788810},

    {8.0, -2022.234497, -272.173492, 35.320312, 1.426424},
    {8.0, -2081.767333, -269.490417, 35.320312, 87.366355},
    {8.0, -2084.182617, -123.777076, 35.320312, 175.445663},
    {8.0, -2030.092773, -124.269126, 35.213527, 181.719879}
};

stock ArenaJogadores(arena)
{
    new c;
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i) && Player[i][pLogado] && Player[i][pArena] == arena) c++;
    return c;
}

stock ArenaSpawnAleatorio(arena, &Float:x, &Float:y, &Float:z, &Float:r)
{
    new idx[16], n;
    for(new i = 0; i < sizeof(ArenaSpawn); i++)
        if(floatround(ArenaSpawn[i][0]) == arena && n < sizeof(idx)) idx[n++] = i;
    if(!n) return 0;
    new k = idx[random(n)];
    x = ArenaSpawn[k][1];
    y = ArenaSpawn[k][2];
    z = ArenaSpawn[k][3];
    r = ArenaSpawn[k][4];
    return 1;
}

stock PVP_Equipar(playerid, arena)
{
    ResetPlayerWeapons(playerid);
    SetPlayerHealth(playerid, 100.0);
    SetPlayerArmour(playerid, 100.0);
    GivePlayerWeapon(playerid, ArenaArma[arena], ArenaMunicao[arena]);
    return 1;
}

stock PVP_Lobby(playerid)
{
    Player[playerid][pArena] = -1;
    Player[playerid][pStreak] = 0;
    ResetPlayerWeapons(playerid);
    SetPlayerHealth(playerid, 100.0);
    SetPlayerArmour(playerid, 0.0);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, VW_LOBBY_PVP);
    SetPlayerPos(playerid, LOBBY_X, LOBBY_Y, LOBBY_Z);
    SetPlayerFacingAngle(playerid, LOBBY_R);
    SetCameraBehindPlayer(playerid);
    return 1;
}

stock PVP_EntrarArena(playerid, arena)
{
    if(arena < 0 || arena >= MAX_ARENAS) return 0;
    new Float:x, Float:y, Float:z, Float:r, s[96];
    if(!ArenaSpawnAleatorio(arena, x, y, z, r)) return 0;
    Player[playerid][pArena] = arena;
    Player[playerid][pStreak] = 0;
    SetPlayerInterior(playerid, ArenaInterior[arena]);
    SetPlayerVirtualWorld(playerid, VW_ARENA_BASE + arena);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, r);
    SetCameraBehindPlayer(playerid);
    PVP_Equipar(playerid, arena);
    format(s, sizeof s, "Voce entrou na arena %s. Use /lobby para sair.", ArenaNome[arena]);
    Info(playerid, s);
    return 1;
}

stock PVP_Respawn(playerid)
{
    new arena = Player[playerid][pArena];
    if(arena < 0) { PVP_Lobby(playerid); return 1; }
    new Float:x, Float:y, Float:z, Float:r;
    if(!ArenaSpawnAleatorio(arena, x, y, z, r)) { PVP_Lobby(playerid); return 1; }
    SetPlayerInterior(playerid, ArenaInterior[arena]);
    SetPlayerVirtualWorld(playerid, VW_ARENA_BASE + arena);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, r);
    SetCameraBehindPlayer(playerid);
    PVP_Equipar(playerid, arena);
    return 1;
}

stock PVP_Morte(playerid, killerid, reason)
{
    if(!IsPVP(playerid)) return 0;
    Player[playerid][pDeaths]++;
    Player[playerid][pStreak] = 0;

    if(killerid != INVALID_PLAYER_ID && IsPVP(killerid) && killerid != playerid)
    {
        new arma[32], s[144], bonus;
        GetWeaponName(reason, arma, sizeof arma);
        Player[killerid][pKills]++;
        Player[killerid][pStreak]++;
        if(Player[killerid][pStreak] > Player[killerid][pMelhorStreak])
            Player[killerid][pMelhorStreak] = Player[killerid][pStreak];
        bonus = 500 + (Player[killerid][pStreak] * 50);
        DarDinheiro(killerid, bonus);
        SetPlayerHealth(killerid, 100.0);
        SetPlayerArmour(killerid, 100.0);

        format(s, sizeof s, "[KILL] {FFFFFF}%s matou %s ({FFD24D}%s{FFFFFF}) | streak: %d", PName(killerid), PName(playerid), arma, Player[killerid][pStreak]);
        for(new i = 0; i < MAX_PLAYERS; i++)
            if(IsPlayerConnected(i) && IsPVP(i) && Player[i][pArena] == Player[playerid][pArena]) SCM(i, 0xFF6B6BFF, s);

        if(Player[killerid][pStreak] >= 5 && Player[killerid][pStreak] % 5 == 0)
        {
            format(s, sizeof s, "[STREAK] {FFFFFF}%s esta com %d kills seguidas!", PName(killerid), Player[killerid][pStreak]);
            SendAll(COR_ADM, s);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Comandos PVP
// ---------------------------------------------------------------------------
CMD:arenas(playerid, params[])
{
    if(!IsPVP(playerid)) return Erro(playerid, "Este comando e do modo PVP. Use /modo.");
    new lista[512], linha[64];
    strcat(lista, "Arena\tJogadores\n");
    for(new i = 0; i < MAX_ARENAS; i++)
    {
        format(linha, sizeof linha, "{FFFFFF}[%d] %s\t{9B6BFF}%d\n", i + 1, ArenaNome[i], ArenaJogadores(i));
        strcat(lista, linha);
    }
    ShowPlayerDialog(playerid, D_ARENAS, DIALOG_STYLE_TABLIST_HEADERS, "Astra PVP - Arenas", lista, "Entrar", "Fechar");
    return 1;
}

CMD:lobby(playerid, params[])
{
    if(!IsPVP(playerid)) return Erro(playerid, "Este comando e do modo PVP.");
    PVP_Lobby(playerid);
    return Info(playerid, "Voce voltou ao lobby.");
}

CMD:stats(playerid, params[])
{
    new s[144], kd[16];
    format(kd, sizeof kd, "%.2f", Player[playerid][pDeaths] ? float(Player[playerid][pKills]) / float(Player[playerid][pDeaths]) : float(Player[playerid][pKills]));
    format(s, sizeof s, "Kills: %d | Mortes: %d | K/D: %s | Streak: %d (melhor: %d)",
        Player[playerid][pKills], Player[playerid][pDeaths], kd, Player[playerid][pStreak], Player[playerid][pMelhorStreak]);
    return Info(playerid, s);
}

CMD:rank(playerid, params[])
{
    new ids[MAX_PLAYERS], n, tmp, s[96];
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i) && Player[i][pLogado]) ids[n++] = i;
    for(new a = 0; a < n; a++)
        for(new b = a + 1; b < n; b++)
            if(Player[ids[b]][pKills] > Player[ids[a]][pKills]) { tmp = ids[a]; ids[a] = ids[b]; ids[b] = tmp; }
    SCM(playerid, COR_ASTRA, "===== Ranking Astra (online) =====");
    for(new i = 0; i < n && i < 10; i++)
    {
        format(s, sizeof s, "%d. %s - %d kills", i + 1, PName(ids[i]), Player[ids[i]][pKills]);
        SCM(playerid, COR_BRANCO, s);
    }
    return 1;
}
