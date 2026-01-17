## Struktura napájení
![Struktura napájení](../images/ksitovka_napajeni.drawio.svg)

- 2× paralelně spojené Li-Ion baterie  
- Základní ochrana proti přetížení a přepólování  
- Step-up modul pro zajištění stabilních 5 V  
- Lineární regulátor pro zajištění 3,3 V  
- ESP dev kit modul dále obsahuje svůj vlastní regulátor 3,3 V  
- Nabíjecí modul  

Celé toto řešení bylo zvoleno z důvodu poměrně vysokých výkonových požadavků LED pásku, u kterého je i po omezení jasu a počtu současně svítících LED očekávána spotřeba 4–5 A. Tato podmínka vyloučila použití běžně dostupných powerbank. Srdcem kšitovky tedy budou dvě Li-Ion baterie, které díky paralelnímu zapojení vyhovují proudovým požadavkům LED pásku.

Step-up modul je zde primárně určen pro napájení motorového DC driveru, zároveň však snižuje nároky na regulátor 3,3 V. Díky tomu je možné předběžně určit pracovní rozsah napětí baterií na 3,3–4,2 V. Pokud napětí klesne pod úroveň 3,3 V, měla by dle dokumentace začít vykazovat nestandardní chování modrá barva.

## Spotřeba dle napěťových větví
Udaná spotřeba je orientační. Na tyto hodnoty jsou dimenzovány zdroje a součástky.
| Napěťová větev [V] | Komponenta| Typická spotřeba [mA]| Poznámka |
|-------------------|------------|-----------------------|--------------------|
|VBAT (3,3 - 4,2)|LED pásek|4 000|Případně se může omezit více|
||Step-up|-|VBAT => 5V (max. 1 A s chlazením)|
|5|DC Motor|50|V případě zablokování hřídele 1 A *|
||Step-down|-|5V => 3,3V (max 800 mA)|
|3,3|ESP32|300|Pro WiFi špičky přidány kondenzátory|
||Displej|240|-|
||Akcelerometr|1|-|

- *Při zablokování by se měl motor programově vypnout, v zapojení je s měřením proudu motorem počítáno. 

## Nabíjení
Nabíjení akumulátoru je realizováno buď externě pomocí nabíjecí stanice, nebo prostřednictvím USB rozhraní připojeného k modulu ESP. V souladu s napájecí strukturou znázorněnou na obrázku výše dochází po připojení USB k následujícím stavům:

- Odpojení napájení modulu ESP z externího regulátoru napětí. Použitý vývojový modul (Dev Kit) obsahuje vlastní napěťový regulátor, který je v případě napájení z USB využit. Tato vlastnost představuje jednu z nevýhod použití vývojového modulu.

- Nabíjení akumulátoru prostřednictvím integrovaného nabíjecího obvodu TP4056. Nabíjecí proud je nastavován rezistorem připojeným na pin PROG. Maximální proud dostupný ze standardního USB portu je uvažován 500 mA, přičemž část tohoto proudu je současně využívána pro napájení samotného modulu ESP. Z tohoto důvodu je nabíjecí proud omezen na hodnotu 300 mA.