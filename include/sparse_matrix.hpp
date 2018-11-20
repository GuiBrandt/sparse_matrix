/**
 * @brief Cabeçalho para a classe de matriz esparsa
 * 
 * @file sparse_matrix.hpp
 * @author Guilherme Brandt
 * @date 2018-09-30
 */

#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <functional>
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
        compare_key() = default;

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

    typedef std::pair<size_t, T> column_t;

    typedef avl_tree<column_t, std::less<column_t>, compare_key<column_t>> column_tree;

    typedef std::pair<size_t, column_tree*> row_t;

    avl_tree<row_t, std::less<row_t>, compare_key<row_t>> row_tree;     //! Árvore interna
    T _default;                                                         //! Valor padrão

public:

    /**
     * @brief Construtor
     */
    sparse_matrix(T def) : _default(def) {}

    /**
     * @brief Construtor de cópia
     * 
     * @param other Matriz a ser copiada
     */
    sparse_matrix(const sparse_matrix & other) {
        row_tree = other.row_tree;
        _default = other._default;
    }

    class row;

    /**
     * @brief Classe de célula na matriz
     */
    class cell {
        friend class sparse_matrix;
        friend class row;

    private:
        sparse_matrix* _matrix; //! Matriz à qual a célula pertence
        size_t _col, _row;      //! Linha e coluna da célula
        
        /**
         * @brief Construtor
         * 
         * @param matrix Matriz "dona" da célula
         * @param row Linha
         * @param col Coluna
         */
        cell(sparse_matrix* matrix, size_t row, size_t col) : 
            _matrix(matrix), _col(col), _row(row) {}

    public:

        /**
         * @brief Construtor de cópia
         * 
         * @param model Objeto modelo
         */
        cell(const cell& model) {
            *this = model;
        }

        /**
         * @brief Operador de atribuição
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
            row_t r(_row, nullptr);
            column_t c(_col, value);

            if (value == _matrix->_default) {
                if (_matrix->row_tree.find(r)) {
                    if (r.second->includes(c))
                        r.second->remove(c);

                    if (r.second->empty()) {
                        delete r.second;
                        _matrix->row_tree.remove(r);
                    }
                }

            } else {
                if (!_matrix->row_tree.find(r)) {
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
         * @brief Operador de swap
         * 
         * @param a Um célula
         * @param b Outra célula
         */
        friend void swap(cell & a, cell & b) {
            using std::swap;

            swap(a._matrix, b._matrix);
            swap(a._row, b._row);
            swap(a._col, b._col);
        }

        /**
         * @brief Operador de derreferenciação
         * 
         * @return T Valor da célula
         */
        T operator*() const {
            row_t r;
            r.first = _row;

            if (_matrix->row_tree.find(r)) {
                column_tree* cols = r.second;

                column_t col;
                col.first = _col;
                
                if (cols->find(col))
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
            row_t r;
            r.first = _row;

            if (_matrix->row_tree.find(r)) {
                column_tree* cols = r.second;

                column_t col;
                col.first = _col;
                
                if (cols->find(col))
                    return &col.second;
            }
            
            return &_matrix->_default;
        }

        /**
         * @brief Escreve uma célula em uma stream de saída
         * 
         * @param c Célula
         * @return std::ostream& Stream de saída
         */
        friend std::ostream & operator<<(std::ostream & os, const cell & c) {
            os << *c;
            return os;
        }
    };

    /**
     * @brief Classe de linha na matriz
     */
    class row {
        friend class sparse_matrix;

        private:
            sparse_matrix* _matrix; //! Matriz à qual a célula pertence
            size_t _row;            //! Número da linha na matriz

            /**
             * @brief Construtor
             * 
             * @param matrix Matriz "dona" da linha
             * @param row Linha
             */
            row(sparse_matrix* matrix, size_t row) : _matrix(matrix), _row(row) {}

        public:
            /**
             * @brief Construtor de cópia
             * 
             * @param model Objeto modelo
             */
            row(const row & model) {
                *this = model;
            }

            /**
             * @brief Operador de atribuição
             * 
             * @param other Objeto modelo
             * @return cell& Esse objeto
             */
            row & operator=(const row & other) {
                _matrix = other._matrix;
                _row = other._row;

                return *this;
            }

            /**
             * @brief Obtém uma célula em uma determinada coluna da linha da matriz
             * 
             * @param row Coluna na linha da matriz
             * @return cell A célula naquela posição da matriz
             */
            cell operator[](size_t col) {
                return cell(_matrix, _row, col);
            }
    };

    /**
     * @brief Obtém uma linha da matriz
     * 
     * @param r Linha na matriz
     * @return cell A célula naquela posição da matriz
     */
    row operator[](size_t r) {
        return row(this, r);
    }
};

#endif // SPARSE_MATRIX_HPP