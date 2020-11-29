# CO2 Ampel

## GraphContainer Properties
> Das SVG Element des GraphContainers kann mit Attributen ausgestattet werden, die verschiedene Metadatan anführen

#### data-slag:
Gibt die URL an, an die die request für neue data-values gesendet wird.

#### data-interval:
Gibt in Sekunden an, in welchem Interval auf die gegebene URL eine request gesendet werden soll, um die Daten regelmäßig nach zu laden.

---

## Graph Properties
> Das Element des Graphs kann mit mehreren Attributen ausgestattet werden, die verschiedene Eigenschaften beeinflussen:


#### data-values:
Dieses Argument nimmt eine Zahlenreihe entgegen, die die Werte repräsentiert, die der Graph darstellen soll.

#### data-min:
Im default entspricht dieser Wert dem kleinsten Wert in *data-values*.
Ist er gesetzt, startet der Graph auf diesem Wert.

#### data-max:
Im default entspricht dieser Wert dem höchsten Wert in *data-values*.
Ist er gesetzt, endet der Graph auf diesem Wert.

#### data-clipping:
Im default entspricht dieser Wert true und die Werte werden abgeschnitten, falls sie den durch *data-min* und *data-max* angegebenen Bereich verlassen.
Ist er auf false gesetzt, werden *data-min* und *data-max* ausgedeht, wenn die Werte den angegebenen Bereich verlassen.

#### data-stepsize:
Gibt an, in welchen Schritten die Beschriftung der Daten angegeben werden soll.

#### data-dencity:
Gibt an, in welchem Minuten-Intervall die Daten aufgenommen wurden.
