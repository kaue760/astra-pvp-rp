// ============================================================================
//   ASTRA PVP/RP  v1.0
//   Gamemode SA-MP 0.3.7 em Pawn  |  PVP (arenas) + RP (economia, casas, faccoes)
//
//   Base: modulos e dados reaproveitados de "GM PVP by Abelha" e do pacote
//   "Santa Group". Reescrito de forma modular, com senhas em SHA256+salt e
//   sem tokens/senhas embutidos. Veja o README.md.
// ============================================================================
#pragma tabsize 4

#include <a_samp>
#include <zcmd>
#include <sscanf2>
#include <streamer>
#include <Dini>

#include "astra/core.inc"
#include "astra/accounts.inc"
#include "astra/pvp.inc"
#include "astra/rp.inc"
#include "astra/admin.inc"

// ---------------------------------------------------------------------------
// Modo de jogo (PVP / RP)
// ---------------------------------------------------------------------------
stock IniciarModo(playerid, modo)
{
    Player[playerid][pModo] = modo;
    Player[playerid][pArena] = -1;
    Player[playerid][pDuty] = false;
    SetPlayerSkin(playerid, Player[playerid][pSkin]);
    if(modo == MODO_PVP) SetPlayerColor(playerid, 0xFF6B6BFF);
    else SetPlayerColor(playerid, 0xFFFFFFFF);

    if(!PSpawnado[playerid])
    {
        SetSpawnInfo(playerid, 0, Player[playerid][pSkin], LOBBY_X, LOBBY_Y, LOBBY_Z, LOBBY_R, 0, 0, 0, 0, 0, 0);
        SpawnPlayer(playerid);
    }
    else
    {
        if(modo == MODO_PVP) PVP_Lobby(playerid);
        else RP_Spawn(playerid);
    }
    return 1;
}

CMD:modo(playerid, params[])
{
    if(!Player[playerid][pLogado]) return 1;
    return MostrarMenuModo(playerid);
}

CMD:ajuda(playerid, params[])
{
    SCM(playerid, COR_ASTRA, "===== Astra PVP/RP - Ajuda =====");
    SCM(playerid, COR_BRANCO, "Geral: /modo /stats /rank /ajuda");
    SCM(playerid, COR_BRANCO, "PVP: /arenas /lobby");
    SCM(playerid, COR_BRANCO, "RP: /me /do /b /g /s /saldo /depositar /sacar /transferir /emprego /demitir /comprarcasa /vendercasa /duty /f /motor /luzes");
    if(Player[playerid][pAdmin] > 0) SCM(playerid, COR_ADM, "ADM: /a /kick /ban /unban /mute /ir /trazer /cv /dv /setskin /darmoney /setlevel /setadm /clima /hora /aviso /tv /tvoff /criarcasa /darfaccao");
    return 1;
}

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------
main()
{
    printf("\n  Astra PVP/RP v%s iniciada.\n", SERVER_VERSAO);
}

public OnGameModeInit()
{
    SetGameModeText("Astra PVP/RP");
    AddPlayerClass(29, LOBBY_X, LOBBY_Y, LOBBY_Z, LOBBY_R, 0, 0, 0, 0, 0, 0);
    UsePlayerPedAnims();
    DisableInteriorEnterExits();
    EnableStuntBonusForAll(0);
    ManualVehicleEngineAndLights();
    ShowPlayerMarkers(1);

    Casa_CarregarTodas();

    // Alguns veiculos de ambientacao perto do spawn RP (Prefeitura de LS)
    CreateVehicle(420, 1470.0, -1755.0, 13.4, 0.0, 6, 1, 300);   // Taxi
    CreateVehicle(420, 1474.0, -1755.0, 13.4, 0.0, 6, 1, 300);   // Taxi
    CreateVehicle(408, 1490.0, -1755.0, 13.6, 0.0, 1, 1, 300);   // Caminhao de lixo
    CreateVehicle(482, 1494.0, -1755.0, 13.6, 0.0, 1, 1, 300);   // Van de entrega

    SetTimer("RP_TimerMinuto", 60000, true);
    print("[Astra] Gamemode carregada.");
    return 1;
}

public OnGameModeExit()
{
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i)) SalvarConta(i);
    return 1;
}

forward MostrarLoginDelay(playerid);
public MostrarLoginDelay(playerid)
{
    if(IsPlayerConnected(playerid) && !Player[playerid][pLogado]) MostrarLoginOuRegistro(playerid);
    return 1;
}

public OnPlayerConnect(playerid)
{
    ResetarJogador(playerid);
    PSpawnado[playerid] = false;

    new arq[64], s[144];
    format(arq, sizeof arq, PASTA_BANS, PName(playerid));
    if(dini_Exists(arq))
    {
        format(s, sizeof s, "Voce esta banido. Motivo: %s", dini_Get(arq, "Motivo"));
        Erro(playerid, s);
        KickComDelay(playerid);
        return 1;
    }

    format(s, sizeof s, "[+] %s entrou no servidor.", PName(playerid));
    SendAll(0x888888FF, s);
    SCM(playerid, COR_ASTRA, "Bem-vindo a {FFFFFF}Astra PVP/RP{9B6BFF}! Faca login para continuar.");
    SetTimerEx("MostrarLoginDelay", 700, false, "i", playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    SalvarConta(playerid);
    if(Player[playerid][pLogado])
    {
        new s[96];
        format(s, sizeof s, "[-] %s saiu do servidor.", PName(playerid));
        SendAll(0x888888FF, s);
    }
    ResetarJogador(playerid);
    PSpawnado[playerid] = false;
    return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    SetPlayerPos(playerid, LOBBY_X, LOBBY_Y, LOBBY_Z);
    SetPlayerCameraPos(playerid, 395.0, -2060.0, 14.0);
    SetPlayerCameraLookAt(playerid, LOBBY_X, LOBBY_Y, LOBBY_Z);
    return 1;
}

public OnPlayerRequestSpawn(playerid)
{
    return Player[playerid][pLogado] ? 1 : 0;
}

public OnPlayerSpawn(playerid)
{
    if(!Player[playerid][pLogado] || Player[playerid][pModo] == MODO_NENHUM) return 1;
    PSpawnado[playerid] = true;
    SetPlayerSkin(playerid, Player[playerid][pSkin]);
    if(Player[playerid][pModo] == MODO_PVP) PVP_Respawn(playerid);
    else RP_Spawn(playerid);
    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    PSpawnado[playerid] = false;
    if(Player[playerid][pModo] == MODO_PVP) PVP_Morte(playerid, killerid, reason);
    return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
    // Lobby PVP e uma zona segura
    if(IsPVP(playerid) && Player[playerid][pArena] == -1) SetPlayerHealth(playerid, 100.0);
    return 1;
}

public OnPlayerText(playerid, text[])
{
    if(!Player[playerid][pLogado]) return 0;
    if(Player[playerid][pMute] > 0)
    {
        Erro(playerid, "Voce esta mutado.");
        return 0;
    }
    new s[180];
    if(Player[playerid][pModo] == MODO_RP)
    {
        format(s, sizeof s, "%s diz: %s", PName(playerid), text);
        ChatLocal(playerid, COR_BRANCO, s);
    }
    else
    {
        format(s, sizeof s, "{FF6B6B}[PVP] {FFFFFF}%s (%d): %s", PName(playerid), playerid, text);
        for(new i = 0; i < MAX_PLAYERS; i++)
            if(IsPlayerConnected(i) && IsPVP(i)) SCM(i, COR_BRANCO, s);
    }
    return 0;
}

public OnPlayerCommandReceived(playerid, cmdtext[])
{
    if(!Player[playerid][pLogado]) { Erro(playerid, "Faca login primeiro."); return 0; }
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
    if(!success) Erro(playerid, "Comando desconhecido. Use /ajuda.");
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case D_REGISTRO:
        {
            if(!response) return KickComDelay(playerid);
            if(strlen(inputtext) < MIN_SENHA)
            {
                Erro(playerid, "A senha precisa de no minimo 6 caracteres.");
                return MostrarLoginOuRegistro(playerid);
            }
            CriarConta(playerid, inputtext);
            ResetPlayerMoney(playerid);
            GivePlayerMoney(playerid, Player[playerid][pDinheiro]);
            Info(playerid, "Conta criada com sucesso!");
            return MostrarMenuModo(playerid);
        }
        case D_LOGIN:
        {
            if(!response) return KickComDelay(playerid);
            new arq[64], hash[65];
            ArquivoConta(playerid, arq);
            format(PHash[playerid], sizeof(PHash[]), "%s", dini_Get(arq, "Hash"));
            format(PSalt[playerid], sizeof(PSalt[]), "%s", dini_Get(arq, "Salt"));
            SHA256_PassHash(inputtext, PSalt[playerid], hash, sizeof hash);
            if(strcmp(hash, PHash[playerid], false) != 0)
            {
                Player[playerid][pTentativas]++;
                if(Player[playerid][pTentativas] >= MAX_SENHA_TENT)
                {
                    Erro(playerid, "Muitas tentativas incorretas.");
                    return KickComDelay(playerid);
                }
                Erro(playerid, "Senha incorreta.");
                return MostrarLoginOuRegistro(playerid);
            }
            CarregarConta(playerid);
            Player[playerid][pLogado] = true;
            ResetPlayerMoney(playerid);
            GivePlayerMoney(playerid, Player[playerid][pDinheiro]);
            SetPlayerScore(playerid, Player[playerid][pLevel]);
            Info(playerid, "Login realizado com sucesso!");
            return MostrarMenuModo(playerid);
        }
        case D_MODO:
        {
            if(!Player[playerid][pLogado]) return 1;
            // Sem botao de cancelar: ambas as respostas aceitam a selecao
            return IniciarModo(playerid, listitem == 0 ? MODO_PVP : MODO_RP);
        }
        case D_ARENAS:
        {
            if(response && IsPVP(playerid)) PVP_EntrarArena(playerid, listitem);
            return 1;
        }
    }
    RP_Dialog(playerid, dialogid, response, listitem);
    return 1;
}

public OnRconCommand(cmd[])
{
    // Definir o primeiro dono: rcon setadm <id> <nivel>
    new nome[16], id, nivel;
    if(!sscanf(cmd, "s[16]dd", nome, id, nivel) && !strcmp(nome, "setadm", true))
    {
        if(id >= 0 && id < MAX_PLAYERS && IsPlayerConnected(id) && Player[id][pLogado] && nivel >= 0 && nivel <= 5)
        {
            Player[id][pAdmin] = nivel;
            SalvarConta(id);
            printf("[Astra] %s agora tem admin nivel %d.", PName(id), nivel);
        }
        else print("[Astra] Uso: setadm <id logado> <0-5>");
        return 1;
    }
    return 0;
}
