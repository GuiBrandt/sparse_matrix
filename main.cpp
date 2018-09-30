#include <algorithm>
#include <iostream>
#include <string>
#include <regex>

#include <sparse_matrix.hpp>

using namespace std;
using namespace regex_constants;

static const regex SET(R"(^\s*(\d+)\s*,\s*(\d+)\s*=\s*(\d+)\s*$)", icase | optimize);
static const regex PRINT(R"(^\s*(?:p|print)\s*(\d+)\s*,\s*(\d+)\s*$)", icase | optimize);
static const regex QUIT(R"(^\s*(?:q|quit|exit)\s*$)", icase | optimize);

/**
 * @brief Ponto de entrada
 * 
 * @param argc Número de argumentos da linha de comando
 * @param argv Valores dos argumentos da linha de comando
 * @return int Código de retorno
 */
int main(int argc, char** argv) {
    string line;
    smatch m;

    sparse_matrix<int, 1000, 1000> matrix(0);

    // Cabeçalho
    cout << "Interactive Sparse Matrix" << endl;
    cout << endl;

    cout << "n,m=x          : Set (N,M) to be X" << endl;
    cout << "p|print n,m    : Print (N,M) out" << endl;
    cout << "q|e|quit|exit  : Quit" << endl;
    cout << endl;

    cout << "Have fun!" << endl;

    for (;;) {

        cout << "matrix (" << decltype(matrix)::rows << ", " << decltype(matrix)::columns << ")> ";
        getline(cin, line);

        // Encerra o programa
        if (regex_match(line, QUIT)) {
            break;

        // Define o valor de uma posição na matriz
        } else if (regex_search(line, m, SET)) {
            try {
                int r = stoi(m[1]),
                    c = stoi(m[2]),
                    x = stoi(m[3]);

                matrix.at(r, c) = x;
            } catch (const char* msg) {
                cerr << "Err: " << msg << endl;
            }

        // Escreve o valor de uma posição da matriz na tela
        } else if (regex_search(line, m, PRINT)) {
            int r = stoi(m[1]),
                c = stoi(m[2]);

            cout << *matrix.at(r, c) << endl;

        // Comando inválido
        } else {
            cerr << "Err: Invalid command" << endl;
            continue;
        }
    }

    return 0;
}