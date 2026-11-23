// ============================================================================
//  ASTRA PVP/RP  -  core.inc
//  Definicoes, cores, dialogs e dados do jogador.
// ============================================================================
#define SERVER_NOME     "Astra PVP/RP"
#define SERVER_VERSAO   "1.0"

#define PASTA_CONTAS    "Astra/Contas/%s.ini"
#define PASTA_CASAS     "Astra/Casas/%d.ini"
#define PASTA_BANS      "Astra/Bans/%s.ini"

#define MAX_CASAS       100
#define MAX_SENHA_TENT  3
#define MIN_SENHA       6
#define RAIO_CHAT_LOCAL 20.0

#define COR_BRANCO      0xFFFFFFFF
#define COR_ASTRA       0x9B6BFFFF
#define COR_ERRO        0xFF5C5CFF
#define COR_OK          0x5CFF8AFF
#define COR_ADM         0xFFD24DFF
#define COR_RP          0xC8A8E8FF
#define COR_INFO        0x8FD3FFFF

#define SCM             SendClientMessage

// Mundos virtuais
#define VW_RP           0
#define VW_LOBBY_PVP    1
#define VW_ARENA_BASE   100

// Modos de jogo
enum { MODO_NENHUM, MODO_PVP, MODO_RP };

// Dialogs
enum
{
    D_NULL = 1000,
    D_REGISTRO,
    D_LOGIN,
    D_MODO,
    D_ARENAS,
    D_EMPREGO,
    D_BANCO_DEPOSITO,
    D_BANCO_SAQUE
};

// Empregos RP
enum { EMP_DESEMPREGADO, EMP_TAXISTA, EMP_LIXEIRO, EMP_ENTREGADOR, EMP_MECANICO };
new const NomeEmprego[][] = {"Desempregado", "Taxista", "Lixeiro", "Entregador", "Mecanico"};
new const SalarioEmprego[] = {0, 900, 800, 850, 950};

// Faccoes RP
enum { FAC_NENHUMA, FAC_POLICIA, FAC_MEDICOS, FAC_MECANICOS };
new const NomeFaccao[][] = {"Civil", "Policia", "Medicos", "Mecanicos"};

// Dados do jogador
enum pInfo
{
    bool:pLogado,
    pDinheiro,
    pBanco,
    pLevel,
    pSkin,
    pAdmin,
    pKills,
    pDeaths,
    pEmprego,
    pFaccao,
    pCargo,
    pCasa,
    pMute,
    pMinutos,
    pModo,
    pArena,
    pStreak,
    pMelhorStreak,
    pTentativas,
    bool:pDuty
};
new Player[MAX_PLAYERS][pInfo];
new PHash[MAX_PLAYERS][65];
new PSalt[MAX_PLAYERS][17];

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
stock PName(playerid)
{
    new n[MAX_PLAYER_NAME];
    GetPlayerName(playerid, n, sizeof n);
    return n;
}

stock Erro(playerid, const msg[])
{
    new s[144];
    format(s, sizeof s, "[Astra] {FFFFFF}%s", msg);
    return SCM(playerid, COR_ERRO, s);
}

stock Info(playerid, const msg[])
{
    new s[144];
    format(s, sizeof s, "[Astra] {FFFFFF}%s", msg);
    return SCM(playerid, COR_ASTRA, s);
}

stock SendAll(color, const msg[])
{
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i) && Player[i][pLogado]) SCM(i, color, msg);
    return 1;
}

stock SendAdmins(const msg[])
{
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i) && Player[i][pAdmin] > 0) SCM(i, COR_ADM, msg);
    return 1;
}

stock IsAdmin(playerid, nivel)
{
    if(Player[playerid][pAdmin] >= nivel) return 1;
    Erro(playerid, "Voce nao tem permissao para usar este comando.");
    return 0;
}

stock IsRP(playerid)  { return Player[playerid][pLogado] && Player[playerid][pModo] == MODO_RP; }
stock IsPVP(playerid) { return Player[playerid][pLogado] && Player[playerid][pModo] == MODO_PVP; }

stock GetPlayerIdByName(const nome[])
{
    new n[MAX_PLAYER_NAME];
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i)) continue;
        GetPlayerName(i, n, sizeof n);
        if(!strcmp(n, nome, true)) return i;
    }
    return INVALID_PLAYER_ID;
}

forward KickDelay(playerid);
public KickDelay(playerid) { Kick(playerid); return 1; }

stock KickComDelay(playerid) { SetTimerEx("KickDelay", 800, false, "i", playerid); return 1; }

stock FormatarDinheiro(valor, out[], len = sizeof out)
{
    new s[16], r[24], c, j;
    valstr(s, valor < 0 ? -valor : valor);
    new l = strlen(s);
    for(new i = l - 1; i >= 0; i--)
    {
        if(c && c % 3 == 0) { r[j++] = '.'; }
        r[j++] = s[i];
        c++;
    }
    if(valor < 0) r[j++] = '-';
    r[j] = 0;
    new k = 0;
    out[0] = '$';
    for(new i = j - 1; i >= 0 && k < len - 2; i--) out[++k] = r[i];
    out[k + 1] = 0;
    return 1;
}

stock DarDinheiro(playerid, valor)
{
    Player[playerid][pDinheiro] += valor;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, Player[playerid][pDinheiro]);
    return 1;
}

new bool:PSpawnado[MAX_PLAYERS];
