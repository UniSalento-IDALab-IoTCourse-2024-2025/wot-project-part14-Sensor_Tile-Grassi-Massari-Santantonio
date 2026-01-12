# Specifiche Tecniche: Protocollo Dati e Parsing per ST SensorTile Box Pro

## 1. Introduzione
Il presente documento descrive le specifiche del protocollo di comunicazione dati rilevato durante l'interfacciamento con il dispositivo ST SensorTile Box Pro (firmware *FP-SNS-DATALOG2*). Viene illustrata la struttura dei pacchetti binari trasmessi via USB e la logica algoritmica implementata nel software di acquisizione (`cli_example`) per la corretta deserializzazione e interpretazione delle grandezze fisiche.

## 2. Architettura del Flusso Dati
Il dispositivo trasmette i dati sensoriali attraverso flussi binari distinti per ciascun sensore. L'analisi dei dump esadecimali ha evidenziato l'utilizzo di due formati di pacchetto differenti, la cui struttura varia in base alla tipologia di sensore (scalare a bassa frequenza vs vettoriale ad alta frequenza).

Tutti i dati multi-byte (Int16, Float, Double) sono trasmessi in formato **Little Endian**.

---

## 3. Formato A: Sensori Scalari (Temperatura, Pressione)
I sensori `stts22h_temp` (Temperatura) e `lps22df_press` (Pressione) utilizzano un protocollo a lunghezza fissa che include il timestamp generato dal dispositivo stesso.

### 3.1 Struttura del Pacchetto
La dimensione fissa del pacchetto è di **16 Byte**.

| Offset (Byte) | Tipo di Dato | Dimensione | Descrizione |
| :--- | :--- | :--- | :--- |
| **0x00 - 0x03** | `uint32_t` | 4 Byte | **Header / Counter.** Valore di allineamento o contatore progressivo (da scartare in fase di analisi). |
| **0x04 - 0x07** | `float` | 4 Byte | **Valore Misurato.** Rappresentazione in virgola mobile IEEE 754 del dato fisico (es. °C o hPa). |
| **0x08 - 0x0F** | `double` | 8 Byte | **Timestamp.** Riferimento temporale generato dal clock interno del microcontrollore. |

### 3.2 Note Interpretative
A differenza delle implementazioni standard dove il timestamp risiede nell'header (offset 0), in questo formato il riferimento temporale è posizionato in coda al pacchetto. Il valore numerico all'offset 0x04 è già convertito in unità ingegneristiche (float), pertanto non richiede moltiplicazioni per fattori di sensibilità.

---

## 4. Formato B: Sensori Vettoriali High-Speed (Accelerometro, Giroscopio, Magnetometro)
I sensori `lsm6dsv16x_acc`, `lsm6dsv16x_gyro` e `lis2mdl_mag` utilizzano un protocollo di trasmissione "bulk" ottimizzato per massimizzare il throughput.

### 4.1 Caratteristiche del Flusso
* **Timestamp:** Assente nel pacchetto dati.
* **Codifica Dati:** Raw Counts (Interi con segno a 16 bit).
* **Dimensione Pacchetto:** Variabile, dipendente dal riempimento del buffer USB (es. 2308 byte per accelerometro, 34 byte per magnetometro).

### 4.2 Struttura del Pacchetto
Ogni trasmissione è composta da un header di 4 byte seguito da una sequenza di campioni triassiali.

| Offset (Byte) | Tipo di Dato | Descrizione |
| :--- | :--- | :--- |
| **0x00 - 0x03** | `uint32_t` | **Header / Counter.** Byte di controllo da ignorare. |
| **0x04 - 0x09** | `int16_t` [3] | **Campione 1 (X, Y, Z).** Dati grezzi. |
| **0x0A - 0x0F** | `int16_t` [3] | **Campione 2 (X, Y, Z).** Dati grezzi. |
| ... | ... | ... |
| **N - 5 - N** | `int16_t` [3] | **Campione N (X, Y, Z).** Ultimo campione del blocco. |

### 4.3 Logica di Ricostruzione Temporale (Interpolazione)
Poiché il sensore non invia un timestamp per singolo campione, la coerenza temporale è ottenuta tramite interpolazione lineare lato host.

1.  Si registra il tempo di sistema ($T_{now}$) all'arrivo del pacchetto.
2.  Si recupera il tempo dell'ultimo pacchetto ricevuto ($T_{prev}$).
3.  Si calcola il numero di campioni ($N$) nel payload:
    $$N = \frac{\text{DimensioneTotale} - 4}{6}$$
4.  Si determina il passo temporale ($\Delta t$):
    $$\Delta t = \frac{T_{now} - T_{prev}}{N}$$
5.  Il timestamp ($t_i$) per l'i-esimo campione del blocco è calcolato come:
    $$t_i = T_{prev} + (i \times \Delta t)$$

---

## 5. Algoritmo di Parsing Implementato
Il modulo software `DataWriter` implementa una macchina a stati per la discriminazione automatica del formato dati in ingresso.

### 5.1 Procedura di Identificazione
L'algoritmo segue una logica gerarchica per prevenire ambiguità nella decodifica (notare che un pacchetto Temperatura da 16 byte potrebbe essere erroneamente interpretato come 2 campioni vettoriali da 6 byte + 4 header, se non prioritizzato).

1.  **Check Prioritario (Formato A):**
    * Se il nome del sensore contiene "temp" o "press":
        * Si verifica se `size % 16 == 0`.
        * In caso affermativo, si esegue il parsing secondo lo schema **[Header 4B] + [Float 4B] + [Timestamp 8B]**.

2.  **Check Secondario (Formato B):**
    * Se il sensore non è scalare, si verifica se il payload (al netto dell'header) è divisibile per 6:
        * `(size - 4) % 6 == 0`
    * In caso affermativo, si attiva la logica di **Interpolazione Temporale** e si leggono terne di `int16_t`.

### 5.2 Conversione Dati Raw
Per i sensori vettoriali, i valori salvati nel file JSON sono i conteggi grezzi del convertitore ADC (LSB). La conversione in unità fisiche (g, dps, gauss) è delegata alla fase di post-processing e deve applicare il fattore di sensibilità ($S$) presente nel file `acquisition_info.json`:

$$Valore_{fisico} = Valore_{raw} \times S$$

## 6. Conclusione
L'adozione di questa logica di parsing ibrida garantisce l'integrità dei dati per tutte le tipologie di sensori a bordo del SensorTile Box Pro, risolvendo le problematiche di disallineamento (NaN) e incoerenza temporale riscontrate nelle versioni precedenti del software.