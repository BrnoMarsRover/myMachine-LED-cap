# Main board

## BOM

[Otevřít interaktivní BOM](ibom.html)

## Schéma

[Otevřít schéma](schematic_v0.0.pdf)

## v0.0

### Chyby

- Nefunkční ochrana proti přepólování pomocí "smart diode controller". Problém se vyřeší klasickými schottkeyho diodami, které mají sice vyšší úbytek napětí, ale nakonec proud, který z baterek teče není tak vysoký, takže ho dioda zvládne.

- Nefunkční odpojení napájení ESP po připojení USB. Při stavu, kdy do ESP není připojeno USB je i tak na pinu 5V Vcc napětí 3V3. Problém se vyřeší prostým neosazením součástky TPS22917LDBV. 

- Špatný footprint pro modul step-up. Při otočení a převrácení ho lze osadit.

- Byly objednány špatné JST 2-pin konektory do desky, jejich odlišný design fyzicky brání k připojení konektoru motoru a bat1 zároveň. Musí se objednat ty správné.

