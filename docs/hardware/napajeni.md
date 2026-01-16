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

TODO:

- Tabulka napájení
- Nabíjení