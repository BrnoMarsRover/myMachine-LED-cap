## Blokové schéma
Hlavní bloky jsou zobrazeny na obrázku.
![Propojení komponent](../images/ksitovka_blok.drawio.svg)


## Elektrické propojení komponent
Komponenty se propojí dle obrázku:
![Propojení komponent](../images/ksiltovka_blok_diafram.drawio.svg)

Jako konektor byl pro celý projekt zvolen JST-XH, lišící se v počtu pinů. 
- U dvoužilového konektoru mezi deskami určeného pro napájení motoru nezáleží na pořadí žil.
- Zvlášťní pozornost je třeba věnovat LED páskům, kde je třeba kontrolovat pořadí žil. I při k top board je třeba stále myslet na pořadí.
- Na piny od modulu displeje bude nejlepší naletovat jednotlivé žíly přímo a na druhý konec nasadit 9 pin konektor, je k tomu primárně určený kabel, který má žíly spojeny k sobě. A potom jen doufat, že to při návrhu PCB nikdo neotočil a nebude se muset všech 9 žil křížit :) .
