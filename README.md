# FastGo SensorTile CLI

Questo progetto contiene il software di interfaccia C++ per il dispositivo ST SensorTile Box Pro. L'applicazione utilizza la libreria ST High Speed Datalog (libhs_datalog) per configurare il sensore, caricare modelli di Machine Learning (MLC) e acquisire dati ad alta frequenza via USB.

## Struttura del Progetto

.
├── CMakeLists.txt       # Configurazione di build CMake
├── include/             # Header files (.h)
├── src/                 # Sorgenti C++ (.cpp)
├── lib/                 # Librerie esterne (libhs_datalog, nlohmann json)
└── build/               # Directory di compilazione (generata)

## Prerequisiti

* Compilatore C++ compatibile con C++11 o superiore (g++, clang)
* CMake (versione 3.10 o superiore)
* Librerie di sistema standard

## Compilazione

Eseguire i seguenti comandi dalla cartella `cli`:

1. Creare la directory di build:
   mkdir -p build
   cd build

2. Generare i file di make:
   cmake ..

3. Compilare l'eseguibile:
   make

L'eseguibile generato si chiamerà `cli_example`.

## Utilizzo

Sintassi base:
./cli_example [OPZIONI]

### Opzioni Disponibili

* -h
  Mostra il messaggio di aiuto.

* -g
  Esporta la configurazione corrente del dispositivo nel file `device_config.json` e termina. Utile per ottenere un template di configurazione.

* -f <file_config.json>
  Carica una configurazione specifica sul dispositivo (formato JSON ST). Se non specificato, viene mantenuta la configurazione attuale.

* -u <file_modello.ucf>
  Carica un file di configurazione UCF (Unicoico Configuration File) per programmare il Machine Learning Core (MLC) del sensore ISM330DHCX.

* -t <secondi>
  Imposta un timeout di acquisizione in secondi. Se non specificato, l'acquisizione continua finché non viene premuto 'q' o ESC.

### Esempi di utilizzo

1. Acquisizione semplice (stop manuale con 'q'):
   ./cli_example

2. Acquisizione di 10 secondi con configurazione specifica:
   ./cli_example -f my_config.json -t 10

3. Caricamento di un modello MLC e acquisizione:
   ./cli_example -u modello_caduta.ucf

4. Esportazione della configurazione attuale:
   ./cli_example -g

## Output dei Dati

Al termine dell'esecuzione, il software crea una cartella rinominata con il timestamp corrente (es. `20250205_15_30_00`).

Contenuto della cartella:
* acquisition_info.json: Metadati dell'acquisizione e configurazione finale del sensore.
* configuration.ucf: Copia del file UCF caricato (se presente).
* <nome_sensore>.json: File contenenti i dati acquisiti (Accelerometro, Giroscopio, etc.).

### Formato JSON Dati
I file dei sensori contengono un array di oggetti JSON:

[
  { "timestamp": 1234.5678, "x": 0.12, "y": -0.98, "z": 0.05 },
  { "timestamp": 1234.5688, "x": 0.13, "y": -0.99, "z": 0.04 }
]

Nota: I dati di temperatura e pressione utilizzano il campo "value" invece di x, y, z.

## Risoluzione Problemi

* "No devices found": Assicurarsi che il SensorTile Box Pro sia collegato via USB e che l'utente abbia i permessi di lettura/scrittura sulla porta seriale/USB (spesso richiede l'aggiunta dell'utente al gruppo `dialout` o `plugdev`).
* "Failed to initialize datalog library": Verificare che le librerie dinamiche in `lib/` siano accessibili o correttamente linkate.
