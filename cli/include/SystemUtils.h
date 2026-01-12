#pragma once
#include <string>
#include <atomic>

// Flag  per la gestione sicura dell'interruzione (SIGINT/SIGTERM) tra thread
extern volatile std::atomic<bool> g_exit_requested;

namespace SystemUtils {

    /**
     * @brief Registra i gestori per SIGINT e SIGTERM.
     */
    void setupSignalHandler();

    /**
     * @brief Controlla se un tasto è stato premuto (non-blocking).
     * @param c Puntatore dove salvare il carattere premuto.
     * @return true se un tasto è stato premuto, false altrimenti.
     */
    bool getKeyboardInput(char *c);

    /**
     * @brief Genera una stringa timestamp corrente formattata.
     * @return Stringa nel formato YYYYMMDD_HH_MM_SS
     */
    std::string getCurrentTimestampString();

    /**
     * @brief Crea una directory con i permessi appropriati (Cross-platform).
     */
    bool createDirectory(const std::string& path);

    /**
     * @brief Sleep cross-platform in millisecondi.
     */
    void sleepMs(int milliseconds);
}