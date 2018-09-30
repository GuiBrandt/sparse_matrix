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
 * @brief Classe de comparador para apenas a chave num std::pair
 * 
 * @tparam T Tipo do pair
 */
template < class T > struct compare_key {
private:
    T::first_type value;

public:
    /**
     * @brief Construtor
     * 
     * @param val Valor esperado da chave
     */
    compare_key(T::first_type val) : val_(val) {}

    /**
     * @brief Executa a comparação
     * 
     * @param pair Par de valores
     * @return true se a chave for igual à esperada
     * @return false se não
     */
    bool operator()(const T& pair) const {
        return val_ == pair.first;
    }
};

/**
 * @brief Classe de matriz esparsa
 * 
 * @tparam T Tipo de valor armazenado na matriz
 * @tparam N Linhas na matriz
 * @tparam M Colunas na matriz
 */
template < class T, unsigned int N, unsigned int M > class sparse_matrix {
private:

    typedef std::pair<size_t, T> column;
    typedef avl_tree<column, std::less<column>, compare_key<column>> column_tree;

    typedef std::pair<size_t, column_tree*> row;

    avl_tree<row, std::less<row>, compare_key<row>> rows;   //! Árvore interna
    T _default;                                             //! Valor padrão 

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
            _value = other.value;

            return *this;
        }

        /**
         * @brief Define o valor da célula
         * 
         * @param value Valor a ser atribuido à célula
         * @return cell& Esse objeto
         */
        cell & operator=(const T & value) {
            _value = value;

            row r(_row, nullptr);
            col c(_col, _value);

            if (!_matrix->rows.includes(r)) {
                columns_tree* ct = new column_tree();
                ct->insert(c);

                r.second = ct;

                _matrix->rows.insert(r);

            } else
                r.second->update(c);

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

            if (_matrix->rows.includes(r)) {
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

            if (_matrix->rows.includes(r)) {
                column_tree* cols = r.second;

                column col;
                col.first = _col;
                
                if (cols->includes(col))
                    return &col.second;
            }
            
            return &_matrix->_default;
        }
    }

    /**
     * @brief Obtém o valor em uma determinada posição da matriz
     * 
     * @param n Linha na matriz
     * @param m Coluna na matriz
     * @return cell A célula naquela posição da matriz
     */
    cell operator[](size_t n, size_t m) const {
        return cell(this, n, m);
    }
};