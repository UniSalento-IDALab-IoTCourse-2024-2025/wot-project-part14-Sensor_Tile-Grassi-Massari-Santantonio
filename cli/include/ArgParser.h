#pragma once
#include <vector>
#include <string>
#include <algorithm>

/**
 * @brief Classe helper per il parsing degli argomenti da linea di comando.
 * Gestisce l'estrazione di flag e parametri associati.
 */
class ArgParser {
public:
    ArgParser(int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }

    // Restituisce il valore associato a un'opzione (es. -f nomefile)
    const std::string& getCmdOption(const std::string &option) const {
        auto itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }

    // Verifica se un flag è presente
    bool cmdOptionExists(const std::string &option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

private:
    std::vector<std::string> tokens;
};