/**
 * @brief Cabeçalho para a classe de matriz esparsa
 * 
 * @file sparse_matrix.hpp
 * @author Guilherme Brandt
 * @date 2018-09-30
 */

#include <utility>

#include <avl_tree.hpp>

/**
 * @brief Classe de matriz esparsa
 * 
 * @tparam T Tipo de valor armazenado na matriz
 */
template < class T > class sparse_matrix {
private:
    /**
     * @brief Classe de comparador para apenas a chave num std::pair
     * 
     * @tparam U Tipo do std::pair
     */
    template < class U > struct compare_key {
    public:
        /**
         * @brief Construtor
         * 
         * @param val Valor esperado da chave
         */
        compare_key() {}

        /**
         * @brief Executa a comparação
         * 
         * @param a Um par de valores do tipo especificado
         * @param b Outro par se valores
         * @return true se a chave de `a` for igual à de `b`
         * @return false se não
         */
        bool operator()(const U& a, const U&b) const {
            return a.first == b.first;
        }
    };

    typedef std::pair<size_t, T> column;
    typedef avl_tree<column, std::less<column>, compare_key<column>> column_tree;

    typedef std::pair<size_t, column_tree*> row;

    avl_tree<row, std::less<row>, compare_key<row>> row_tree;   //! Árvore interna
    T _default;                                                 //! Valor padrão 

public:
    /**
     * @brief Construtor
     */
    sparse_matrix(T def) : _default(def) {}

    /**
     * @brief Classe de célula na matriz
     */
    class cell {
    friend class sparse_matrix;

    private:
        sparse_matrix* _matrix; //! Matriz à qual a célula pertence
        size_t _col, _row;      //! Linha e coluna da célula
        
        cell(sparse_matrix* matrix, size_t row, size_t col) : 
            _matrix(matrix), _col(col), _row(row) {}

    public:

        /**
         * @brief Construtor de cópia
         * 
         * @param other Objeto modelo
         * @return cell& Esse objeto
         */
        cell & operator=(const cell & other) {
            _matrix = other._matrix;
            _col = other._col;
            _row = other._row;

            return *this;
        }

        /**
         * @brief Define o valor da célula
         * 
         * @param value Valor a ser atribuido à célula
         * @return cell& Esse objeto
         */
        cell & operator=(const T & value) {
            row r(_row, nullptr);
            column c(_col, value);

            if (value == _matrix->_default) {
                if (_matrix->row_tree.includes(r)) {
                    if (r.second->includes(c))
                        r.second->remove(c);

                    if (r.second->empty()) {
                        delete r.second;
                        _matrix->row_tree.remove(r);
                    }
                }
            } else {
                if (!_matrix->row_tree.includes(r)) {
                    column_tree* ct = new column_tree();
                    ct->insert(c);

                    r.second = ct;

                    _matrix->row_tree.insert(r);

                } else
                    r.second->update(c);
            }

            return *this;
        }

        /**
         * @brief Operador de derreferenciação
         * 
         * @return T Valor da célula
         */
        T operator*() const {
            row r;
            r.first = _row;

            if (_matrix->row_tree.includes(r)) {
                column_tree* cols = r.second;

                column col;
                col.first = _col;
                
                if (cols->includes(col))
                    return col.second;
            }
            
            return _matrix->_default;
        }
        
        /**
         * @brief Operador de derreferenciação
         * 
         * @return T Valor da célula
         */
        T* operator->() {
            row r;
            r.first = _row;

            if (_matrix->row_tree.includes(r)) {
                column_tree* cols = r.second;

                column col;
                col.first = _col;
                
                if (cols->includes(col))
                    return &col.second;
            }
            
            return &_matrix->_default;
        }
    };

    /**
     * @brief Obtém o valor em uma determinada posição da matriz
     * 
     * @param n Linha na matriz
     * @param m Coluna na matriz
     * @return cell A célula naquela posição da matriz
     */
    cell at(size_t n, size_t m) {
        return cell(this, n, m);
    }
};