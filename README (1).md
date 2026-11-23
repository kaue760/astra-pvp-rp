# Astra PVP/RP v1.0

Gamemode SA-MP 0.3.7 em Pawn com dois modos no mesmo servidor, escolhidos apos o login (`/modo`):

- **PVP**: lobby seguro, 9 arenas (Desert, Mata-Mata, Ammu, PT2, Predio, M4 Cidade, M4 Delegacia, Combat Shotgun, Sniper), killfeed, streaks com bonus, `/rank` e `/stats`.
- **RP**: banco, empregos com salario por hora, casas persistentes, faccoes (Policia/Medicos/Mecanicos) com `/duty` e `/f`, chat local (`/me /do /b /g /s`), `/motor` e `/luzes`.
- **Admin** (niveis 1-5): `/a /kick /ban /unban /mute /ir /trazer /cv /dv /setskin /darmoney /setlevel /setadm /clima /hora /aviso /tv /criarcasa /darfaccao`.

## Instalar
1. Copie `gamemodes/`, `include/`, `plugins/` e `scriptfiles/` para a pasta do servidor.
2. Compile `gamemodes/astra.pwn` com o `pawncc` (includes ja vem em `include/`; `a_samp` e do seu servidor).
3. Edite `server.cfg` e troque `rcon_password`.
4. Inicie o servidor. Entre, registre-se e rode no console: `setadm <id> 5` para virar dono.
5. Em RP, use `/criarcasa <preco>` onde quiser colocar casas a venda.

## Estrutura
`gamemodes/astra.pwn` (callbacks) e `gamemodes/astra/`: `core.inc`, `accounts.inc`, `pvp.inc`, `rp.inc`, `admin.inc`. Arenas, armas e spawns ficam no topo de `pvp.inc`.

## Seguranca (importante)
- Senhas agora usam **SHA256 + salt**. O GM antigo guardava a senha em texto puro (`Senha=`), entao **as contas antigas nao foram migradas**: todos precisam se registrar de novo.
- O pacote Santa Group trazia um **token de bot do Discord** e uma senha RCON antigos. Nada disso foi copiado. **Revogue o token no Discord Developer Portal** e troque qualquer senha RCON que ja foi usada.
- Os arquivos `scriptfiles/Contas` e `ID` originais (dados de jogadores) tambem nao foram incluidos.

## Limitacoes
- **Nao foi compilado nem testado em servidor real** (sem compilador Pawn no ambiente onde foi escrito). Se o `pawncc` apontar erros, me mande a saida e eu corrijo.
- Nao foram portados: os ~500 comandos e os mapas gigantes do Santa Group (122 mil linhas), os textdraws do PVP, voz (sampvoice) e mapa do Rio Rise. A Astra e uma base nova e limpa para voce ir adicionando.
- Interiores/armas das arenas foram estimados; ajuste as tabelas em `pvp.inc`.

## Creditos
Base de arenas/spawns: *GM PVP by Abelha*. Ideias de sistemas RP: *Santa Group*. Includes: Dini, zcmd (ZeeX), sscanf2 (Y_Less), streamer (Incognito). Respeite as licencas e os creditos dos autores originais ao publicar.
