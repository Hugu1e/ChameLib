#include "base/Element_t_matrix.h"

Element_t_matrix::Element_t_matrix()
{
    value.resize(0);
}
Element_t_matrix::Element_t_matrix(signed long int r, signed long int c, element_s *sample_element)
{
    value.resize(r);

    for (signed long int i = 0; i < r; ++i)
    {
        for (signed long int j = 0; j < c; ++j)
        {
            element_s *initialization_element = new element_s();
            element_init_same_as(initialization_element, sample_element);
            value[i].push_back(initialization_element);
        }
    }
}

/**
 * @brief formatted output of the matrix
 */
void Element_t_matrix::printMatrix()
{
    signed long int r = row();
    signed long int c = col();

    for (signed long int i = 0; i < r; ++i)
    {
        for (signed long int j = 0; j < c; ++j)
        {
            if (c - 1 == j)
            {
                element_printf("%B\n", value[i][j]);
            }
            else
            {
                element_printf("%B ", value[i][j]);
            }
        }
    }
}

/**
 * @brief get the row of the matrix
 *
 * @return row
 */
signed long int Element_t_matrix::row()
{
    return value.size();
}

/**
 * @brief get the col of the matrix
 *
 * @return col
 */
signed long int Element_t_matrix::col()
{
    return value[0].size();
}

/**
 * @brief get the element with the row index 'r' and the col index 'c'
 *
 * @param r row index
 * @param c col index
 * @return element
 */
element_s *Element_t_matrix::getElement(signed long int r, signed long int c)
{
    return value[r][c];
}

/**
 * @brief set 'elem' to the element with the row index 'r' and the col index 'c'
 * 
 * @param r the row index
 * @param c the col index
 * @param elem 'elem'
 */
void Element_t_matrix::setElement(signed long int r, signed long int c, element_s *elem)
{
    element_set(value[r][c], elem);
}

/**
 * @brief add '*v' to the end of the matrix as a row
 * 
 * @param v 'v'
 */
void Element_t_matrix::pushBack(Element_t_vector *v)
{
    signed long int len = v->length();
    if (0 == len)
    {
        return;
    }

    element_s *sample_element = v->getElement(0);
    signed long int r = row();

    std::vector<element_s *> vv;

    for (signed long int i = 0; i < len; ++i)
    {
        element_s *initialization_element = new element_s();
        element_init_same_as(initialization_element, sample_element);
        element_set(initialization_element, v->getElement(i));
        vv.push_back(initialization_element);
    }

    if (0 == r)
    {
        value.push_back(vv);
        return;
    }

    signed long int c = col();

    if (len == c)
    {
        value.push_back(vv);
    }
    else if (len < c)
    {
        for (signed long int i = 0; i < c - len; ++i)
        {
            element_s *initialization_element = new element_s();
            element_init_same_as(initialization_element, sample_element);
            element_set0(initialization_element);
            vv.push_back(initialization_element);
        }
        value.push_back(vv);
    }
    else
    {
        for (signed long int rr = 0; rr < r; ++rr)
        {
            for (signed long int i = 0; i < len - c; ++i)
            {
                element_s *initialization_element = new element_t();
                element_init_same_as(initialization_element, sample_element);
                element_set0(initialization_element);
                value[rr].push_back(initialization_element);
            }
        }
        value.push_back(vv);
    }
}

/**
 * @brief add '*v' to the insert_row of the matrix as a row
 * 
 * @param v 'v'
 */
void Element_t_matrix::pushBack(Element_t_vector *v, signed long int insert_row)
{
    signed long int len = v->length();
    if (0 == len)
    {
        return;
    }

    element_s *sample_element = v->getElement(0);
    signed long int r = row();

    std::vector<element_s *> vv;

    for (signed long int i = 0; i < len; ++i)
    {
        element_s *initialization_element = new element_s();
        element_init_same_as(initialization_element, sample_element);
        element_set(initialization_element, v->getElement(i));
        vv.push_back(initialization_element);
    }

    if (0 == r)
    {
        value.push_back(vv);
        return;
    }

    signed long int c = col();

    if (len == c)
    {
        if (insert_row == r - 1)
        {
            value.push_back(vv);
        }
        else
        {
            value.insert(value.begin() + insert_row + 1, vv);
        }
    }
    else if (len < c)
    {
        for (signed long int i = 0; i < c - len; ++i)
        {
            element_s *initialization_element = new element_s();
            element_init_same_as(initialization_element, sample_element);
            element_set0(initialization_element);
            vv.push_back(initialization_element);
        }
        if (insert_row == r - 1)
        {
            value.push_back(vv);
        }
        else
        {
            value.insert(value.begin() + insert_row + 1, vv);
        }
    }
    else
    {
        for (signed long int rr = 0; rr < r; ++rr)
        {
            for (signed long int i = 0; i < len - c; ++i)
            {
                element_s *initialization_element = new element_s();
                element_init_same_as(initialization_element, sample_element);
                element_set0(initialization_element);
                value[rr].push_back(initialization_element);
            }
        }
        if (insert_row == r - 1)
        {
            value.push_back(vv);
        }
        else
        {
            value.insert(value.begin() + insert_row + 1, vv);
        }
    }
}

/**
 * @brief add 'name'
 */
void Element_t_matrix::pushBackName(std::string name)
{
    names.push_back(name);
}

/**
 * @brief get the name with the row index 'r'
 */
std::string Element_t_matrix::getName(signed long int r)
{
    return names[r];
}

/**
 * @param M the matrix to be inverted
 * @return the inverse of the matrix
 */
Element_t_matrix* Element_t_matrix::inverse() {
    if (0 == row() || 0 == col()) {
        return NULL;
    }

    Element_t_matrix *res = new Element_t_matrix(col(), row(), getElement(0, 0));
    for (signed long int i = 0; i < row(); ++i) {
        for (signed long int j = 0; j < col(); ++j) {
            element_set(res->getElement(j, i), getElement(i, j));
        }
    }
    return res;
}


Element_t_vector* Element_t_matrix::getCoordinateAxisUnitVector() {
    if (0 == row() || 0 == col()) {
        return NULL;
    }

    Element_t_vector *res = new Element_t_vector(row(), getElement(0, 0));

    element_set1(res->getElement(0));
    for (signed long int i = 1; i < res->length(); ++i) {
        element_set0(res->getElement(i));
    }

    return res;
}


/**
 * @param x the solution vector of 'Ax=b'
 * @param A the coefficient matrix
 * @param b the constant vector
 * @return the return value indicates the kind of the solution,
 *         '-1' indicates there is no solution, '0' indicates there is the only solution,
 *         positive integer indicates there is innumberable solution, and marks the number of the free variable
 */
signed long int Element_t_matrix::gaussElimination(Element_t_vector *x, Element_t_matrix *A, Element_t_vector *b) {
    if (!(A->col() == x->length() && A->row() == b->length())) {
        return -1;
    }
    if (0 == A->col() || 0 == A->row()) {
        return -1;
    }

    // get the augmented matrix
    Element_t_matrix augmented_matrix(A->row(), A->col() + 1, x->getElement(0));
    for (signed long int i = 0; i < A->row(); i++) {
        for (signed long int j = 0; j < A->col() + 1; j++) {
            if (j == A->col()) {
                augmented_matrix.setElement(i, j, b->getElement(i));
            } else {
                augmented_matrix.setElement(i, j, A->getElement(i, j));
            }
        }
    }

    // free_x marks whether the free variable
    Num_vector free_x(A->col());

    // initialization
    element_t zero_elem;
    element_init_same_as(zero_elem, x->getElement(0));
    element_set0(zero_elem);
    for (signed long int i = 0; i < x->length(); i++) {
        x->setElement(i, zero_elem);
        free_x.setElement(i, 1);
    }

    // the currently processed row
    signed long int current_row;
    // the currently processed column
    signed long int current_col;
    // the row of the augmented matrix
    signed long int row = augmented_matrix.row();
    // the column of the augmented matrix
    signed long int col = augmented_matrix.col();

    // the row with nonzero value of the currently processed column
    signed long int nonzero_row;

    // free x number
    signed long int free_x_num;

    // the temporary variable for swap
    element_t temp;
    element_init_same_as(temp, zero_elem);

    // the inverse of the key element
    element_t inverse;
    element_init_same_as(inverse, zero_elem);

    // the coefficient for elimination
    element_t temp_coefficient;
    element_t coefficient;
    element_init_same_as(temp_coefficient, zero_elem);
    element_init_same_as(coefficient, zero_elem);

    // the elimination element
    element_t elimination;
    element_init_same_as(elimination, zero_elem);
    element_t elimination_result;
    element_init_same_as(elimination_result, zero_elem);

    // convert to the echelon matrix
    current_col = 0;
    for (current_row = 0; (current_row < row) && (current_col < col - 1); current_row++, current_col++) {
        // find the row with the nonzero value of the currently processed column (from 'current_row' to 'row - 1'),
        // and swap the row with index 'current_row' and the row with index 'nonzero_row'
        // when necessary so that augmented_matrix[current_row][current_col] is a nonzero value
        for (signed long int i = current_row; i < row; i++) {
            nonzero_row = i;
            if (!element_is0(augmented_matrix.getElement(nonzero_row, current_col))) {
                break;
            }
        }
        // this indicates the row of the currently processed column after the index 'current_row' has a zero value,
        // so we should process the next column of the currently processed row
        if (element_is0(augmented_matrix.getElement(nonzero_row, current_col))) {
            current_row--;
            continue;
        }
        // swap
        if (nonzero_row != current_row) {
            for (signed long int j = current_col; j < col; j++) {
                element_set(temp, augmented_matrix.getElement(current_row, j));
                augmented_matrix.setElement(current_row, j, augmented_matrix.getElement(nonzero_row, j));
                augmented_matrix.setElement(nonzero_row, j, temp);
            }
        }
        // eliminate the rows of the currently processed column after the index 'current_row'
        for (signed long int i = current_row + 1; i < row; i++) {
            if (!element_is0(augmented_matrix.getElement(i, current_col))) {
                element_invert(inverse, augmented_matrix.getElement(current_row, current_col));
                element_mul(temp_coefficient, inverse, augmented_matrix.getElement(i, current_col));
                element_neg(coefficient, temp_coefficient);
                for (signed long int j = current_col; j < col; j++) {
                    element_mul(elimination, augmented_matrix.getElement(current_row, j), coefficient);
                    element_add(elimination_result, augmented_matrix.getElement(i, j), elimination);
                    augmented_matrix.setElement(i, j, elimination_result);
                }
            }
        }
    }

//    cout << "the echelon matrix is" << endl;
//    augmented_matrix.printMatrix();

    // no solution
    for (signed long int i = current_row; i < row; i++) {
        if (!element_is0(augmented_matrix.getElement(i, col - 1))) {
            // free
            element_s* element_clear_list[] = {zero_elem, temp, inverse, temp_coefficient, coefficient, elimination, elimination_result};
            for (int i = 0; i < sizeof(element_clear_list) / sizeof(element_s*); i++) element_clear(element_clear_list[i]);
            
            return -1;
        }
    }

    // innumberable solution
    element_t random_value;
    element_init_same_as(random_value, zero_elem);
    element_t part_mul;
    element_init_same_as(part_mul, zero_elem);
    element_t inverse_part_mul;
    element_init_same_as(inverse_part_mul, zero_elem);
    element_t res;
    element_init_same_as(res, zero_elem);
    // free_index
    Num_vector free_index(col - 1);
    if (current_row < col - 1) {
        for (signed long int i = current_row - 1; i >= 0; i--) {
            element_set(res, augmented_matrix.getElement(i, col - 1));
            free_x_num = 0;
            for (signed long int j = i; j < col - 1; j++) {
                if (0 == free_x_num) {
                    if ((!element_is0(augmented_matrix.getElement(i, j))) && free_x.getElement(j)) {
                        free_x_num++;
                        free_index.setElement(free_x_num - 1, j);
                    }
                } else {
                    if (free_x.getElement(j)) {
                        free_x_num++;
                        free_index.setElement(free_x_num - 1, j);
                    }
                }
            }
            if (free_x_num > 1) {
                for (signed long int k = free_x_num - 1; k > 0; k--) {
                    // set random value
                    element_random(random_value);
                    x->setElement(free_index.getElement(k), random_value);
                    free_x.setElement(free_index.getElement(k), 0);
                    element_mul(part_mul, augmented_matrix.getElement(i, free_index.getElement(k)), random_value);
                    element_neg(inverse_part_mul, part_mul);
                    element_add(res, res, inverse_part_mul);
                }
                for (signed long int k = col - 2; k > free_index.getElement(free_x_num - 1); k--) {
                    if (!element_is0(augmented_matrix.getElement(i, k))) {
                        element_mul(part_mul, augmented_matrix.getElement(i, k), x->getElement(k));
                        element_neg(inverse_part_mul, part_mul);
                        element_add(res, res, inverse_part_mul);
                    }
                }
                element_invert(inverse, augmented_matrix.getElement(i, free_index.getElement(0)));
                element_mul(res, inverse, res);
                x->setElement(free_index.getElement(0), res);
                free_x.setElement(free_index.getElement(0), 0);
            } else {
                for (signed long int k = col - 2; k > free_index.getElement(0); k--) {
                    if (!element_is0(augmented_matrix.getElement(i, k))) {
                        element_mul(part_mul, augmented_matrix.getElement(i, k), x->getElement(k));
                        element_neg(inverse_part_mul, part_mul);
                        element_add(res, res, inverse_part_mul);
                    }
                }
                element_invert(inverse, augmented_matrix.getElement(i, free_index.getElement(0)));
                element_mul(res, inverse, res);
                x->setElement(free_index.getElement(0), res);
                free_x.setElement(free_index.getElement(0), 0);
            }
        }

        // free
        element_s* element_clear_list[] = {zero_elem, temp, inverse, temp_coefficient, coefficient, elimination, elimination_result, random_value, part_mul, inverse_part_mul, res};
        for (int i = 0; i < sizeof(element_clear_list) / sizeof(element_s*); i++) element_clear(element_clear_list[i]);

        return col - 1 - current_row;
    }

    // the only solution
    for (signed long int i = col - 2; i >= 0; i--) {
        element_set(res, augmented_matrix.getElement(i, col - 1));
        for (signed long int j = col - 2; j >= i + 1; j--) {
            if (!element_is0(augmented_matrix.getElement(i, j))) {
                element_mul(part_mul, augmented_matrix.getElement(i, j), x->getElement(j));
                element_neg(inverse_part_mul, part_mul);
                element_add(res, res, inverse_part_mul);
            }
        }
        element_invert(inverse, augmented_matrix.getElement(i, i));
        element_mul(res, inverse, res);
        x->setElement(i, res);
    }

    // free
    element_s* element_clear_list[] = {zero_elem, temp, inverse, temp_coefficient, coefficient, elimination, elimination_result, random_value, part_mul, inverse_part_mul, res};
    for (int i = 0; i < sizeof(element_clear_list) / sizeof(element_s*); i++) element_clear(element_clear_list[i]);

    return 0;
}

Element_t_matrix::~Element_t_matrix(){
    for(int i=0; i<value.size(); i++){
        for(int j=0; j<value[i].size(); j++){
            element_clear(value[i][j]);
            delete value[i][j];
        }
    }
}