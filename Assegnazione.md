# Esonero di Laboratorio UDP - Reti di Calcolatori (ITPS A-L) 2025-26 :santa: :christmas_tree: :gift:

## Obiettivo Generale
**Migrare** l'applicazione client/server del servizio meteo dal protocollo **TCP** (realizzata nel primo esonero) al protocollo **UDP** (User Datagram Protocol).

L'obiettivo è modificare il codice già scritto per l'assegnazione TCP in modo da usare UDP come protocollo di trasporto, mantenendo invariato il protocollo applicativo (strutture dati, formati, funzionalità).

## Cosa Cambia nella Migrazione da TCP a UDP

### Modifiche da Apportare al Codice

Nel passaggio da TCP a UDP, dovrete modificare **solo il livello di trasporto**, lasciando invariato tutto il resto:

**DA MODIFICARE:**
- Tipo di socket
- Chiamate di sistema
- Output

**NON MODIFICARE:**
- Protocollo applicativo: strutture `struct request` e `struct response`
- Interfaccia a linea di comando (opzioni `-s`, `-p`, `-r`)
- Logica di business: parsing richieste, validazione città, generazione dati meteo
- Funzioni `get_temperature()`, `get_humidity()`, `get_wind()`, `get_pressure()`

## Protocollo Applicativo

**IMPORTANTE**: Il protocollo applicativo rimane **identico** all'assegnazione TCP.  
Le strutture dati definite in `protocol.h` **NON devono essere modificate**.

### Strutture Dati

**Richiesta Client:**
```c
struct request {
    char type;      // 't'=temperatura, 'h'=umidità, 'w'=vento, 'p'=pressione
    char city[64];  // nome città (null-terminated)
};
