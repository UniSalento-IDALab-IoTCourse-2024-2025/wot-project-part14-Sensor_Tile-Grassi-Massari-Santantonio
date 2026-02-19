# FastGo - Piattaforma di Delivery IoT & Blockchain

![Architettura del Sistema](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-2024-2025-part1-Grassi-Massari-Santantonio/blob/main/Arc_semplice3.jpeg?raw=true)

## Panoramica del Progetto
FastGo è una piattaforma logistica e di food delivery di nuova generazione che integra tecnologie Internet of Things (IoT), Machine Learning e Blockchain per garantire l'integrità delle merci trasportate. A differenza dei servizi di consegna tradizionali, che si limitano a tracciare la posizione geografica del pacco, FastGo sposta il focus sulla **garanzia della qualità** del trasporto. Il sistema monitora attivamente le condizioni fisiche della spedizione (vibrazioni, urti, orientamento e temperatura) durante l'intero processo, utilizzando sensori embedded. Questi dati telemetrici vengono analizzati per calcolare un "Damage Score" (Punteggio di Danno), assicurando che i clienti ricevano i loro ordini in condizioni ottimali e fornendo prove tangibili in caso di deterioramento della merce.

Oltre al monitoraggio, la piattaforma introduce un innovativo livello di gamification trasparente: i corrieri (Rider) vengono valutati non tramite recensioni soggettive, ma sulla base della qualità oggettiva della loro guida e della cura nel trasporto. L'eccellenza operativa viene premiata attraverso certificati digitali immutabili (NFT) coniati sulla blockchain di Ethereum. Questo crea un sistema di reputazione "trustless", dove i Rider possono dimostrare professionalmente le proprie competenze e accedere a livelli di servizio superiori, mentre i commercianti e i clienti ottengono una trasparenza senza precedenti sulla filiera distributiva.

## Architettura del Sistema
L'ecosistema FastGo è costruito su una **Architettura a Microservizi** modulare, progettata per garantire scalabilità, tolleranza ai guasti e una netta separazione delle responsabilità. L'infrastruttura backend è composta da cinque servizi core sviluppati in **Spring Boot** (Auth, Client, Rider, Shop e Blockchain), ognuno dei quali gestisce il proprio database **MongoDB** dedicato, aderendo rigorosamente al pattern architetturale *Database-per-Service* per assicurare il disaccoppiamento dei dati.

La comunicazione tra i servizi sfrutta un approccio di messaggistica ibrido e resiliente:
1.  **RabbitMQ (AMQP):** Gestisce la sincronizzazione asincrona dei dati tra i microservizi e l'orchestrazione dei processi in stile RPC (Remote Procedure Call), garantendo la coerenza eventuale dell'intero sistema distribuito anche in caso di picchi di carico.
2.  **Mosquitto (MQTT):** Gestisce i flussi di dati provenienti dai dispositivi IoT e invia aggiornamenti di stato in tempo reale alle interfacce frontend tramite WebSockets, permettendo un tracking fluido e reattivo.

Il livello IoT è costituito dal dispositivo **ST Microelectronics SensorTile Box Pro**, controllato da un **RaspberryPi 5** tramite un firmware custom in C++. Questi dispositivi operano nell'edge, acquisendo dati ambientali e inerziali che vengono trasmessi via Bluetooth Low Energy (BLE) all'applicazione mobile del Rider. I dati grezzi vengono poi processati da un **Motore di Inferenza dedicato in Python** a bordo dello stesso RaspberryPi 5, che utilizza un modello Random Forest pre-addestrato per classificare eventi critici di trasporto (come cadute, impatti o ribaltamenti) e calcolare le metriche di stabilità termica.

Infine, il **Web3 Gateway** agisce come ponte sicuro verso la tecnologia decentralizzata, interagendo con la testnet **Ethereum Sepolia** tramite la libreria **Web3j**. Questo modulo gestisce l'esecuzione degli Smart Contracts per la notarizzazione degli ordini (rendendo i log di consegna immutabili) e per la distribuzione dei Badge ERC-721, i cui metadati sono ancorati in modo permanente su **IPFS** tramite Pinata. L'esperienza utente è erogata attraverso una dashboard web in React per i clienti, i rider e i negozianti, e un'applicazione mobile cross-platform in React Native che permette ai clienti di ordinare e ai rider di gestire le consegne.

### Schema Tecnico dei Flussi Dati
![Diagramma Tecnico Microservizi e IoT](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-2024-2025-part1-Grassi-Massari-Santantonio/blob/main/Archittetura.png?raw=true)


## Ecosistema FastGo - Progetti Correlati

Di seguito la lista completa dei repository che compongono il sistema IoT FastGo.

### Backend & Infrastruttura
* [**Auth Service**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part1-Auth_Service-Grassi-Massari-Santantonio) - Gestisce registrazione, login (JWT) e sincronizzazione utenti.
* [**Client Service**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part2-Client_Service-Grassi-Massari-Santantonio) - Gestisce i profili dei clienti e lo storico ordini.
* [**Rider Service**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part3-Rider_Service-Grassi-Massari-Santantonio) - Gestisce i corrieri, la geolocalizzazione e l'assegnazione ordini.
* [**Shop Service**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part5-Shop_Service-Grassi-Massari-Santantonio) - Gestisce i ristoranti, i menu e il ciclo di vita dell'ordine.
* [**Message Broker**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part4-Message_Broker-Grassi-Massari-Santantonio) - Configurazione Docker per RabbitMQ e Mosquitto (MQTT).

### Frontend & Mobile
* [**Frontend Web**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part8-Frontend-Grassi-Massari-Santantonio) - Dashboard React per Amministratori, Ristoratori e Clienti.
* [**App Rider**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part15-App_Rider-rassi-Massari-Santantonio) - App mobile per corrieri con connessione BLE al sensore e gestione consegne.
* [**App User**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part15-App_Rider-Grassi-Massari-Santantonio) - App mobile per clienti per ordinare e tracciare le consegne in tempo reale.

### IoT, AI & Sensori
* [**Sensor Tile Firmware**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part14-Sensor_Tile-Grassi-Massari-Santantonio) - Codice C++ per l'acquisizione dati dal dispositivo SensorTile Box Pro.
* [**Bluetooth Gateway**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part11-Bluetooth-Grassi-Massari-Santantonio) - Servizio Python per interfacciare il sensore BLE con il cloud tramite MQTT.
* [**Inference Engine**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part12-Inference-Grassi-Massari-Santantonio) - Modulo di analisi dati per valutare la qualità del trasporto.
* [**AI Training**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part13-Training-Grassi-Massari-Santantonio) - Script per la generazione del dataset e l'addestramento del modello ML.

### Blockchain & Web3
* [**Blockchain Service**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part7-BlockchainService-Grassi-Massari-Santantonio) - Gateway Java/Web3j per la notarizzazione e gestione NFT.
* [**Smart Contract: Tracking**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part10-Contract_Blockchain-Grassi-Massari-Santantonio) - Contratto Solidity per la registrazione immutabile degli ordini.
* [**Smart Contract: NFT**](https://github.com/UniSalento-IDALab-IoTCourse-2024-2025/wot-project-part9-Contract_NFT-Grassi-Massari-Santantonio) - Contratto ERC-721 per la gestione dei Badge premio per i rider.

[**Project Presentation Page**](https://unisalento-idalab-iotcourse-2024-2025.github.io/wot-project-2024-2025-part1-Grassi-Massari-Santantonio/)



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

