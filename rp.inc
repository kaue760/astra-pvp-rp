// ============================================================================
//  ASTRA PVP/RP  -  rp.inc
//  Banco, empregos, salario, casas, faccoes e chat de roleplay.
// ============================================================================
#define RP_X    1481.0
#define RP_Y    -1747.5
#define RP_Z    13.55
#define RP_R    0.0

enum cInfo
{
    bool:cExiste,
    cDono[MAX_PLAYER_NAME],
    cPreco,
    Float:cX,
    Float:cY,
    Float:cZ,
    cPickup
};
new Casa[MAX_CASAS][cInfo];

// ---------------------------------------------------------------------------
// Spawn RP
// ---------------------------------------------------------------------------
stock RP_Spawn(playerid)
{
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, VW_RP);
    new c = Player[playerid][pCasa];
    if(c >= 0 && c < MAX_CASAS && Casa[c][cExiste])
        SetPlayerPos(playerid, Casa[c][cX], Casa[c][cY], Casa[c][cZ]);
    else
    {
        SetPlayerPos(playerid, RP_X, RP_Y, RP_Z);
        SetPlayerFacingAngle(playerid, RP_R);
    }
    SetPlayerHealth(playerid, 100.0);
    ResetPlayerWeapons(playerid);
    SetCameraBehindPlayer(playerid);
    return 1;
}

// ---------------------------------------------------------------------------
// Casas (persistidas em scriptfiles/Astra/Casas/<id>.ini)
// ---------------------------------------------------------------------------
stock Casa_Salvar(id)
{
    new arq[48];
    format(arq, sizeof arq, PASTA_CASAS, id);
    if(!dini_Exists(arq)) dini_Create(arq);
    dini_Set(arq, "Dono", Casa[id][cDono]);
    dini_IntSet(arq, "Preco", Casa[id][cPreco]);
    dini_FloatSet(arq, "X", Casa[id][cX]);
    dini_FloatSet(arq, "Y", Casa[id][cY]);
    dini_FloatSet(arq, "Z", Casa[id][cZ]);
    return 1;
}

stock Casa_Pickup(id)
{
    if(Casa[id][cPickup]) DestroyDynamicPickup(Casa[id][cPickup]);
    // 1273 = casa verde (a venda), 19522 = casa azul (ocupada)
    Casa[id][cPickup] = CreateDynamicPickup(strlen(Casa[id][cDono]) ? 19522 : 1273, 1,
        Casa[id][cX], Casa[id][cY], Casa[id][cZ], VW_RP);
    return 1;
}

stock Casa_CarregarTodas()
{
    new arq[48], n;
    for(new i = 0; i < MAX_CASAS; i++)
    {
        format(arq, sizeof arq, PASTA_CASAS, i);
        if(!dini_Exists(arq)) continue;
        Casa[i][cExiste] = true;
        format(Casa[i][cDono], MAX_PLAYER_NAME, "%s", dini_Get(arq, "Dono"));
        Casa[i][cPreco] = dini_Int(arq, "Preco");
        Casa[i][cX] = dini_Float(arq, "X");
        Casa[i][cY] = dini_Float(arq, "Y");
        Casa[i][cZ] = dini_Float(arq, "Z");
        Casa_Pickup(i);
        n++;
    }
    printf("[Astra] %d casas carregadas.", n);
    return n;
}

stock Casa_Proxima(playerid)
{
    for(new i = 0; i < MAX_CASAS; i++)
        if(Casa[i][cExiste] && IsPlayerInRangeOfPoint(playerid, 3.0, Casa[i][cX], Casa[i][cY], Casa[i][cZ])) return i;
    return -1;
}

CMD:criarcasa(playerid, params[])
{
    if(!IsAdmin(playerid, 4)) return 1;
    new preco;
    if(sscanf(params, "d", preco) || preco < 1000) return Info(playerid, "Uso: /criarcasa [preco >= 1000]");
    new id = -1;
    for(new i = 0; i < MAX_CASAS; i++) if(!Casa[i][cExiste]) { id = i; break; }
    if(id == -1) return Erro(playerid, "Limite de casas atingido.");
    Casa[id][cExiste] = true;
    Casa[id][cDono][0] = 0;
    Casa[id][cPreco] = preco;
    GetPlayerPos(playerid, Casa[id][cX], Casa[id][cY], Casa[id][cZ]);
    Casa_Salvar(id);
    Casa_Pickup(id);
    new s[64];
    format(s, sizeof s, "Casa #%d criada.", id);
    return Info(playerid, s);
}

CMD:comprarcasa(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new id = Casa_Proxima(playerid);
    if(id == -1) return Erro(playerid, "Voce nao esta perto de uma casa.");
    if(strlen(Casa[id][cDono])) return Erro(playerid, "Esta casa ja tem dono.");
    if(Player[playerid][pCasa] != -1) return Erro(playerid, "Voce ja possui uma casa.");
    if(Player[playerid][pDinheiro] < Casa[id][cPreco]) return Erro(playerid, "Dinheiro insuficiente.");
    DarDinheiro(playerid, -Casa[id][cPreco]);
    format(Casa[id][cDono], MAX_PLAYER_NAME, "%s", PName(playerid));
    Player[playerid][pCasa] = id;
    Casa_Salvar(id);
    Casa_Pickup(id);
    SalvarConta(playerid);
    return Info(playerid, "Parabens! Voce comprou a casa. Voce nascera nela ao entrar.");
}

CMD:vendercasa(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new id = Player[playerid][pCasa];
    if(id < 0) return Erro(playerid, "Voce nao tem casa.");
    DarDinheiro(playerid, Casa[id][cPreco] * 70 / 100);
    Casa[id][cDono][0] = 0;
    Casa_Salvar(id);
    Casa_Pickup(id);
    Player[playerid][pCasa] = -1;
    SalvarConta(playerid);
    return Info(playerid, "Casa vendida por 70% do valor.");
}

// ---------------------------------------------------------------------------
// Banco
// ---------------------------------------------------------------------------
CMD:saldo(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new a[24], b[24], s[96];
    FormatarDinheiro(Player[playerid][pDinheiro], a);
    FormatarDinheiro(Player[playerid][pBanco], b);
    format(s, sizeof s, "Carteira: %s | Banco: %s", a, b);
    return Info(playerid, s);
}

CMD:depositar(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new v;
    if(sscanf(params, "d", v) || v < 1) return Info(playerid, "Uso: /depositar [valor]");
    if(v > Player[playerid][pDinheiro]) return Erro(playerid, "Voce nao tem tudo isso na carteira.");
    DarDinheiro(playerid, -v);
    Player[playerid][pBanco] += v;
    return Info(playerid, "Deposito realizado.");
}

CMD:sacar(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new v;
    if(sscanf(params, "d", v) || v < 1) return Info(playerid, "Uso: /sacar [valor]");
    if(v > Player[playerid][pBanco]) return Erro(playerid, "Saldo bancario insuficiente.");
    Player[playerid][pBanco] -= v;
    DarDinheiro(playerid, v);
    return Info(playerid, "Saque realizado.");
}

CMD:transferir(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new id, v, s[96];
    if(sscanf(params, "ud", id, v) || v < 1) return Info(playerid, "Uso: /transferir [id/nome] [valor]");
    if(id == INVALID_PLAYER_ID || id == playerid || !IsRP(id)) return Erro(playerid, "Jogador invalido.");
    if(v > Player[playerid][pBanco]) return Erro(playerid, "Saldo bancario insuficiente.");
    Player[playerid][pBanco] -= v;
    Player[id][pBanco] += v;
    format(s, sizeof s, "Voce transferiu $%d para %s.", v, PName(id));
    Info(playerid, s);
    format(s, sizeof s, "%s transferiu $%d para sua conta.", PName(playerid), v);
    return Info(id, s);
}

// ---------------------------------------------------------------------------
// Empregos e salario (pago a cada hora jogada em RP)
// ---------------------------------------------------------------------------
CMD:emprego(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    new lista[256], linha[64];
    for(new i = 1; i < sizeof(NomeEmprego); i++)
    {
        format(linha, sizeof linha, "%s\t$%d/h\n", NomeEmprego[i], SalarioEmprego[i]);
        strcat(lista, linha);
    }
    ShowPlayerDialog(playerid, D_EMPREGO, DIALOG_STYLE_TABLIST, "Astra RP - Empregos", lista, "Assinar", "Fechar");
    return 1;
}

CMD:demitir(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    Player[playerid][pEmprego] = EMP_DESEMPREGADO;
    return Info(playerid, "Voce pediu demissao.");
}

// ---------------------------------------------------------------------------
// Faccoes
// ---------------------------------------------------------------------------
CMD:darfaccao(playerid, params[])
{
    if(!IsAdmin(playerid, 3)) return 1;
    new id, fac, cargo, s[96];
    if(sscanf(params, "udd", id, fac, cargo) || fac < 0 || fac >= sizeof(NomeFaccao))
        return Info(playerid, "Uso: /darfaccao [id] [0-3 Civil/Policia/Medicos/Mecanicos] [cargo 0-5]");
    if(id == INVALID_PLAYER_ID || !IsRP(id)) return Erro(playerid, "Jogador invalido (precisa estar em RP).");
    Player[id][pFaccao] = fac;
    Player[id][pCargo] = cargo;
    format(s, sizeof s, "Voce agora e da faccao %s (cargo %d).", NomeFaccao[fac], cargo);
    Info(id, s);
    return Info(playerid, "Faccao atribuida.");
}

CMD:duty(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(Player[playerid][pFaccao] == FAC_NENHUMA) return Erro(playerid, "Voce nao pertence a nenhuma faccao.");
    Player[playerid][pDuty] = !Player[playerid][pDuty];
    if(Player[playerid][pDuty])
    {
        if(Player[playerid][pFaccao] == FAC_POLICIA) { SetPlayerSkin(playerid, 280); GivePlayerWeapon(playerid, 24, 200); SetPlayerArmour(playerid, 100.0); }
        else if(Player[playerid][pFaccao] == FAC_MEDICOS) SetPlayerSkin(playerid, 275);
        else SetPlayerSkin(playerid, 50);
        return Info(playerid, "Voce entrou em servico.");
    }
    SetPlayerSkin(playerid, Player[playerid][pSkin]);
    ResetPlayerWeapons(playerid);
    SetPlayerArmour(playerid, 0.0);
    return Info(playerid, "Voce saiu de servico.");
}

// Chat de faccao
CMD:f(playerid, params[])
{
    if(!IsRP(playerid) || Player[playerid][pFaccao] == FAC_NENHUMA) return Erro(playerid, "Voce nao esta em uma faccao.");
    if(isnull(params)) return Info(playerid, "Uso: /f [mensagem]");
    new s[160];
    format(s, sizeof s, "[%s] %s: %s", NomeFaccao[Player[playerid][pFaccao]], PName(playerid), params);
    for(new i = 0; i < MAX_PLAYERS; i++)
        if(IsPlayerConnected(i) && IsRP(i) && Player[i][pFaccao] == Player[playerid][pFaccao]) SCM(i, 0x6BCBFFFF, s);
    return 1;
}

// ---------------------------------------------------------------------------
// Chat RP local
// ---------------------------------------------------------------------------
stock ChatLocal(playerid, color, const msg[], Float:raio = RAIO_CHAT_LOCAL)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i) || !IsRP(i)) continue;
        if(GetPlayerVirtualWorld(i) != GetPlayerVirtualWorld(playerid)) continue;
        if(IsPlayerInRangeOfPoint(i, raio, x, y, z)) SCM(i, color, msg);
    }
    return 1;
}

CMD:me(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(isnull(params)) return Info(playerid, "Uso: /me [acao]");
    new s[160];
    format(s, sizeof s, "* %s %s", PName(playerid), params);
    return ChatLocal(playerid, COR_RP, s);
}

CMD:do(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(isnull(params)) return Info(playerid, "Uso: /do [descricao]");
    new s[160];
    format(s, sizeof s, "* %s (( %s ))", params, PName(playerid));
    return ChatLocal(playerid, COR_RP, s);
}

CMD:b(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(isnull(params)) return Info(playerid, "Uso: /b [mensagem OOC]");
    new s[160];
    format(s, sizeof s, "(( %s [%d]: %s ))", PName(playerid), playerid, params);
    return ChatLocal(playerid, 0xAAAAAAFF, s);
}

CMD:g(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(isnull(params)) return Info(playerid, "Uso: /g [mensagem]");
    new s[160];
    format(s, sizeof s, "%s grita: %s!!", PName(playerid), params);
    return ChatLocal(playerid, COR_BRANCO, s, 45.0);
}

CMD:s(playerid, params[])
{
    if(!IsRP(playerid)) return Erro(playerid, "Comando do modo RP.");
    if(isnull(params)) return Info(playerid, "Uso: /s [mensagem]");
    new s[160];
    format(s, sizeof s, "%s sussurra: %s", PName(playerid), params);
    return ChatLocal(playerid, 0xBBBBBBFF, s, 3.0);
}

// ---------------------------------------------------------------------------
// Veiculos (qualquer veiculo que o jogador esteja dirigindo)
// ---------------------------------------------------------------------------
CMD:motor(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return Erro(playerid, "Voce precisa estar dirigindo.");
    new v = GetPlayerVehicleID(playerid), eng, lights, alarm, doors, bonnet, boot, obj;
    GetVehicleParamsEx(v, eng, lights, alarm, doors, bonnet, boot, obj);
    eng = (eng == 1) ? 0 : 1;
    SetVehicleParamsEx(v, eng, lights, alarm, doors, bonnet, boot, obj);
    if(eng) return Info(playerid, "Motor ligado.");
    return Info(playerid, "Motor desligado.");
}

CMD:luzes(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return Erro(playerid, "Voce precisa estar dirigindo.");
    new v = GetPlayerVehicleID(playerid), eng, lights, alarm, doors, bonnet, boot, obj;
    GetVehicleParamsEx(v, eng, lights, alarm, doors, bonnet, boot, obj);
    lights = (lights == 1) ? 0 : 1;
    SetVehicleParamsEx(v, eng, lights, alarm, doors, bonnet, boot, obj);
    if(lights) return Info(playerid, "Luzes ligadas.");
    return Info(playerid, "Luzes desligadas.");
}

// ---------------------------------------------------------------------------
// Timer de 1 minuto: tempo de jogo e salario por hora
// ---------------------------------------------------------------------------
forward RP_TimerMinuto();
public RP_TimerMinuto()
{
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i) || !Player[i][pLogado]) continue;
        Player[i][pMinutos]++;
        if(IsRP(i) && Player[i][pMinutos] % 60 == 0)
        {
            new sal = SalarioEmprego[Player[i][pEmprego]], s[96];
            if(sal > 0)
            {
                Player[i][pBanco] += sal;
                format(s, sizeof s, "Salario de $%d depositado no banco (%s).", sal, NomeEmprego[Player[i][pEmprego]]);
                Info(i, s);
            }
        }
        if(Player[i][pMinutos] % 5 == 0) SalvarConta(i);
    }
    return 1;
}

stock RP_Dialog(playerid, dialogid, response, listitem)
{
    if(dialogid == D_EMPREGO && response)
    {
        Player[playerid][pEmprego] = listitem + 1;
        new s[96];
        format(s, sizeof s, "Voce agora trabalha como %s.", NomeEmprego[listitem + 1]);
        Info(playerid, s);
        return 1;
    }
    return 0;
}
