// ============================================================================
//  ASTRA PVP/RP  -  accounts.inc
//  Registro, login (senha com SHA256 + salt) e salvamento.
//  Obs.: o GM antigo guardava a senha em texto puro. Aqui ela e hasheada.
// ============================================================================
stock ArquivoConta(playerid, out[], len = sizeof out)
{
    format(out, len, PASTA_CONTAS, PName(playerid));
    return 1;
}

stock GerarSalt(out[], len = 16)
{
    static const cs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for(new i = 0; i < len; i++) out[i] = cs[random(sizeof(cs) - 1)];
    out[len] = 0;
    return 1;
}

stock ResetarJogador(playerid)
{
    Player[playerid][pLogado] = false;
    Player[playerid][pDinheiro] = 0;
    Player[playerid][pBanco] = 0;
    Player[playerid][pLevel] = 1;
    Player[playerid][pSkin] = 29;
    Player[playerid][pAdmin] = 0;
    Player[playerid][pKills] = 0;
    Player[playerid][pDeaths] = 0;
    Player[playerid][pEmprego] = EMP_DESEMPREGADO;
    Player[playerid][pFaccao] = FAC_NENHUMA;
    Player[playerid][pCargo] = 0;
    Player[playerid][pCasa] = -1;
    Player[playerid][pMute] = 0;
    Player[playerid][pMinutos] = 0;
    Player[playerid][pModo] = MODO_NENHUM;
    Player[playerid][pArena] = -1;
    Player[playerid][pStreak] = 0;
    Player[playerid][pMelhorStreak] = 0;
    Player[playerid][pTentativas] = 0;
    Player[playerid][pDuty] = false;
    PHash[playerid][0] = 0;
    PSalt[playerid][0] = 0;
    return 1;
}

stock MostrarLoginOuRegistro(playerid)
{
    new arq[64], s[200];
    ArquivoConta(playerid, arq);
    if(dini_Exists(arq))
    {
        format(s, sizeof s, "{FFFFFF}Bem-vindo de volta, {9B6BFF}%s{FFFFFF}!\nDigite sua senha para entrar na {9B6BFF}Astra PVP/RP{FFFFFF}:", PName(playerid));
        ShowPlayerDialog(playerid, D_LOGIN, DIALOG_STYLE_PASSWORD, "Astra - Login", s, "Entrar", "Sair");
    }
    else
    {
        format(s, sizeof s, "{FFFFFF}Ola, {9B6BFF}%s{FFFFFF}! Esta conta ainda nao existe.\nCrie uma senha (minimo %d caracteres):", PName(playerid), MIN_SENHA);
        ShowPlayerDialog(playerid, D_REGISTRO, DIALOG_STYLE_PASSWORD, "Astra - Registro", s, "Registrar", "Sair");
    }
    return 1;
}

stock SalvarConta(playerid)
{
    if(!IsPlayerConnected(playerid) || !Player[playerid][pLogado]) return 0;
    new arq[64];
    ArquivoConta(playerid, arq);
    if(!dini_Exists(arq)) return 0;
    dini_IntSet(arq, "Dinheiro", Player[playerid][pDinheiro]);
    dini_IntSet(arq, "Banco", Player[playerid][pBanco]);
    dini_IntSet(arq, "Level", Player[playerid][pLevel]);
    dini_IntSet(arq, "Skin", Player[playerid][pSkin]);
    dini_IntSet(arq, "Admin", Player[playerid][pAdmin]);
    dini_IntSet(arq, "Kills", Player[playerid][pKills]);
    dini_IntSet(arq, "Deaths", Player[playerid][pDeaths]);
    dini_IntSet(arq, "Emprego", Player[playerid][pEmprego]);
    dini_IntSet(arq, "Faccao", Player[playerid][pFaccao]);
    dini_IntSet(arq, "Cargo", Player[playerid][pCargo]);
    dini_IntSet(arq, "Casa", Player[playerid][pCasa]);
    dini_IntSet(arq, "Mute", Player[playerid][pMute]);
    dini_IntSet(arq, "Minutos", Player[playerid][pMinutos]);
    dini_IntSet(arq, "MelhorStreak", Player[playerid][pMelhorStreak]);
    return 1;
}

stock CriarConta(playerid, const senha[])
{
    new arq[64];
    ArquivoConta(playerid, arq);
    GerarSalt(PSalt[playerid]);
    SHA256_PassHash(senha, PSalt[playerid], PHash[playerid], sizeof(PHash[]));

    dini_Create(arq);
    dini_Set(arq, "Hash", PHash[playerid]);
    dini_Set(arq, "Salt", PSalt[playerid]);

    Player[playerid][pDinheiro] = 5000;
    Player[playerid][pBanco] = 10000;
    Player[playerid][pLevel] = 1;
    Player[playerid][pSkin] = 29;
    Player[playerid][pCasa] = -1;
    Player[playerid][pLogado] = true;
    SalvarConta(playerid);
    return 1;
}

stock CarregarConta(playerid)
{
    new arq[64];
    ArquivoConta(playerid, arq);
    format(PHash[playerid], sizeof(PHash[]), "%s", dini_Get(arq, "Hash"));
    format(PSalt[playerid], sizeof(PSalt[]), "%s", dini_Get(arq, "Salt"));
    Player[playerid][pDinheiro]     = dini_Int(arq, "Dinheiro");
    Player[playerid][pBanco]        = dini_Int(arq, "Banco");
    Player[playerid][pLevel]        = dini_Int(arq, "Level");
    Player[playerid][pSkin]         = dini_Int(arq, "Skin");
    Player[playerid][pAdmin]        = dini_Int(arq, "Admin");
    Player[playerid][pKills]        = dini_Int(arq, "Kills");
    Player[playerid][pDeaths]       = dini_Int(arq, "Deaths");
    Player[playerid][pEmprego]      = dini_Int(arq, "Emprego");
    Player[playerid][pFaccao]       = dini_Int(arq, "Faccao");
    Player[playerid][pCargo]        = dini_Int(arq, "Cargo");
    Player[playerid][pCasa]         = dini_Isset(arq, "Casa") ? dini_Int(arq, "Casa") : -1;
    Player[playerid][pMute]         = dini_Int(arq, "Mute");
    Player[playerid][pMinutos]      = dini_Int(arq, "Minutos");
    Player[playerid][pMelhorStreak] = dini_Int(arq, "MelhorStreak");
    if(Player[playerid][pLevel] < 1) Player[playerid][pLevel] = 1;
    if(Player[playerid][pSkin] < 0 || Player[playerid][pSkin] > 311) Player[playerid][pSkin] = 29;
    return 1;
}

stock MostrarMenuModo(playerid)
{
    ShowPlayerDialog(playerid, D_MODO, DIALOG_STYLE_TABLIST_HEADERS, "Astra - Escolha seu modo",
        "Modo\tDescricao\n{FF6B6B}PVP\t{FFFFFF}Arenas, kills, ranking e streaks\n{6BCBFF}RP\t{FFFFFF}Roleplay: empregos, casas, banco e faccoes",
        "Entrar", "");
    return 1;
}
