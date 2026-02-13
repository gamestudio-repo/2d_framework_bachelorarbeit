# Minimal2D Collision Framework (C++ / Windows)

Ein kleines, bewusst **minimal gehaltenes 2D-Framework** für **Kollisionserkennung** bzw. das **Testen von Datenstrukturen**, die zur räumlichen Abfrage genutzt werden.

Im Fokus steht das **Vergleichen/Validieren** von Datenstrukturen anhand von genau **5 Kernfunktionen**:

- `Build`
- `Query`
- `Insert`
- `Remove`
- `Update`

Das Projekt ist in **C++** entwickelt, läuft auf **Windows (getestet unter Windows 10)** und wird mit dem **MinGW64 Standalone Compiler** kompiliert.

---

## Inhalt

- [Projektidee](#projektidee)
- [Was wird getestet](#was-wird-getestet)
- [Ordnerstruktur](#ordnerstruktur)
- [Voraussetzungen](#voraussetzungen)
- [Quick Start: Build & Run](#quick-start-build--run)
- [So funktioniert das Framework](#so-funktioniert-das-framework)
- [Konsolenbefehle](#konsolenbefehle)
- [Szenenaufbau](#szenenaufbau)
- [Szenenkonfigurationsdateien](#szenenkonfigurationsdateien)
- [Logdateien](#logdateien)
- [TestSzenen](#testszenen)
- [Lizenz](#lizenz)

---

## Projektidee

Dieses Repository stellt ein **kleines 2D-Testframework** bereit, um **räumliche Datenstrukturen** (z. B. Grid, Quadtree, BVH, …) einheitlich zu testen.

Ziel ist es, **Kollisionserkennung / räumliche Abfragen** (z. B. „Welche Objekte liegen in dieser Region?“) über verschiedene Datenstrukturen vergleichbar zu machen – mit einem klaren, gemeinsamen Interface.

---

## Was wird getestet

Jede Datenstruktur wird über die folgenden 5 Operationen geprüft:

1. **Build**  
   Aufbau der Datenstruktur aus einem bestehenden Objekt-Set.

2. **Query**  
   Abfrage nach Kandidaten/Objekten in einem Suchbereich (z. B. AABB/Rect).

3. **Insert**  
   Einfügen eines neuen Objekts.

4. **Remove**  
   Entfernen eines existierenden Objekts.

5. **Update**  
   Aktualisieren eines Objekts (z. B. Position/Bounds geändert) innerhalb der Struktur.

> Hinweis: Das Framework ist minimal – es geht darum, diese 5 Operationen sauber zu testen/benchmarkbar zu machen, nicht um eine „Full Game Engine“.

---

## Ordnerstruktur

```text
.
├─ main.cpp
├─ ...
├─ (weitere .cpp Dateien im Root)
├─ Toolbox/
├─ Datastructures/
└─ Graphics/        (nur Assets, kein Code)
└─ Scene/			(vordefinierte Szenen)
└─ window.txt		(Window Konfigurationsdatei)
```

- **Toolbox/**: Hilfsfunktionen/Utilities (z. B. Mathe, Timer)
- **Datastructures/**: Implementierungen des Interface für die Datenstrukturen und der Datenstrukturen, die getestet werden
- **Graphics/**: Grafikinhalte/Assets (keine `.cpp` / kein Build-relevanter Code)

---

## Voraussetzungen

- **Windows 10** (oder neuer, sollte aber auch unter Win10 sicher laufen)
- **MinGW64 Standalone** (g++ / mingw-w64)
- Optional: Git (zum Klonen des Repos)

---

## Quick Start: Build & Run

> Hinweis: `Graphics/` enthält nur Assets und wird beim Kompilieren ignoriert. `Graphics/` ist essenziell und muss während der Laufzeit im selben Ordner liegen wie die Exe-Datei.  

> Zusätzlich muss auch **`window.txt`** im selben Ordner liegen wie die EXE (z. B. `bin\window.txt`), da darüber die **Szenen-/Fenstergröße** und der **GUI-Modus** konfiguriert werden.

> Zusätzlich muss es im Ausführungsordner auch einen Ordner **`Scene/`** geben (z. B. `bin\Scene\`).  
> In diesem Ordner liegen die einzelnen **Szenenordnerstrukturen** (jede Szene = eigener Unterordner).  
> Der Ordner darf auch **leer** sein, er muss aber existieren, da das Framework dort nach Szenen sucht.

### CMD Build

Diese Variante funktioniert, wenn sich alle `.cpp` Dateien nur im Root, `Toolbox/` und `Datastructures/` befinden (ohne weitere Unterordner).

```bat
mkdir bin 2>nul
g++ -std=c++17 -o bin\framework.exe src\Main.cpp -static -lgdi32 -lws2_32

REM Assets kopieren (Graphics enthält nur Inhalte, keinen Code)
xcopy Graphics bin\Graphics /E /I /Y >nul
REM Falls xcopy nicht auf dem System funktioniert, den Ordner "Graphics" manuell in "bin\" kopieren

REM Konfiguration kopieren (Szenengröße + GUI-Modus)
copy window.txt bin\window.txt >nul
REM Falls copy nicht funktioniert, "window.txt" manuell nach "bin\" kopieren

REM Szenen-Ordner anlegen/kopieren
mkdir bin\Scene 2>nul
REM Alternativ: vorhandenen "Scene" Ordner kopieren:
REM xcopy Scene bin\Scene /E /I /Y >nul

bin\framework.exe
```

### window.txt Format

`window.txt` besteht aus **4 Zeilen**:

```text
Zeile 1: WIDTH
Zeile 2: HEIGHT
Zeile 3: DRAWING
Zeile 4: GUIMODUS
```

- **WIDTH / HEIGHT (Zeile 1 & 2)** sind **entscheidend**, da sie sowohl die **Szenengröße** als auch die **GUI/Fenstergröße** festlegen.
- **#DRAWING (Zeile 3)**: `0` = false, `1` = true  
  - **Wichtig:** Wenn **GUI-Modus aktiv** ist (`#UIMODUS = 1`), muss `DRAWING` **immer `1`** sein, sonst wird **nichts gezeichnet**.
- **#GUIMODUS (Zeile 4)**: `0` = false, `1` = true  
  - `1` = Programm läuft im **GUI-Modus** (du siehst die Szene)
  - `0` = **kein GUI**, nur Konsole/CMD (Framework läuft headless)

---

## So funktioniert das Framework

Das Framework ist so aufgebaut, dass du **eine Szene per Kommando auswählst** und diese dann durch den **Start-Befehl** ausgeführt wird (es sei denn man startet die Anwendung im **GUI-Modus**, dann wird es automatisch ausgeführt wird).

Ablauf:

1. Anwendung starten (`framework.exe`)
2. In der Konsole einen Szenen-Befehl eingeben, z. B.  
   `/testScene_update Anzahl_Updates "SzenenName"`
   `/start` (Falls nicht im **GUI-Modus**)
3. Das Framework lädt die Szene und die Konfigurationsdateien und führt den Test **automatisch** aus.
4. Du wartest, bis die Anwendung fertig ist.
5. Danach erzeugt die Anwendung automatisch **Logdateien**, in denen die Ergebnisse stehen.
6. Du kannst mit `/getUpdates` den derzeitigen Counter auslesen, um zu sehen, wie viele Updates noch gemacht werden müssen.

> Wichtig: Sobald eine Szene gestartet wurde, musst du normalerweise nichts mehr tun – die Testausführung läuft bis zum Ende durch und schreibt anschließend die Logs.

---

## Konsolenbefehle

```text
/help                               -> Zeigt die Befehle an
/drawStruc true|false               -> Zeichnet die Datenstruktur
/testScene_update Anzahl_Updates "SzenenName"
                                    -> Wechselt zu "SzenenName" fuer gewisse Anzahl an Updates
                                       (MAX Value: 18.446.744.073.709.551.615)
/turnMapOn true|false               -> Zeichnet die Map
/showCollisions true|false          -> Zeichnet die Kollisionen
/start                              -> Starte Szene ohne GUI (nur möglich bei keiner GUI)
/getUpdates                         -> Zeigt den Updatecounter an
```

### Beispiele

- Hilfe anzeigen:
  `/help`

- Szene ausführen (z. B. 10.000 Updates):
  `/testScene_update 10000 "TestScene"`

- Zeichnen aktivieren/deaktivieren:
  `/drawStruc true`  
  `/turnMapOn false`  
  `/showCollisions true`

- Updatecounter abfragen:
  `/getUpdates`

> Wichtig: Zeichnen aktivieren/deaktivieren dient eher zu Test- und Lernzwecken. Diese Features sind nicht konstant überprüft worden, weshalb es möglicherweise zum Absturz kommen kann, wenn man einen der 3 Befehle (`drawStruc`,`turnMapOn `,`showCollisions`) ausführt.

---

## Szenenaufbau

Eine **Szene** ist ein **Ordner**, der mehrere Textdateien (und ggf. Unterordner) enthält. Beim Starten einer Szene (z. B. über `/testScene_update ... "SzenenName"`) lädt das Framework diese Dateien ein und führt den Test automatisch  nach dem Befehl `/start` aus (im **GUI-Modus** komplett automatisch).

### Struktur einer Szene

```text
SzenenName/
├─ benchmarks/        (muss existieren, kann leer sein)
├─ objects/           (muss existieren, kann leer sein)
├─ move_path/
├─ Commands.txt       (muss existieren, kann leer sein)
├─ Datastruc.txt
├─ Objects.txt
├─ Operations.txt
└─ Map.txt            (nur nötig im GUI-Modus)
```

### Pflicht / Optional

**Muss existieren (auch wenn leer):**
- `objects/`
- `Commands.txt`
- `benchmarks`

> `Commands.txt` und `objects/` sind beide für zukünftige Projekte/Tests vorgesehen und werden aktuell **nicht** verwendet.  
> Sie sollten **nicht bearbeitet** werden – es reicht, wenn ein **leerer Ordner** `objects/` und eine **leere Datei** `Commands.txt` vorhanden sind.

**Nur im GUI-Modus erforderlich:**
- `Map.txt`

> Wenn du im **GUI-Modus** startest und `Map.txt` fehlt, erscheint eine Fehlermeldung wie:  
> **„Fehler keine Map.txt gefunden“**  
> Im **Non-GUI/CLI-Modus** muss `Map.txt` **nicht** vorhanden sein.

### Bedeutung von `Map.txt`
#### Map.txt (GUI / Optik)

`Map.txt` ist **ausschließlich für visuelle Testzwecke im GUI-Modus** gedacht (Background/Layer), z. B. um eine Szene „schöner“ darzustellen oder das Verhalten **optisch** zu prüfen.

- **Für Benchmarks / Auswertungen sollte `Map.txt` nicht genutzt werden.**
- **Starte dafür nicht im GUI-Modus** (`GUIMODUS = 0` in `window.txt` und dann `/start` bzw. per Test-Befehl laufen lassen).

> Hinweis: `Map.txt` hat **keinen Einfluss auf die Logik** der Tests (Build/Query/Insert/Remove/Update).  
> Der GUI-Modus kann jedoch durch Rendering/Overhead Messwerte beeinflussen – daher für reproduzierbare Tests besser **ohne GUI**.

---

###### Format / Aufbau

`Map.txt` ist zeilenbasiert aufgebaut und beschreibt ein **Tile-Raster** plus eine **ID-Matrix**, die auf ein Tileset-Bild verweist.

####### Zeile 1: Tile-Größe (Pixel pro Raster-Feld)

Beispiel:
```text
32x32
```

→ Jedes Feld im Raster ist **32 Pixel breit** und **32 Pixel hoch**.

---

####### Zeile 2: Map-Größe (Columns x Rows)

Beispiel:
```text
36x27
```

→ Die Map besteht aus **36 Spalten** und **27 Reihen** (jedes Feld hat die oben definierte Tile-Größe).

---

####### Zeile 3: Tileset-Dateiname

Beispiel:
```text
Test1.bmp
```

→ Das Bild, aus dem die Tiles später geladen/geschnitten werden.  
Erlaubt sind **`.png`** oder **`.bmp`**.

---

####### Zeile 4: Start-Signal für die Grid-Daten

Beispiel:
```text
######
```

→ Diese Zeile (mindestens ein `#`) ist das Signal für den Loader:  
**Ab hier kommen die Tile-IDs als Grid.**

---

####### Danach: Tile-IDs als Grid

Jetzt kommen Zeilen wie:
```text
205,206,205,206,...
```

Regeln:
- **Jede Zeile entspricht einer Reihe (Row)** der Map (erste Zeile nach `######` = Row 0, zweite = Row 1, …).
- **Komma-getrennte Werte** sind die **Spalten (Columns)** in dieser Reihe (erstes Element = Column 0, zweites = Column 1, …).
- Der Wert ist die **Tile-ID**, die auf ein Tile im Tileset-Bild verweist (die Zuordnung/Interpretation passiert im Loader).

Wichtig:
- Pro Zeile müssen genau **so viele IDs** stehen wie `Columns`.
- Es müssen genau **so viele Zeilen** folgen wie `Rows`.

---

####### Letzte Zeile: End-Signal

Am Ende muss erneut eine Zeile mit `#` kommen:
```text
######
```

→ Signalisiert dem Loader: **Map-Daten sind zu Ende.**

---

###### Mini-Beispiel (4 Spalten x 3 Reihen)

```text
32x32
4x3
Tiles.bmp
######
1,1,1,1
1,2,2,1
1,1,1,1
######
```

### Relevante Dateien/Ordner für die Tests

Für die eigentliche Testausführung sind typischerweise relevant:
- `Datastruc.txt` (welche Datenstruktur / Einstellungen)
- `Objects.txt` (Objekte der Szene)
- `Operations.txt` (welche Operationen im Test durchgeführt wird)
- `move_path/` (Bewegungspfade, falls die Szene Bewegungs-Updates simuliert)

> Wichtig ist: Die Dateien müssen im Szenenordner korrekt vorhanden sein, damit die Szene geladen und ausgeführt werden kann.

---

## Szenenkonfigurationsdateien

### Objects.txt

In `Objects.txt` definierst du, welche Objekte in einer Szene gespawnt werden.  
**Eine Zeile = ein Objekt-Template** (oder eine Objektgruppe, wenn Kopien erzeugt werden).

Die Werte sind **Semikolon-getrennt** (`;`).

---

#### Grundformat (pro Zeile)

```text
Tile_info;Copy_bool;(copy_amount;random_pos;)ImageFileName;x;y;move_type;(((path_filename)|(d_xpos;d_ypos;));)speed(;tilewidth;tileheight;{{Tile1},{Tile2}})(;pattern_count;dir1;dir2;...dirN)
```

> Wichtig: ImageFileName muss die Endung des Image-Formats haben, z.B. `Test.png`. Es werden nur folgeden Imageformate unterstützt: .png, .bmp. Die Imagine Datei muss immer im Ordner **Graphics/** liegen und wird auch nur von dort aus geladen. Ansonsten gibt es einen **ERROR** und das Programm stürzt ab!
> Wichtig: Movetype 6, also die Bewegung in Richtung der Zielkoordinate, ist Testweise implentiert worden, aber nicht vollständig geprüft sowie ausreichend getestet worden. Es kann also sein, dass diese Bewegung **Bugs** enthält bzw. nicht so funktioniert wie sie soll.
> Alles in `()` ist **nur unter bestimmten Bedingungen** erforderlich.

---

#### Bedeutungen der Felder

- **Tile_info** (`0` oder `1`)
  - `0` = normales Objekt
  - `1` = Tile-Objekt → Tile-Parameter sind am Ende Pflicht

- **Copy_bool** (`0` oder `1`)
  - `0` = es wird genau **1** Objekt erzeugt
  - `1` = es werden **copy_amount** Objekte erzeugt → dann sind `copy_amount` + `random_pos` Pflicht

- **copy_amount** *(nur wenn `Copy_bool == 1`)*
  - Anzahl der Kopien/Instanzen

- **random_pos** *(nur wenn `Copy_bool == 1`)*
  - Verteilung der Positionen für die Kopien:
    - `0` = alle gleiche (x,y)
    - `1` = random (x,y)
    - `2` = nah beieinander (Cluster)
    - `3` = relativ gut verteilt

- **ImageFile**
  - Dateiname inkl. Endung (z. B. `player.png`)

- **x; y**
  - Startposition

- **move_type** (0–8)
  - `0` = Links
  - `1` = Rechts
  - `2` = Oben
  - `3` = Unten
  - `4` = Custom Path **ohne** Berücksichtigung der entgegengesetzten Richtung bei Kollision
  - `5` = Random Bewegung (von 0–3)
  - `6` = Versucht zur Zielkoordinate **(d_xpos,d_ypos)** zu laufen
  - `7` = Move-Pattern (Liste aus Bewegungsrichtungen 0–3)
  - `8` = Custom Path **mit** Berücksichtigung der entgegengesetzten Richtung bei Kollision

- **speed**
  - Bewegungsgeschwindigkeit
---

#### Bedingungen / Pflichtfelder je nach Modus

##### 1) Kopien erzeugen (`Copy_bool == 1`)
Dann **müssen direkt nach `Copy_bool`** diese zwei Werte kommen:

```text
copy_amount;random_pos;
```

##### 2) Zusätzliche Bewegungs-Parameter (abhängig von `move_type`)

Nach `move_type` kommen **zusätzliche Parameter**, aber nur für bestimmte `move_type`s:

- **move_type == 4 oder move_type == 8** → **Pflicht: `path_filename`**
  - Format (direkt nach `move_type`):
    ```text
    PfadDatei.txt;
    ```
  - Die Pfaddatei liegt typischerweise in `move_path/`.

- **move_type == 6** → **Pflicht: Zielkoordinate**
  - Format (direkt nach `move_type`):
    ```text
    target_x;target_y;
    ```

- **move_type == 7** → **Pflicht: Move-Pattern am Ende**
  - Hier kommt **nach allen normalen Feldern** (inkl. optionaler Tile-Infos) ein Pattern-Block:
    ```text
    ;pattern_count;dir1;dir2;...;dirN
    ```
  - `pattern_count` = Anzahl der Bewegungen
  - `dirX` = Bewegungsrichtung (0–3):
    - `0` = Links
    - `1` = Rechts
    - `2` = Oben
    - `3` = Unten

> Für alle anderen move_types (0,1,2,3,5) gibt es **keine** extra Parameter nach `move_type`.

##### 3) Tile-Objekt (`Tile_info == 1`)
Dann sind **Tile-Parameter Pflicht** (nach `speed`). Dadurch wird ein Tile(-Bild) aus dem orginalen Bild **(ImageFileName)** erzeugt:

```text
;tilewidth;tileheight;{{Tile1},{Tile2}}
```

- `tilewidth` / `tileheight` = Tile-Größe
- `{{...},{...}}` = Tile-Matrix / Tile-Liste im von dir verwendeten Format  
  (z. B. `{{1,3},{4,4}}`)
---

#### Beispiele

**Normales Objekt, keine Kopien, Random-Bewegung**
```text
0;0;Name.png;100;200;5;1
```

**Normales Objekt, mit Kopien (copy_amount=10), random verteilt (random_pos=1)**
```text
0;1;10;1;Name.png;100;200;5;2
```

**Tile-Objekt**
```text
1;0;Tiles.png;0;0;5;1;16;16;{{1,3},{4,4}}
```

**Tile-Objekt mit Custom Path (move_type=4)**
```text
1;1;5;1;Tiles.png;0;0;4;"Test.txt";1;16;16;{{1,3},{4,4}}
```

**Tile-Objekt mit Move-Pattern (move_type=7)**  
Beispiel: 5 Bewegungen → Links, Links, Oben, Rechts, Oben
```text
1;1;1;1;Tiles.png;0;0;7;1;16;16;{{1,3},{4,4}};5;0;0;2;1;2
```

---

### Operations.txt

In `Operations.txt` definierst du, **welche Operationen** während eines Tests ausgeführt werden (z. B. Inserts/Removes/Random-Workloads).  
**Eine Zeile** beschreibt dabei einen kompletten Operations-Block.

Die Werte sind **Semikolon-getrennt** (`;`).

---

#### Grundformat (pro Zeile)

```text
Amount;Type_mode(;Type_mode_extra)(;weitere Parameter...)
```

- **Amount**: Anzahl der Operationen, die ausgeführt werden
- **Type_mode**: Art des Operations-Blocks
- **Type_mode_extra**: Zusatzmodus (nur bei bestimmten `Type_mode` Pflicht)

---

#### Type_mode Übersicht

- `0` = Nix / Error (keine sinnvolle Operation)
- `1` = Fix vordefinierte Operation (genau definierte Operation, inkl. Parameter)
- `2` = Random (zufällige Operationen)
- `3` = Random Inserts (zufällige Inserts: random Größe & Position)
- `4` = Remove (verschiedene Remove-Strategien, über `Type_mode_extra`)
- `5` = Insert-Remove abwechselnd (Pattern: Insert → Remove → Insert → Remove …)

---

### Type_mode Details

#### Type_mode = 1 (Fix vordefinierte Operation)

**Format:**
```text
Amount;1;Type_mode_extra(;weitere Parameter...)
```

- `Type_mode_extra`:
  - `0` = Insert
  - `1` = Remove

##### Type_mode_extra = 0 (Insert)
Dann müssen **zusätzliche Objektinfos** folgen:

```text
Amount;1;0;Width;Height;x;y
```

- `Width;Height` = Größe des einzufügenden Objekts
- `x;y` = Position des Objekts

**Beispiel:**
```text
1;1;0;20;20;0;4
```
→ Fügt ein **20x20** Objekt bei **(0,4)** ein.

##### Type_mode_extra = 1 (Remove)
Dann muss die **Objekt-ID** folgen:

```text
Amount;1;1;ObjektID
```

**Beispiel:**
```text
1;1;1;200
```
→ Entfernt Objekt mit **ID 200**.

---

#### Type_mode = 2 (Random)

**Format:**
```text
Amount;2
```

**Beispiel:**
```text
100;2
```
→ Führt **100 zufällige Operationen** aus.

---

#### Type_mode = 3 (Random Inserts)

**Format:**
```text
Amount;3
```

**Beispiel:**
```text
100;3
```
→ Führt **100 random Inserts** aus (Größe und Position zufällig).

---

#### Type_mode = 4 (Remove)

**Format:**
```text
Amount;4;Type_mode_extra
```

`Type_mode_extra` bestimmt die Remove-Strategie:

- `0` = random
- `1` = letztes Element
- `2` = erstes Element
- `3` = alternierend letztes → erstes → letztes → …
- `4` = alternierend random → letztes → erstes → random → …

**Beispiele:**
```text
100;4;0
```
→ 100x Remove (random)

```text
100;4;1
```
→ 100x Remove (letztes Element)

```text
100;4;3
```
→ 100x Remove (abwechselnd letztes/erstes)

```text
100;4;4
```
→ 100x Remove (abwechselnd random/letztes/erstes)

---

#### Type_mode = 5 (Insert-Remove abwechselnd)

**Format:**
```text
Amount;5
```

**Beschreibung:**  
Führt abwechselnd Inserts und Removes aus, typisches Pattern z. B.:

- Insert
- Remove (random)
- Insert
- Remove (letztes Element)
- Insert
- Remove (erstes Element)
- …

**Beispiel:**
```text
100;5
```
→ 100 Operationen im Insert/Remove-Wechsel.

---

### Datastruc.txt

In `Datastruc.txt` wird festgelegt, **welche Datenstrukturen** in einer Szene getestet werden.  
Die Datei wird **Zeile für Zeile** abgearbeitet. Für **jede** eingetragene Datenstruktur gilt:

- Die Szene wird **neu gestartet / zurückgesetzt** (wieder am Anfang)
- Die gleiche Szene wird dann **mit der jeweiligen Datenstruktur** ausgeführt
- Danach werden Logdateien erzeugt (oder ergänzt), sodass du die Ergebnisse pro Struktur vergleichen kannst

---

#### Grundformat (pro Zeile)

```text
DatastructID;AUSWERTUNGSNAME(;zusätzliche Parameter...)
```

- **DatastructID**: Interne ID der Datenstruktur (wird zum Erstellen/Instanziieren verwendet)
- **AUSWERTUNGSNAME**: Frei wählbarer Name, der **genau so in den Logdateien** als Bezeichnung verwendet wird  
  → hat **keinen Einfluss** auf die Logik/Ergebnisse, nur auf die Benennung in den Logs
- **zusätzliche Parameter**: Nur bei bestimmten Datenstrukturen erforderlich

> Hinweis: Werte sind **Semikolon-getrennt** (`;`).

---

### DatastructID Übersicht

```text
0 = Quadtree
1 = BVH
2 = UniformGrid
3 = Compressed_GridQuadtree
4 = Compressed_BVHQuadtree
5 = Ugrid_BVH
6 = Compressed_Quadtree
```

---

### Formate & Beispiele pro Datenstruktur

#### 0) Quadtree

**Format:**
```text
0;NAME;MAX_TIEFE
```

**Beispiel (Tiefe 5):**
```text
0;QUADTREE;5
```

---

#### 1) BVH

**Format:**
```text
1;NAME
```

**Beispiel:**
```text
1;BVH
```

---

#### 2) UniformGrid

**Format:**
```text
2;NAME;RASTER_WIDTH;RASTER_HEIGHT
```

**Beispiel (Raster 32x32):**
```text
2;UNIFORM_GRID;32;32
```

---

#### 3) Compressed_GridQuadtree

**Format:**
```text
3;NAME;MAX_TIEFE;RASTER_WIDTH;RASTER_HEIGHT
```

**Beispiel (Tiefe 4, Raster 16x16):**
```text
3;COMPRESSED_GRIDQUADTREE;4;16;16
```

---

#### 4) Compressed_BVHQuadtree

**Format:**
```text
4;NAME;MAX_TIEFE
```

**Beispiel (Tiefe 4):**
```text
4;COMPRESSED_BVHQUADTREE;4
```

---

#### 5) UGrid_BVH

**Format:**
```text
5;NAME;RASTER_WIDTH;RASTER_HEIGHT
```

**Beispiel (Raster 32x32):**
```text
5;UGRID_BVH;32;32
```

---

#### 6) Compressed_Quadtree

**Format:**
```text
6;NAME;MAX_TIEFE
```

**Beispiel (Tiefe 5):**
```text
6;COMPRESSED_QUADTREE;5
```

---

#### Tipp: AUSWERTUNGSNAME sinnvoll wählen

Da der **AUSWERTUNGSNAME** genau so in deinen **Logdateien** auftaucht, lohnt es sich, dort z. B. Parameter direkt mit abzubilden, z. B.:

```text
0;QT_depth5;5
2;UG_32x32;32;32
```

---

## Logdateien

Beim Ausführen einer Szene erstellt das Framework automatisch Logdateien.  
Wenn eine Logdatei **bereits existiert**, werden neue Messungen **am Ende der Datei angehängt** (appended) und nicht überschrieben.

---

### Welche Logs werden erstellt?

1) **`Comparing.txt`** (im Szenen-Verzeichnis)  
- Enthält das **Endergebnis** der Messungen **einmalig am Ende** (Summary)  
- Am Ende wird außerdem ein **Vergleich gegen BruteForce** ausgegeben (z. B. „QUADTREE vs BruteForce“)

2) Ordner **`benchmarks/`** (im Szenen-Verzeichnis) 
- Enthält pro Datenstruktur eine eigene Logdatei:
  - **`benchmarks/Name_der_Datenstruktur.txt`**
- **`Name_der_Datenstruktur`** ist genau der **AUSWERTUNGSNAME**, den du in `Datastruc.txt` vergeben hast.
- In diesen Dateien werden **Snapshots/Logs ca. jede 1 Sekunde** aufgenommen (laufende Messungen während der Ausführung).

---

### Was steht in den Logs?

Die Logdaten enthalten typischerweise:

- **Zeitmessungen** für Operationen (z. B. Build, Query, Insert, Remove, Update)
  - `Longest Time`, `Shortest Time`, `Average Time`
- **Counter / Trefferzahlen**
  - z. B. bei `Query`: wie viele Objekte gefunden wurden (`Found: ...`)
- **Szeneninformationen**
  - z. B. Anzahl der Objekte in der Szene
- **Speicherverbrauch der Datenstruktur**
  - z. B. Total Allocated Bytes und Allocation-Counts pro StructID
- **Vergleich gegen BruteForce** (in `Comparing.txt` am Ende)
  - z. B. wie viel langsamer/schneller eine Struktur im Vergleich ist

---

### Beispiel (Auszug)

```text
[2025-12-26 14:10:36] ======================Speicherverwaltung-Anfang======================
[2025-12-26 14:10:36] --->Szene mit Objektanzahl: 5000
[2025-12-26 14:10:36] [MemoryManager] Total Allocated: 196560 Bytes
[2025-12-26 14:10:36]  -> StructID 1 (QUADTREE): 196560 Bytes in 1365 allocations
...
[2025-12-26 14:11:06] ----- Benchmark von QUADTREE fuer Insert-Operationen -----
[2025-12-26 14:11:06] Longest Time: 0.015500 ms
[2025-12-26 14:11:06] Shortest Time: 0.000500 ms
[2025-12-26 14:11:06] Average Time: 0.002378 ms
[2025-12-26 14:11:06] Found: 500
...
```

---

### Wichtig

- **`Comparing.txt`**: enthält hauptsächlich die **finalen Ergebnisse** + **BruteForce-Vergleich** (einmal pro Run am Ende).  
- **`benchmarks/Name_der_Datenstruktur.txt`**: enthält **laufende Snapshots** (ca. jede 1 Sekunde), um den Verlauf während der Szene zu sehen.

---

## Testszenen

Im Ordner **`Testszenen/`** findest du die von mir verwendeten, bereits definierten Szenen. Damit kannst du die Tests **einfach reproduzieren**, indem du diese Szenen in deinen `Scene/`-Ordner kopierst und anschließend wie gewohnt per CMD lädst.

> **Achtung:** Für eine **Reproduktion** musst du auf die **Szenengröße** achten.  
> Die Szenengröße entspricht immer den Werten aus `window.txt` (**WIDTH** und **HEIGHT**). Um exakt die gleichen Szenen wie in meiner Arbeit zu erhalten, solltest du in der Arbeit nachschauen, welche Szenengröße verwendet wurde, und diese Werte in `window.txt` übernehmen.  
> Da `window.txt` beim Start eingelesen wird, muss das Programm für unterschiedliche Szenengrößen **neu gestartet** werden.

---

## Lizenz

*(Hier deine Lizenz eintragen, z. B. MIT / Apache-2.0 / Proprietary)*





