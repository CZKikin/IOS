# IOS
Projects to IOS

# Project 1
## Popis úlohy
Cílem úlohy je vytvořit skript pro analýzu záznamu systému pro obchodování na burze. Skript bude filtrovat záznamy a poskytovat statistiky podle zadání úživatele.

## Zjednodušený úvod do problematiky
Na burze se obchoduje s cennými papíry (např. akcie společností, dluhopisy), komoditami (např. ropa, zelí) apod. Každý obchodovaný artikl má jednoznačný identifikátor, tzv. ticker (např. akcie firmy Intel mají na burze NASDAQ ticker INTC, bitcoin může mít přiřazený ticker BTC). Cena artiklů se mění v čase. Obchodník na burze vstupuje do pozic, buď tak, že koupí artikl a očekává, že jeho cena poroste, aby jej pak prodal za vyšší cenu (tzv. dlouhá pozice), nebo že artikl prodá a očekává, že jeho cena klesne, aby jej poté mohl koupit levněji (tzv. krátká pozice). Obchodník může prodat i artikl, který právě nevlastní (v reálu to funguje tak, že si ho od někoho, kdo ho vlastní, “vypůjčí”, prodá jej, a potom ho koupí za nižší cenu a “vrátí”). V našem případě budeme uvažovat, že obchodníkův systém posílá na burzu příkazy k nákupu (buy) nebo prodeji (sell) určitého množství jednotek artiklu označeného nějakým tickerem.

## Specifikace rozhraní skriptu
tradelog \[-h|--help] \[FILTR] \[PŘÍKAZ] [LOG [LOG2 [...]]

## PŘÍKAZ může být jeden z:
- list-tick – výpis seznamu vyskytujících se burzovních symbolů, tzv. “tickerů”.
- profit – výpis celkového zisku z uzavřených pozic.
- pos – výpis hodnot aktuálně držených pozic seřazených sestupně dle hodnoty.
- last-price – výpis poslední známé ceny pro každý ticker.
- hist-ord – výpis histogramu počtu transakcí dle tickeru.
- graph-pos – výpis grafu hodnot držených pozic dle tickeru.

## FILTR může být kombinace následujících:
- -a DATETIME – after: jsou uvažovány pouze záznamy PO tomto datu (bez tohoto data). DATETIME je formátu YYYY-MM-DD HH:MM:SS.
- -b DATETIME – before: jsou uvažovány pouze záznamy PŘED tímto datem (bez tohoto data).
- -t TICKER – jsou uvažovány pouze záznamy odpovídající danému tickeru. Při více výskytech přepínače se bere množina všech uvedených tickerů.
- -w WIDTH – u výpisu grafů nastavuje jejich šířku, tedy délku nejdelšího řádku na WIDTH. Tedy, WIDTH musí být kladné celé číslo. Více výskytů přepínače je chybné spuštění.
- -h a --help vypíšou nápovědu s krátkým popisem každého příkazu a přepínače.

## Popis
- Skript filtruje záznamy z nástroje pro obchodování na burze. Pokud je skriptu zadán také příkaz, nad filtrovanými záznamy daný příkaz provede.
- Pokud skript nedostane ani filtr ani příkaz, opisuje záznamy na standardní výstup.
- Skript umí zpracovat i záznamy komprimované pomocí nástroje gzip (v případě, že název souboru končí .gz).
- V případě, že skript na příkazové řádce nedostane soubory se záznamy (LOG, LOG2 …), očekává záznamy na standardním vstupu.
- Pokud má skript vypsat seznam, každá položka je vypsána na jeden řádek a pouze jednou. Není-li uvedeno jinak, je pořadí řádků dáno abecedně dle tickerů. Položky se nesmí opakovat.
- Grafy jsou vykresleny pomocí ASCII a jsou otočené doprava. Každý řádek histogramu udává ticker. Kladná hodnota či četnost jsou vyobrazeny posloupností znaku mřížky #, záporná hodnota (u graph-pos) je vyobrazena posloupností znaku vykřičníku !.
## Podrobné požadavky
- Skript analyzuje záznamy (logy) pouze ze zadaných souborů v daném pořadí.
- Formát logu je CSV kde oddělovačem je znak středníku ;. Formát je řádkový, každý řádek odpovídá záznamu o jedné transakci ve tvaru
- `DATUM A CAS;TICKER;TYP TRANSAKCE;JEDNOTKOVA CENA;MENA;OBJEM;ID`
    - DATUM A CAS jsou ve formátu YYYY-MM-DD HH:MM:SS
    - TICKER je řetězec neobsahující bílé znaky a znak středníku
    - TYP TRANSAKCE nabývá hodnoty buy nebo sell značící nákup resp. prodej
    - JEDNOTKOVA CENA je cena za jednu akcii, jednotku komodity, atp. s přesností na maximálně dvě desetinná místa; jako oddělovač jednotek a desetin slouží znak tečky .; Např. 1234567.89
    - MENA je třípísmenný kód měny, např USD, EUR, CZK, SEK, GBP atd.
    - OBJEM značí množství jednotek (akcií, jednotek komodity atp.) v transakci
    - ID je identifikátor transakce (řetězec neobsahující bílé znaky a znak středníku)
    - Hodnota transakce je JEDNOTKOVA CENA * OBJEM. Příklad záznamů:

- 2021-07-29 23:43:13;TSM;buy;667.90;USD;306;65fb53f6-7943-11eb-80cb-8c85906a186d
- 2021-07-29 23:43:15;BTC;sell;50100;USD;5;65467d26-7943-11eb-80cb-8c85906a186d
- První záznam značí nákup 306 akcií firmy TSMC (ticker TSM) za cenu 667.90 USD / akcie. Hodnota transakce je tedy 204377.40 USD.
- Druhý záznam značí prodej 5 bitcoinů (ticker BTC) za cenu 50 100 USD / bitcoin. Hodnota transakce je tedy 250500.00 USD.
- Předpokládejte, že měna je u všech záznamů stejná (není potřeba ověřovat).

- Skript žádný soubor nemodifikuje. Skript nepoužívá dočasné soubory.
- Můžete předpokládat, že záznamy jsou ve vstupních souborech uvedeny chronologicky a je-li na vstupu více souborů, jejich pořadí je také chronologické.
- Celkový zisk z uzavřených pozic (příkaz profit) se spočítá jako suma hodnot sell transakcí - suma hodnot buy transakcí.
- Hodnota aktuálně držených pozic (příkazy pos a graph-pos) se pro každý ticker spočítá jako počet držených jednotek * jednotková cena z poslední transakce, kde počet držených jednotek je dán jako suma objemů buy transakcí - suma objemů sell transakcí.
- Pokud není při použití příkazu hist-ord uvedena šířka WIDTH, pak každá pozice v histogramu odpovídá jedné transakci.
- Pokud není při použití příkazu graph-pos uvedena šířka WIDTH, pak každá pozice v histogramu odpovídá hodnotě 1000 (zaokrouhleno na tisíce směrem k nule, tj. hodnota 2000 bude reprezentována jako ## zatímco hodnota 1999.99 jako # a hodnota -1999.99 jako !.
- U příkazů hist-ord a graph-pos s uvedenou šířkou WIDTH při dělení zaokrouhlujte směrem k nule (tedy např. při graph-pos -w 6 a nejdelším řádku s hodnotou 1234 bude řádek s hodnotou 1234 vypadat takto ######, řádek s hodnotou 1233.99 takto ##### a řádek s hodnotou -1233.99 takto !!!!!).
- Pořadí argumentů stačí uvažovat takové, že nejřív budou všechny přepínače, pak (volitelně) příkaz a nakonec seznam vstupních souborů (lze tedy použít getopts). Podpora argumentů v libovolném pořadí je nepovinné rozšíření, jehož implementace může kompenzovat případnou ztrátu bodů v jiné časti projektu.
- Předpokládejte, že vstupní soubory nemůžou mít jména odpovídající některému příkazu nebo přepínači.
- V případě uvedení přepínače -h nebo --help se vždy pouze vypíše nápověda a skript skončí (tedy, pokud by za přepínačem následoval nějaký příkaz nebo soubor, neprovede se).
- Při výpisu pomocí příkazů pos, last-price, hist-ord a graph-pos musí být tickery zarovnány doleva a dvojtečka na 11. pozici na řádku (výplň proveďte pomocí mezer). U příkazů hist-ord a graph-pos je za dvojtečkou na všech řádcích právě jedna mezera (případně žádná, pokud v pravém sloupci daného řádku nic není), u příkazů pos a last-price jsou hodnoty v pravé části výpisu formátovány tak, aby (v případě neprázdného výpisu) byla na řádku s nejdelší řetězcovou reprezentací hodnoty (tj. včetně znaménka) mezi dvojtečkou a hodnotou právě jedna mezera a ostatní řádky byly zarovnány doprava vzhledem k délce tohoto řádku (vizte příklady výpisů níže).

## Návratová hodnota
- Skript vrací úspěch v případě úspěšné operace. Interní chyba skriptu nebo chybné argumenty budou doprovázeny chybovým hlášením a neúspěšným návratovým kódem.

## Implementační detaily
- Skript by měl mít v celém běhu nastaveno POSIXLY\_CORRECT=yes.
- Skript by měl běžet na všech běžných shellech (dash, ksh, bash). Pokud použijete vlastnost specifickou pro nějaký shell, uveďte to pomocí direktivy interpretu na prvním řádku souboru, např. #!/bin/bash nebo #!/usr/bin/env bash pro bash. Můžete použít GNU rozšíření pro sed či awk. Jazyky Perl, Python, Ruby, atd. povoleny nejsou.
- UPOZORNĚNÍ: některé servery, např. merlin.fit.vutbr.cz, mají symlink /bin/sh -> bash. Ověřte si proto, že skript skutečně testujete daným shellem. Doporučuji ověřit správnou funkčnost pomocí virtuálního stroje níže.
- Skript musí běžet na běžně dostupných OS GNU/Linux, BSD a MacOS.
- Skript nesmí používat dočasné soubory. Povoleny jsou však dočasné soubory nepřímo tvořené jinými příkazy (např. příkazem sed -i).
- Čísla vypisujte v desítkovém zápisu s přesností na dvě desetinná místa. Pozor, některé nástroje (např. awk) mohou větší čísla vypisovat implicitně pomocí vědeckého zápisu.

# Project 2
## Podrobná specifikace úlohy
### Spuštění:
$ ./proj2 NE NR TE TR
- NE: počet skřítků. 0\<NE\<1000
- NR: počet sobů. 0\<NR\< 20
- TE: Maximální doba v milisekundách, po kterou skřítek pracuje samostatně. 0\<=TE<=1000.
- TR: Maximální doba v milisekundách, po které se sob vrací z dovolené domů. 0\<=RE<=1000.
- Všechny parametry jsou nezáporná celá čísla.

### Chybové stavy:
- Pokud některý ze vstupů nebude odpovídat očekávanému formátu nebo bude mimo povolený
rozsah, program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud
alokované zdroje a ukončí se s kódem (exit code) 1.
- Pokud selže některá z operací se semafory, nebo sdílenou pamětí, postupujte stejně-- program
vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované
zdroje a ukončí se s kódem (exit code) 1.

### Implementační detaily:
- Každý proces vykonává své akce a současně zapisuje informace o akcích do souboru s názvem
proj2.out. Součástí výstupních informací o akci je pořadové číslo A prováděné akce (viz popis
výstupů). Akce se číslují od jedničky.
- Použijte sdílenou paměť pro implementaci čítače akcí a sdílených proměnných nutných pro
synchronizaci.
- Použijte semafory pro synchronizaci procesů.
- Nepoužívejte aktivní čekání (včetně cyklického časového uspání procesu) pro účely
synchronizace.
- Pracujte s procesy, ne s vlákny.

### Hlavní proces
1. Hlavní proces vytváří ihned po spuštění jeden proces Santa, NE procesů skřítků a NR procesů sobů.
2. Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy
ukončeny, ukončí se i hlavní proces s kódem (exit code) 0.

### Proces Santa
1. Po spuštění vypíše: A: Santa: going to sleep
2. Po probuzení skřítky jde pomáhat elfům---vypíše: A: Santa: helping elfes
3. Poté, co pomůže skřítkům jde spát (bez ohledu na to, jestli před dílnou čekají další skřítci)
a vypíše: A: Santa: going to sleep
4. Po probuzení posledním sobem uzavře dílnu a vypíše: A: Santa: closing workshop
a pak jde ihned zapřahat soby do saní.
5. Ve chvíli, kdy jsou zapřažení všichni soby vypíše: A: Stanta: Christmass started
a ihned proces končí.

### Proces Skřítek
1. Každý skřítek je unikátně identifikován číslem elfID. 0\<elfID<=NE
2. Po spuštění vypíše: A: Elf elfID: started
3. Samostatnou práci modelujte voláním funkce usleep na náhodný čas v intervalu \<0,TE>.
4. Když skončí samostatnou práci, potřebuje pomoc od Santy. Vypíše: A: Elf elfID: need help
a zařadí se do fronty před Santovou dílnou.
5. Pokud je třetí ve frontě před dílnou, dílna je prázdná a na dílně není cedule „Vánoce – zavřeno“,
tak společně s prvním a druhým z fronty vstoupí do dílny a vzbudí Santu.
6. Skřítek v dílně dostane pomoc a vypíše: A: Elf elfID: get help (na pořadí pomoci skřítkům v
dílně nezáleží)
7. Po obdržení pomoci ihned odchází z dílny a pokud je dílna již volná, tak při odchodu z dílny
může upozornit čekající skřítky, že už je volno (volitelné).
8. Pokud je na dveřích dílny nápis „Vánoce – zavřeno“ vypíše: A: Elf elfID: taking holydays
a proces ihned skončí.

### Proces Sob
1. Každý sob je identifikován číslem rdID, 0\<rdID<=NR
2. Po spuštění vypíše: A: RD rdID: rstarted
3. Čas na dovolené modelujte voláním usleep na náhodný interval \<TR/2,TR>
4. Po návratu z letní dovolené vypíše: A: RD rdID: return home
a následně čeká, než ho Santa zapřáhne k saním. Pokud je posledním sobem, který se vrátil z
dovolené, tak vzbudí Santu.
5. Po zapřažení do saní vypíše: A: RD rdID: get hitched
a následně proces končí.

# Project 2v2
## Popis Úlohy (Building H2O)
Molekuly vody jsou vytvářeny ze dvou atomů vodíku a jednoho atomu kyslíku. V systému jsou tři typy
procesů: (0) hlavní proces, (1) kyslík a (2) vodík. Po vytvoření procesů se procesy reprezentující
kyslíky a vodíky řadí do dvou front—jedna pro kyslíky a druhá pro vodíky. Ze začátku fronty vždy
vystoupí jeden kyslík a dva vodíky a vytvoří molekulu. V jednu chvíli je možné vytvářet pouze jednu
molekulu. Po jejím vytvoření je prostor uvolněn dalším atomům pro vytvoření další molekuly. Procesy,
které vytvořily molekulu následně končí. Ve chvíli, kdy již není k dispozici dostatek atomů kyslíku
nebo vodíku pro další molekulu (a ani žádné další již nebudou hlavním procesem vytvořeny) jsou
všechny zbývající atomy kyslíku a vodíku uvolněny z front a procesy jsou ukončeny.

## Podrobná specifikace úlohy
Spuštění:
$ ./proj2 NO NH TI TB
- NO: Počet kyslíků
- NH: Počet vodíků
- TI: Maximální čas milisekundách, po který atom kyslíku/vodíku po svém vytvoření čeká, než se
zařadí do fronty na vytváření molekul. 0\<=TI\<=1000
- TB: Maximální čas v milisekundách nutný pro vytvoření jedné molekuly. 0\<=TB\<=1000
Chybové stavy:
- Pokud některý ze vstupů nebude odpovídat očekávanému formátu nebo bude mimo povolený
rozsah, program vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud
alokované zdroje a ukončí se s kódem (exit code) 1.
- Pokud selže některá z operací se semafory, nebo sdílenou pamětí, postupujte stejně--program
vytiskne chybové hlášení na standardní chybový výstup, uvolní všechny dosud alokované
zdroje a ukončí se s kódem (exit code) 1.
Implementační detaily:
- Každý proces vykonává své akce a současně zapisuje informace o akcích do souboru s názvem
proj2.out. Součástí výstupních informací o akci je pořadové číslo A prováděné akce (viz popis
výstupů). Akce se číslují od jedničky.
- Použijte sdílenou paměť pro implementaci čítače akcí a sdílených proměnných nutných pro
synchronizaci.
- Použijte semafory pro synchronizaci procesů.
- Nepoužívejte aktivní čekání (včetně cyklického časového uspání procesu) pro účely
synchronizace.
- Pracujte s procesy, ne s vlákny.

## Hlavní proces
- Hlavní proces vytváří ihned po spuštění NO procesů kyslíku a NH procesů vodíku.
- Poté čeká na ukončení všech procesů, které aplikace vytváří. Jakmile jsou tyto procesy
ukončeny, ukončí se i hlavní proces s kódem (exit code) 0.

## Proces Kyslík
- Každý kyslík je jednoznačně identifikován číslem idO, 0\<idO\<=NO
- Po spuštění vypíše: A: O idO: started
- Následně čeká pomocí volání usleep náhodný čas v intervalu \<0,TI\>
- Vypíše: A: O idO: going to queue a zařadí se do fronty kyslíků na vytváření molekul.
- Ve chvíli, kdy není vytvářena žádná molekula, jsou z čela front uvolněny kyslík a dva vodíky.
Příslušný proces po uvolnění vypíše: A: O idO: creating molecule noM (noM je číslo molekuly,
ty jsou číslovány postupně od 1).
- Pomocí usleep na náhodný čas v intervalu \<0,TB\> simuluje dobu vytváření molekuly.
- Po uplynutí času vytváření molekuly informuje vodíky ze stejné molekuly, že je molekula
dokončena.
- Vypíše: A: O idO: molecule noM created a proces končí.
- Pokud již není k dispozici dostatek vodíků (ani nebudou žádné další vytvořeny/zařazeny do
fronty) vypisuje: A: O idO: not enough H a proces končí.

## Proces Vodík
- Každý vodík je jednoznačně identifikován číslem idH, 0\<idH\<=NH
- Po spuštění vypíše: A: H idH: started
- Následně čeká pomocí volání usleep náhodný čas v intervalu \<0,TI\>
- Vypíše: A: H idH: going to queue a zařadí se do fronty vodíků na vytváření molekul.
- Ve chvíli, kdy není vytvářena žádná molekula, jsou z čela front uvolněny kyslík a dva vodíky.
Příslušný proces po uvolnění vypíše: A: H idH: creating molecule noM (noM je číslo molekuly,
ty jsou číslovány postupně od 1).
- Následně čeká na zprávu od kyslíku, že je tvorba molekuly dokončena.
- Vypíše: A: H idH: molecule noM created a proces končí.
- Pokud již není k dispozici dostatek kyslíků nebo vodíků (ani nebudou žádné další
vytvořeny/zařazeny do fronty) vypisuje: A: H idH: not enough O or H a process končí.
