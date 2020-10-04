#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

/* ----- FORWARD DECLARATIONS ----- */

template<typename, typename> class Matrix;
template<typename, typename> class RowIterator;
template<typename, typename> class ConstRowIterator;

/* ----- PRINT OPERATOR ----- */

template<typename T, typename Container>
std::ostream& operator<<(std::ostream&, const Matrix<T,Container>&);

/* ----- OPERATORS ----- */

// return the matrix multiplication.
// The container type used for the returned matrix is a shared_ptr.
template<typename T, typename Container_2>
Matrix<T, std::shared_ptr<T>> operator*(const Matrix<T, std::shared_ptr<T>>&, const Matrix<T, Container_2>&);


// return the matrix multiplication.
// The container type used for the returned matrix is the same type as the one used for the first matrix.
template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator*(const Matrix<T, Container_1>&, const Matrix<T, Container_2>&);

// return the matrix multiplication.
// The container type used for the returned matrix is the same type as the one used for the first matrix but non pointer.
template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator*(const Matrix<T, Container_1*>&, const Matrix<T, Container_2>&);

template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator+(const Matrix<T, Container_1>&, const Matrix<T, Container_2>&);

template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator-(const Matrix<T, Container_1>&, const Matrix<T, Container_2>&);

template<typename T, typename Container>
Matrix<T, Container> operator*(const Matrix<T, Container>&, T);
template<typename T, typename Container>
Matrix<T, Container> operator*(T, const Matrix<T, Container>&);
template<typename T, typename Container>
Matrix<T, Container> operator/(const Matrix<T, Container>&, T);

/* ----- OTHER OPERATIONS ----- */

template<typename T, typename Container1, typename Container2>
T dot(const Matrix<T, Container1>&, const Matrix<T, Container2>&);

/* ----- MATRIX ----- */

template<typename T, typename Container=std::shared_ptr<T>>
class Matrix{
public:
    Matrix(size_t, size_t);
    Matrix(std::initializer_list<std::initializer_list<T>>);

    template<typename Container_row>
    explicit Matrix(size_t rows, size_t columns, const Container_row &row);

    ~Matrix();

    Matrix(const Matrix&);
    Matrix(Matrix&&) noexcept;

    Matrix& operator=(const Matrix &);
//    Matrix& operator=(Matrix&&) noexcept;

    /* ----- DIMENTIONS ----- */

    //gets total size.
    size_t size() const{
        return _rows*_columns;
    }

    //gets number of rows.
    size_t rows() const{
        //return (_isTransposed^1) *RowsN + (_isTransposed) *ColumnsN;
        return _rows;
    }

    //gets number of columns.
    size_t columns() const{
        //return (_isTransposed^1) *ColumnsN + (_isTransposed) *RowsN;
        return _columns;
    }

    /* ----- TRANSPOSE ----- */

    Matrix<T,Container> transposed() const;

    /* ----- GETTERS & SETTERS ----- */

    // returns a reference to the element in row 'row', column 'column'.
    // if the element does not exist it may be created depending of the Container used for the row.
    // For example, if the container is a 'map', and the value at row,column does not exist, it will be created.
    // if you are using a 'map' container and you want to retrieve a copy of the value but you don't want to create it if it does not exist
    // use 'retrieveAt' instead.
    T& at(size_t row, size_t column){
        //auto actualRowColumn = actualRowColumnIndex(row, column);
        //return _mat.get()[actualRowColumn.first][actualRowColumn.second];
        return atRowIndex(_mat.get()[row],column);
    }

    // returns a copy of the element in row 'row', column 'column'.
    // if the Container is a structure like a 'map',
    // this method will not create an element in position (row,column) if there isn't one in that position.
    // This is usefull for sparce matrix implementations.
    T retrieveAt(size_t row, size_t column) const;

    void insertValueAtRowColumn(const T& value, size_t row, size_t column);

    // returns a matrix representing a copy of the row in the specified index.
    // The container of the returned matrix is the same as the one used in the original one.
    Matrix<T,Container> copyRowAtIndex(size_t index) const{
        return Matrix<T,Container>(1,_columns,_mat.get()[index]);
    }

    // returns a matrix representing a 'reference' to the row in the specified index.
    // The container of the returned matrix is a pointer to the container used in the original one.
    Matrix<T,Container*> rowAtIndex(size_t index) & {
        return Matrix<T,Container*>(1,_columns,&(_mat.get()[index]));
    }

    /* ----- HELPERS ----- */

    // returns the number of actually stored elements
    // usefull for testing when implementing sparce matrix using 'map' as container.
    size_t storedElementsCount() const{
        size_t elements = 0;

        auto end = _mat.get() + _rows;
        size_t row = 0;
        for(auto rowsIt = _mat.get(); rowsIt != end; ++rowsIt){
            for(auto columnsIt = rowIteratorBegin(row); columnsIt != rowIteratorEnd(row); ++columnsIt){
                ++elements;
            }
            ++row;
        }

        return elements;
    }

    /* ----- ITERATORS OPERATIONS ----- */

    //returns a ConstRowIterator for the begining of the row at 'rowIndex'
    ConstRowIterator<T, Container> rowIteratorBegin(size_t rowIndex) const{
        return ConstRowIterator<T, Container>::begin(&(_mat.get()[rowIndex]));
    }

    //returns a RowIterator for the begining of the row at 'rowIndex'
    RowIterator<T, Container> rowIteratorBegin(size_t rowIndex){
        return RowIterator<T, Container>::begin(&(_mat.get()[rowIndex]));
    }

    //returns a ConstRowIterator for the ending of the row at 'rowIndex'
    ConstRowIterator<T, Container> rowIteratorEnd(size_t rowIndex) const{
        return ConstRowIterator<T, Container>::end(&(_mat.get()[rowIndex]), _columns);
    }

    //returns a RowIterator for the ending of the row at 'rowIndex'
    RowIterator<T, Container> rowIteratorEnd(size_t rowIndex){
        return RowIterator<T, Container>::end(&(_mat.get()[rowIndex]), _columns);
    }

    /* ----- FRIENDS ----- */

    /* ----- OPERATORS ----- */

    template<typename Container2>
    Matrix<T, Container>& operator+=(const Matrix<T,Container2>&);

    template<typename Container2>
    Matrix<T, Container>& operator-=(const Matrix<T,Container2>&);

    Matrix<T, Container>& operator*=(const T&);
    Matrix<T, Container>& operator/=(const T&);

    // returns a reference to this matrix with the negative of each element
    // used for rvalues
    Matrix<T, Container>& operator-() &&{
        return (*this)*=(-1);
    }

    // returns a copy of the matrix with the negative of each element
    // used for lvalues
    Matrix<T, Container> operator-() const &{
        Matrix<T, Container>ret(*this);
        return ret*=(-1);
    }

    //Vector operations

private:
    Matrix() = delete;
    /* ----- UTILITIES ----- */

    T& atRowIndex(std::shared_ptr<T> &row , size_t index){
        return row.get()[index];
    }
    T& atRowIndex(std::map<size_t,T>& row, size_t index){
        return row[index];
    }
    T getValueAtIndex(const std::shared_ptr<const T>, size_t) const;
    T getValueAtIndex(const std::map<size_t,T>&, size_t) const;
    void insertValueAtRowIndex(const T&, std::shared_ptr<T>, size_t);
    void insertValueAtRowIndex(const T&, std::map<size_t, T>&, size_t);

    void resetRow(std::shared_ptr<T>&, size_t);
    void resetRow(std::map<size_t, T>&, size_t);

    void copyRow(Container &dest, const std::shared_ptr<const T> src, size_t);
    void copyRow(Container &dest, const std::map<size_t, T> &src, size_t);

    /* ----- MEMBERS ----- */

    size_t _rows;
    size_t _columns;
    std::unique_ptr<Container[]> _mat;
};

/* ----- MATRIX SPECIALIZATION ----- */

template<typename T, typename Container>
class Matrix<T, Container*>{
public:

    template<typename Container_row>
    explicit Matrix(size_t rows, size_t columns, Container_row row);

    Matrix(Matrix &&) noexcept;
    Matrix& operator=(Matrix &&) noexcept;

    /* ----- DIMENTIONS ----- */

    //gets total size.
    size_t size() const{
        return _rows*_columns;
    }

    //gets number of rows.
    size_t rows() const{
        return _rows;
    }

    //gets number of columns.
    size_t columns() const{
        return _columns;
    }

    /* ----- GETTERS & SETTERS ----- */

    // returns a reference to the element in row 'row', column 'column'.
    // if the element does not exist it may be created depending of the Container used for the row.
    // For example, if the container is a 'map*', and the value at row,column does not exist, it will be created.
    // if you are using a 'map*' container and you want to retrieve a copy of the value but you don't want to create it if it does not exist
    // use 'retrieveAt' instead.
    T& at(size_t row, size_t column){
        return atRowIndex(*(_mat.get()[row]),column);
    }

    // returns a copy of the element in row 'row', column 'column'.
    // if the Container is a structure like a 'map*',
    // this method will not create an element in position (row,column) if there isn't one in that position.
    // This is usefull for sparce matrix implementations.
    T retrieveAt(size_t row, size_t column) const;

    void insertValueAtRowColumn(const T& value, size_t row, size_t column);

    /* ----- HELPERS ----- */

    // returns the number of actually stored elements
    // usefull for testing when implementing sparce matrix using 'map' as container.
    size_t storedElementsCount() const{
        size_t elements = 0;

        auto end = _mat.get() + _rows;
        size_t row = 0;
        for(auto rowsIt = _mat.get(); rowsIt != end; ++rowsIt){
            for(auto columnsIt = rowIteratorBegin(row); columnsIt != rowIteratorEnd(row); ++columnsIt){
                ++elements;
            }
            ++row;
        }

        return elements;
    }

    /* ----- ITERATORS OPERATIONS ----- */

    //returns a ConstRowIterator for the begining of the row at 'rowIndex'
    ConstRowIterator<T, Container> rowIteratorBegin(size_t rowIndex) const{
        return ConstRowIterator<T, Container>::begin(_mat.get()[rowIndex]);
    }

    //returns a RowIterator for the begining of the row at 'rowIndex'
    RowIterator<T, Container> rowIteratorBegin(size_t rowIndex){
        return RowIterator<T, Container>::begin(_mat.get()[rowIndex]);
    }

    //returns a ConstRowIterator for the ending of the row at 'rowIndex'
    ConstRowIterator<T, Container> rowIteratorEnd(size_t rowIndex) const{
        return ConstRowIterator<T, Container>::end(_mat.get()[rowIndex], _columns);
    }

    //returns a RowIterator for the ending of the row at 'rowIndex'
    RowIterator<T, Container> rowIteratorEnd(size_t rowIndex){
        return RowIterator<T, Container>::end(_mat.get()[rowIndex], _columns);
    }

    /* ----- OPERATORS ----- */

    template<typename Container2>
    Matrix<T, Container*>& operator+=(const Matrix<T,Container2>&);
    template<typename Container2>
    Matrix<T, Container*>& operator-=(const Matrix<T,Container2>&);
    Matrix<T, Container*>& operator*=(const T&);
    Matrix<T, Container*>& operator/=(const T&);

private:
    Matrix() = delete;
    Matrix(const Matrix &) = delete;
    Matrix& operator=(const Matrix &) = delete;
    /* ----- UTILITIES ----- */

    T& atRowIndex(std::shared_ptr<T> &row , size_t index){
        return row.get()[index];
    }
    T& atRowIndex(std::map<size_t,T>& row, size_t index){
        return row[index];
    }
    T getValueAtIndex(const std::shared_ptr<const T>, size_t) const;
    T getValueAtIndex(const std::map<size_t,T>&, size_t) const;
    void insertValueAtRowIndex(const T&, std::shared_ptr<T>, size_t);
    void insertValueAtRowIndex(const T&, std::map<size_t, T>&, size_t);

    void resetRow(std::shared_ptr<T>&, size_t);
    void resetRow(std::map<size_t, T>&, size_t);

    /* ----- MEMBERS ----- */

    size_t _rows;
    size_t _columns;
    std::unique_ptr<Container*[]> _mat;
};

/* ----- ROWITERATOR (array) ----- */

template<typename T>
class RowIterator<T, std::shared_ptr<T>>{
public:
    typedef std::shared_ptr<T> Container;

    static RowIterator begin(Container *row){
        return RowIterator(row, 0);
    }

    static RowIterator end(Container *row, size_t columns){
        return RowIterator(row, columns);
    }

    RowIterator(const RowIterator &ri):
        _row(ri._row),
        _index(ri._index){
    }

    RowIterator& operator++(){
        ++_index;
        return *this;
    }

    std::pair<size_t, T&> operator*(){
        return {_index, (*_row).get()[_index]};
    }

    bool operator==(const RowIterator &rho)const{
        return _row == rho._row && _index == rho._index;
    }

    bool operator!=(const RowIterator &rho)const{
        return !(*this == rho);
    }
    
private:
    explicit RowIterator(Container *row, size_t index):
        _row(row),
        _index(index){
    }

    /* ----- MEMBERS ----- */

    Container *_row;
    size_t _index;
};

/* ----- CONSTROWITERATOR (array) ----- */

template<typename T>
class ConstRowIterator<T,std::shared_ptr<T>>{
public:
    typedef std::shared_ptr<T> Container;

    static ConstRowIterator begin(const Container *row){
        return ConstRowIterator(row, 0);
    }

    static ConstRowIterator end(const Container *row, size_t columns){
        return ConstRowIterator(row, columns);
    }

    ConstRowIterator(const ConstRowIterator &ri):
        _row(ri._row),
        _index(ri._index){
    }

    ConstRowIterator& operator++(){
        ++_index;
        return *this;
    }

    std::pair<size_t, const T&> operator*(){
        return {_index, (*_row).get()[_index]};
    }

    bool operator==(const ConstRowIterator &rho)const{
        return _row == rho._row && _index == rho._index;
    }

    bool operator!=(const ConstRowIterator &rho)const{
        return !(*this == rho);
    }
    
private:
    explicit ConstRowIterator(const Container *row, size_t index):
        _row(row),
        _index(index){
    }

    /* ----- MEMBERS ----- */

    const Container *_row;
    size_t _index;
};

/* ----- ROWITERATOR (map) ----- */

template<typename T>
class RowIterator<T, std::map<size_t, T>>{
public:
    typedef std::map<size_t, T> Container;
    typedef typename std::map<size_t, T>::iterator iterator;

    static RowIterator begin(Container *row){
        return RowIterator(row->begin());
    }

    static RowIterator end(Container *row, size_t columns){
        return RowIterator(row->end());
    }

    RowIterator(const RowIterator &ri):
        _it(ri._it){
    }

    RowIterator& operator++(){
        ++_it;
        return *this;
    }

    std::pair<size_t, T&> operator*(){
        return { _it->first, _it->second };
    }

    bool operator==(const RowIterator &rho)const{
        return _it == rho._it;
    }

    bool operator!=(const RowIterator &rho)const{
        return !(*this == rho);
    }
    
private:
    explicit RowIterator(const iterator & it):
        _it(it){
    }

    /* ----- MEMBERS ----- */

    iterator _it;
};

/* ----- CONSTROWITERATOR (map) ----- */

template<typename T>
class ConstRowIterator<T, std::map<size_t, T>>{
public:
    typedef std::map<size_t, T> Container;
    typedef typename std::map<size_t, T>::const_iterator constIterator;

    static ConstRowIterator begin(const Container *row){
        return ConstRowIterator(row->begin());
    }

    static ConstRowIterator end(const Container *row, size_t columns){
        return ConstRowIterator(row->end());
    }

    ConstRowIterator(const ConstRowIterator &ri):
        _it(ri._it){
    }

    ConstRowIterator& operator++(){
        ++_it;
        return *this;
    }

    std::pair<size_t, const T&> operator*(){
        return { _it->first, _it->second };
    }

    bool operator==(const ConstRowIterator &rho)const{
        return _it == rho._it;
    }

    bool operator!=(const ConstRowIterator &rho)const{
        return !(*this == rho);
    }
    
private:
    explicit ConstRowIterator(const constIterator & it):
        _it(it){
    }

    /* ----- MEMBERS ----- */

    constIterator _it;
};

/* ----- DEFINITIONS ----- */

/* ----- ROWITERATOR (Array) DEFINITIONS ----- */

/* ----- MATRIX DEFINITIONS ----- */

template<typename T, typename Container>
Matrix<T, Container>::Matrix(size_t rows, size_t columns):
    _rows(rows),
    _columns(columns),
    _mat(std::unique_ptr<Container[]>(new Container[rows])){
    auto end = _mat.get()+_rows;
    for(auto rowIt=_mat.get(); rowIt != end; rowIt++){
        resetRow(*rowIt,_columns);
    }
}

template<typename T, typename Container>
Matrix<T, Container>::Matrix(std::initializer_list<std::initializer_list<T>> il):
    _rows(il.size()),
    _columns(0),
    _mat(std::unique_ptr<Container[]>(new Container[il.size()])){

    size_t row = 0;
    size_t column = 0;
    auto endIL = il.end();
    for(auto rowIt = il.begin(); rowIt != endIL; ++rowIt){
        _columns=std::max(rowIt->size(), _columns);
        resetRow(_mat.get()[row], _columns);
        column = 0;
        auto endInnerIL = rowIt->end();
        for(auto columnIt = rowIt->begin(); columnIt != endInnerIL; ++columnIt){
            this->insertValueAtRowColumn(*columnIt, row, column);
            ++column;
        }
        ++row;
    }
}

template<typename T, typename Container>
template<typename Container_row>
Matrix<T, Container>::Matrix(size_t rows, size_t columns, const Container_row &srcRow):
    Matrix(rows,columns){
    auto end = _mat.get()+_rows;
    for(auto rowIt=_mat.get(); rowIt != end; rowIt++){
        copyRow(*rowIt, srcRow,_columns);
    }
}

template<typename T, typename Container>
template<typename Container_row>
Matrix<T, Container*>::Matrix(size_t rows, size_t columns, Container_row srcRow):
    _rows(rows),
    _columns(columns),
    _mat(std::unique_ptr<Container*[]>(new Container*[rows])){
    auto end = _mat.get()+_rows;
    for(auto rowIt=_mat.get(); rowIt != end; rowIt++){
        *rowIt = srcRow;
    }
}

template<typename T, typename Container>
Matrix<T, Container>::~Matrix(){
    auto end = _mat.get()+_rows;
    for(auto rowIt=_mat.get(); rowIt != end; rowIt++){
        *rowIt = Container();
    }
    _mat.reset();
}

template<typename T, typename Container>
Matrix<T, Container>::Matrix(const Matrix<T, Container>&oth):
    _rows(oth._rows),
    _columns(oth._columns),
    _mat(std::unique_ptr<Container[]>(new Container[oth._rows])){
    
    for(size_t row = 0; row < _rows; ++row){
        resetRow(_mat.get()[row],_columns);
        copyRow(_mat.get()[row], oth._mat.get()[row], _columns);
    }
}

template<typename T, typename Container>
Matrix<T, Container>::Matrix(Matrix<T, Container> &&oth) noexcept:
    _rows(oth._rows),
    _columns(oth._columns),
    _mat(oth._mat.release()){
}

template<typename T, typename Container>
Matrix<T, Container*>::Matrix(Matrix<T, Container*> &&oth) noexcept:
    _rows(oth._rows),
    _columns(oth._columns),
    _mat(oth._mat.release()){
}

template<typename T, typename Container>
Matrix<T, Container>& Matrix<T, Container>::operator=(const Matrix<T, Container> &rho){
    std::unique_ptr<Container[]> aux=std::unique_ptr<Container[]>(new Container[rho._rows]);
    for(size_t row = 0; row < rho._rows; ++row){
        resetRow(aux[row],rho._columns);
        copyRow(aux[row], rho._mat.get()[row], rho._columns);
    }

    _columns = rho._columns;
    _rows = rho._rows;
    _mat.reset(aux.release());

    return *this;
}
/*
template<typename T, typename Container>
Matrix<T, Container>& Matrix<T, Container>::operator=(Matrix<T, Container> &&rho) noexcept {
    if(&rho != this){
        _columns = rho._columns;
        _rows = rho._rows;
        _mat.reset(rho._mat.release());
    }
    return *this;
}
*/
template<typename T, typename Container>
Matrix<T, Container*>& Matrix<T, Container*>::operator=(Matrix<T, Container*> &&rho) noexcept {
    if(&rho != this){
        _columns = rho._columns;
        _rows = rho._rows;
        _mat.reset(rho._mat.release());
    }
    return *this;
}

template<typename T, typename Container>
void Matrix<T, Container>::resetRow(std::shared_ptr<T>& row, size_t columns){
    row = std::shared_ptr<T>(new T[columns], std::default_delete<T[]>());
    auto end = row.get()+columns;
    for(auto it = row.get(); it != end; ++it){
        *it = T();
    }
}

template<typename T, typename Container>
void Matrix<T, Container>::resetRow(std::map<size_t, T> &row, size_t columns){
    row=std::map<size_t, T>();
}

template<typename T, typename Container>
void Matrix<T, Container*>::resetRow(std::shared_ptr<T>& row, size_t columns){
    row = std::shared_ptr<T>(new T[columns], std::default_delete<T[]>());
    auto end = row.get()+columns;
    for(auto it = row.get(); it != end; ++it){
        *it = T();
    }
}

template<typename T, typename Container>
void Matrix<T, Container*>::resetRow(std::map<size_t, T> &row, size_t columns){
    row=std::map<size_t, T>();
}

template<typename T, typename Container>
void Matrix<T, Container>::copyRow(Container &dest, const std::shared_ptr<const T> src, size_t columns){
    size_t i=0;
    auto end = src.get()+columns;
    for(auto it = src.get(); it != end; ++it){
        dest.get()[i]=*it;
        ++i;
    }
}

template<typename T, typename Container>
void Matrix<T, Container>::copyRow(Container &dest, const std::map<size_t, T> &src, size_t columns){
    resetRow(dest,columns);
    for(auto it = src.begin(); it != src.end(); ++it){
        dest[it->first]=it->second;
    }
}

template<typename T, typename Container>
Matrix<T,Container> Matrix<T, Container>::transposed() const{
    Matrix<T,Container> mat(_columns, _rows);

    for(size_t r = 0; r < _rows; ++r){
        for(size_t c = 0; c < _columns; ++c){
            mat.insertValueAtRowColumn(this->retrieveAt(r,c), c, r);
        }
    }
        
    return mat;
}

template<typename T, typename Container>
T Matrix<T, Container>::retrieveAt(size_t row, size_t column) const{
    return getValueAtIndex(_mat.get()[row], column);
}

template<typename T, typename Container>
T Matrix<T, Container*>::retrieveAt(size_t row, size_t column) const{
    return getValueAtIndex(*(_mat.get()[row]), column);
}

template<typename T, typename Container>
T Matrix<T, Container>::getValueAtIndex(const std::shared_ptr<const T> row, size_t index) const{
    return row.get()[index];
}

template<typename T, typename Container>
T Matrix<T, Container*>::getValueAtIndex(const std::shared_ptr<const T> row, size_t index) const{
    return row.get()[index];
}

template<typename T, typename Container>
T Matrix<T, Container>::getValueAtIndex(const std::map<size_t,T> &row, size_t index) const{
    auto colIt = row.find(index);
    if(colIt != row.end()){
        return colIt->second;
    }
    else{
        return T();
    }
}

template<typename T, typename Container>
T Matrix<T, Container*>::getValueAtIndex(const std::map<size_t,T> &row, size_t index) const{
    auto colIt = row.find(index);
    if(colIt != row.end()){
        return colIt->second;
    }
    else{
        return T();
    }
}

template<typename T, typename Container>
void Matrix<T, Container>::insertValueAtRowColumn(const T& value, size_t row, size_t column){
    insertValueAtRowIndex(value, _mat.get()[row], column);
}

template<typename T, typename Container>
void Matrix<T, Container*>::insertValueAtRowColumn(const T& value, size_t row, size_t column){
    insertValueAtRowIndex(value, *_mat.get()[row], column);
}

template<typename T, typename Container>
void Matrix<T, Container>::insertValueAtRowIndex(const T& value, std::shared_ptr<T> row, size_t index){
    row.get()[index] = value;
}

template<typename T, typename Container>
void Matrix<T, Container*>::insertValueAtRowIndex(const T& value, std::shared_ptr<T> row, size_t index){
    row.get()[index] = value;
}

template<typename T, typename Container>
void Matrix<T, Container>::insertValueAtRowIndex(const T &value, std::map<size_t, T> &row, size_t index){
    if(value == T()){
        // if '0' is trying to be inserted, we must remove the position if it exists.
        auto colIt = row.find(index);
        if(colIt != row.end()){
            //if position exists, remove it.
            row.erase(colIt);
        }
    }
    else{
        // A value other than '0' is trying to be inserted so we either create the position and assigns the value
        // or assign the value to the existing position.
        row[index] = value;
    }
}

template<typename T, typename Container>
void Matrix<T, Container*>::insertValueAtRowIndex(const T &value, std::map<size_t, T> &row, size_t index){
    if(value == T()){
        // if '0' is trying to be inserted, we must remove the position if it exists.
        auto colIt = row.find(index);
        if(colIt != row.end()){
            //if position exists, remove it.
            row.erase(colIt);
        }
    }
    else{
        // A value other than '0' is trying to be inserted so we either create the position and assigns the value
        // or assign the value to the existing position.
        row[index] = value;
    }
}

template<typename T, typename Container>
template<typename Container2>
Matrix<T, Container>& Matrix<T, Container>::operator+=(const Matrix<T,Container2> &m2){
    for(size_t row = 0; row < _rows; ++row){
        auto endIt = m2.rowIteratorEnd(row);
        for(auto it = m2.rowIteratorBegin(row); it != endIt; ++it){
            T sum = this->at(row, (*it).first) + (*it).second;
            this->insertValueAtRowColumn(sum, row, (*it).first);
        }
    }

    return *this;
}

template<typename T, typename Container>
template<typename Container2>
Matrix<T, Container*>& Matrix<T, Container*>::operator+=(const Matrix<T,Container2> &m2){
    for(size_t row = 0; row < _rows; ++row){
        auto endIt = m2.rowIteratorEnd(row);
        for(auto it = m2.rowIteratorBegin(row); it != endIt; ++it){
            T sum = this->at(row, (*it).first) + (*it).second;
            this->insertValueAtRowColumn(sum, row, (*it).first);
        }
    }

    return *this;
}

template<typename T, typename Container>
template<typename Container2>
Matrix<T, Container>& Matrix<T, Container>::operator-=(const Matrix<T,Container2> &m2){
    for(size_t row = 0; row < _rows; ++row){
        auto endIt = m2.rowIteratorEnd(row);
        for(auto it = m2.rowIteratorBegin(row); it != endIt; ++it){
            T sum = this->at(row, (*it).first) - (*it).second;
            this->insertValueAtRowColumn(sum, row, (*it).first);
        }
    }

    return *this;
}

template<typename T, typename Container>
template<typename Container2>
Matrix<T, Container*>& Matrix<T, Container*>::operator-=(const Matrix<T,Container2> &m2){
    for(size_t row = 0; row < _rows; ++row){
        auto endIt = m2.rowIteratorEnd(row);
        for(auto it = m2.rowIteratorBegin(row); it != endIt; ++it){
            T sum = this->at(row, (*it).first) - (*it).second;
            this->insertValueAtRowColumn(sum, row, (*it).first);
        }
    }

    return *this;
}

template<typename T, typename Container>
Matrix<T, Container>& Matrix<T, Container>::operator*=(const T &scalar){
    if(scalar == T())
    {
        auto end = _mat.get() + _rows;
        // if multiplying by 0
        for(auto rowIt = _mat.get(); rowIt != end; ++rowIt){
            // reset each row
            resetRow(*rowIt,_columns);
        }
    }
    else{
        //if multiplying by scalar not equal 0
        for(size_t row = 0; row < _rows; ++row){
            //update each row
            auto endIt = this->rowIteratorEnd(row);
            for(auto it = this->rowIteratorBegin(row); it != endIt; ++it){
                //multiplying only stored values
                (*it).second*=scalar;
            }
        }
    }

    return *this;
}

template<typename T, typename Container>
Matrix<T, Container*>& Matrix<T, Container*>::operator*=(const T &scalar){
    if(scalar == T())
    {
        auto end = _mat.get() + _rows;
        // if multiplying by 0
        for(auto rowIt = _mat.get(); rowIt != end; ++rowIt){
            // reset each row
            resetRow(**rowIt,_columns);
        }
    }
    else{
        //if multiplying by scalar not equal 0
        for(size_t row = 0; row < _rows; ++row){
            //update each row
            auto endIt = this->rowIteratorEnd(row);
            for(auto it = this->rowIteratorBegin(row); it != endIt; ++it){
                //multiplying only stored values
                (*it).second*=scalar;
            }
        }
    }

    return *this;
}

template<typename T, typename Container>
Matrix<T, Container>& Matrix<T, Container>::operator/=(const T &scalar){
    for(size_t row = 0; row < _rows; ++row){
        //update each row
        auto endIt = this->rowIteratorEnd(row);
        for(auto it = this->rowIteratorBegin(row); it != endIt; ++it){
            //dividing only stored values
            (*it).second/=scalar;
        }
    }

    return *this;
}

template<typename T, typename Container>
Matrix<T, Container*>& Matrix<T, Container*>::operator/=(const T &scalar){
    for(size_t row = 0; row < _rows; ++row){
        //update each row
        auto endIt = this->rowIteratorEnd(row);
        for(auto it = this->rowIteratorBegin(row); it != endIt; ++it){
            //dividing only stored values
            (*it).second/=scalar;
        }
    }

    return *this;
}

/* ----- FUNCTIONS ----- */

template<typename T, typename Container>
std::ostream& operator<<(std::ostream &op, const Matrix<T,Container> &mat){
    op << "[" << std::endl;
    for(size_t row = 0; row < mat.rows(); ++row){
        op << " [";
        for(size_t column = 0; column < mat.columns(); ++column){
            op << " " << mat.retrieveAt(row,column);
        }
        op << " ]" << std::endl;
    }
    op << "]" << std::endl;
    return op;
}

// return the matrix addition.
// The container type used for the returned matrix is the same type as the one used for the left hand matrix.
template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator+(const Matrix<T, Container_1> &lh, const Matrix<T, Container_2> &rh){
    Matrix<T, Container_1> ret(lh);
    return ret+=rh;
}

// return the matrix subtraction.
// The container type used for the returned matrix is the same type as the one used for the left hand matrix.
template<typename T, typename Container_1, typename Container_2>
Matrix<T, Container_1> operator-(const Matrix<T, Container_1> &lh, const Matrix<T, Container_2> &rh){
    Matrix<T, Container_1> ret(lh);
    return ret-=rh;
}

template<typename U, typename Container_2>
Matrix<U, std::shared_ptr<U>> operator*(const Matrix<U, std::shared_ptr<U>> &mat1, const Matrix<U, Container_2> &mat2){
    auto rows1=mat1.rows();
    Matrix<U, std::shared_ptr<U>> ret(rows1,mat2.columns());
    // for each row in mat 1
    for(size_t row1 = 0; row1 < rows1; ++row1){    
        auto endRow1It = mat1.rowIteratorEnd(row1);
        // grab stored values in current row of mat1
        for(auto row1It = mat1.rowIteratorBegin(row1); row1It != endRow1It; ++row1It){
            auto endRow2It = mat2.rowIteratorEnd((*row1It).first);
            // grab stored values in the corresponding row of mat2
            for(auto row2It = mat2.rowIteratorBegin((*row1It).first); row2It != endRow2It; ++row2It){
                // multiply the corresponding value of the row of mat1 with the corresponding value of the row of mat2
                // and add that value in the corresponding position
                ret.at(row1,(*row2It).first)+=((*row1It).second*(*row2It).second);
            }
        }
    }

    return ret;
}

template<typename U, typename Container_2>
Matrix<U, std::shared_ptr<U>> operator*(const Matrix<U, std::shared_ptr<U>*> &mat1, const Matrix<U, Container_2> &mat2){
    auto rows1=mat1.rows();
    Matrix<U, std::shared_ptr<U>> ret(rows1,mat2.columns());
    // for each row in mat 1
    for(size_t row1 = 0; row1 < rows1; ++row1){    
        auto endRow1It = mat1.rowIteratorEnd(row1);
        // grab stored values in current row of mat1
        for(auto row1It = mat1.rowIteratorBegin(row1); row1It != endRow1It; ++row1It){
            auto endRow2It = mat2.rowIteratorEnd((*row1It).first);
            // grab stored values in the corresponding row of mat2
            for(auto row2It = mat2.rowIteratorBegin((*row1It).first); row2It != endRow2It; ++row2It){
                // multiply the corresponding value of the row of mat1 with the corresponding value of the row of mat2
                // and add that value in the corresponding position
                ret.at(row1,(*row2It).first)+=((*row1It).second*(*row2It).second);
            }
        }
    }

    return ret;
}

template<typename U, typename Container_1, typename Container_2>
Matrix<U, Container_1> operator*(const Matrix<U, Container_1> &mat1, const Matrix<U, Container_2> &mat2){
    auto rows1=mat1.rows();
    auto columns2=mat2.columns();
    Matrix<U, Container_1> ret(rows1,columns2);
    // for each row in mat 1
    for(size_t row1 = 0; row1 < rows1; ++row1){    
        auto endRow1It = mat1.rowIteratorEnd(row1);
        std::vector<U> tmpRow(columns2, 0);
        // grab stored values in current row of mat1
        for(auto row1It = mat1.rowIteratorBegin(row1); row1It != endRow1It; ++row1It){
            auto endRow2It = mat2.rowIteratorEnd((*row1It).first);
            // grab stored values in the corresponding row of mat2
            for(auto row2It = mat2.rowIteratorBegin((*row1It).first); row2It != endRow2It; ++row2It){
                // multiply the corresponding value of the row of mat1 with the corresponding value of the row of mat2
                // and add that value to the temp row in the corresponding position
                tmpRow[(*row2It).first]+=((*row1It).second*(*row2It).second);
            }
        }
        // finally copy the tempRow to the corresponding row of the returned matrix
        auto tmpRowSize = tmpRow.size();
        for(size_t i = 0; i < tmpRowSize; ++i){
            ret.insertValueAtRowColumn(tmpRow[i],row1, i);
        }
    }

    return ret;
}

// return the matrix multiplication.
// The container type used for the returned matrix is the same type as the one used for the first matrix but non pointer.
template<typename U, typename Container_1, typename Container_2>
Matrix<U, Container_1> operator*(const Matrix<U, Container_1*> &mat1, const Matrix<U, Container_2> &mat2){
    auto rows1=mat1.rows();
    auto columns2=mat2.columns();
    Matrix<U, Container_1> ret(rows1,columns2);
    // for each row in mat 1
    for(size_t row1 = 0; row1 < rows1; ++row1){    
        auto endRow1It = mat1.rowIteratorEnd(row1);
        std::vector<U> tmpRow(columns2, 0);
        // grab stored values in current row of mat1
        for(auto row1It = mat1.rowIteratorBegin(row1); row1It != endRow1It; ++row1It){
            auto endRow2It = mat2.rowIteratorEnd((*row1It).first);
            // grab stored values in the corresponding row of mat2
            for(auto row2It = mat2.rowIteratorBegin((*row1It).first); row2It != endRow2It; ++row2It){
                // multiply the corresponding value of the row of mat1 with the corresponding value of the row of mat2
                // and add that value to the temp row in the corresponding position
                tmpRow[(*row2It).first]+=((*row1It).second*(*row2It).second);
            }
        }
        // finally copy the tempRow to the corresponding row of the returned matrix
        auto tmpRowSize = tmpRow.size();
        for(size_t i = 0; i < tmpRowSize; ++i){
            ret.insertValueAtRowColumn(tmpRow[i],row1, i);
        }
    }

    return ret;
}

template<typename T, typename Container>
Matrix<T, Container> operator*(const Matrix<T, Container> &mat, T scalar){
    Matrix<T, Container> ret(mat);
    return ret*=scalar;
}

template<typename T, typename Container>
Matrix<T, Container> operator*(T scalar, const Matrix<T, Container> &mat){
    return mat*scalar;
}

template<typename T, typename Container>
Matrix<T, Container> operator/(const Matrix<T, Container> &mat, T scalar){
    Matrix<T, Container> ret(mat);
    return ret/=scalar;
}

//Vector operations

// returns the dot product of two vectors
// It will always return the dot product as long as you provide two matrix with rows == 1 and same columns
// If what is intended is the matrix product use the matrix operator*
template<typename T, typename Container1, typename Container2>
T dot(const Matrix<T, Container1> &v1, const Matrix<T, Container2> &v2){
    T sum = T();
    auto endIt = v1.rowIteratorEnd(0);

    for(auto it = v1.rowIteratorBegin(0); it != endIt; ++it){
        sum += ((*it).second * v2.retrieveAt(0,(*it).first));
    }

    return sum;
}

using DenseMatrix = Matrix<double>;
using SparceMatrix = Matrix<double, std::map<size_t, double>>;

#endif