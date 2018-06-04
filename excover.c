#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY (-1)

typedef struct instance{ // list of instance
    char *row;
    struct instance *next;
    int index;
}instance_t;

typedef struct result{
    int set;
    struct result *next;
} result_t;

typedef struct node { // node of incidence graph
    struct node *col_header;
    struct node *top;
    struct node *right;
    struct node *bottom;
    struct node *left;
    int rowID, colID, nodes_counter;
}node_t;

void append_string(instance_t *ins, char *s){
    instance_t *ptr, *new;
    ptr = ins;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    if (ptr->row == NULL){
        ptr->row = s;
        ptr->index = 0;
    }
    else {
        new = (instance_t *) malloc(sizeof(instance_t));
        new->row = s;
        new->next = NULL;
        new->index = ptr->index + 1;
        ptr->next = new;
    }
}

void append_set(result_t *res, int s){
    result_t *ptr, *new;
    ptr = res;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    if (ptr->set == EMPTY) {
        ptr->set = s;

    }
    else {
        new = (result_t *) malloc(sizeof(result_t));
        new->set = s;
        new->next = NULL;
        ptr->next = new;
    }
}

void remove_last_set(result_t *res) {
    result_t *prev_ptr;
    result_t *ptr;

    if (res->next == NULL) {
        res->set = EMPTY;
    }
    else {
        prev_ptr = res;
        ptr = res->next;

        while (ptr->next != NULL) {
            prev_ptr = prev_ptr->next;
            ptr = ptr->next;
        }

        if (ptr != NULL) {
            prev_ptr->next = NULL;
            free(ptr);
        }
    }
}

void read(instance_t *ins){
    int c;
    int i = 0;
    int size = 1;
    char *s;
    s = (char *) malloc(size * sizeof(char));
    c = getchar();
    while (c != EOF) {
        if (i + 1 == size) {
            size++;
            s = realloc(s, size * sizeof(char));
            if (c == '\n') {
                s[i] = '\0';
                append_string(ins, s);
                size = 1;
                i = 0;
                s = (char *) malloc(size * sizeof(char));
                c = getchar();
                continue;
            }
        }
        s[i++] = (char) c;
        c = getchar();
    }
    free(s);
}

int hm_rows(instance_t *ins){
    int rows = 0;
    instance_t *ptr;
    ptr = ins;
    while (ptr != NULL){
        rows++;
        ptr = ptr->next;
    }
    return rows;
}

void incidence_matrix(instance_t *ins, int *matrix, int r, int c){
    instance_t *ptr;
    ptr = ins->next;
    for (int j = 0; j < c; j++) {
        *(matrix + j) = 1;
    }
    for (int i = 1; i < r; i++) {
        int j = 0;
        while (j < c){
            if (ptr->row[j] == '_') {
                *((matrix + i*c) + j) = 0;
            }
            else {
                *((matrix + i*c) + j) = 1;
            }
            j++;
        }
        ptr = ptr->next;
    }
}

int go_up(int i, instance_t *ins){
    return ((i - 1 < 0) ? hm_rows(ins) - 1 : i - 1);
}
int go_right(int i, instance_t *ins){
    return ((i + 1) % (int) strlen(ins->row));
}
int go_down(int i, instance_t *ins){
    return ((i + 1) % (hm_rows(ins)));
}
int go_left(int i, instance_t *ins){
    return ((i - 1 < 0) ? (int) strlen(ins->row) - 1 : i - 1);
}

node_t *make_graph(int *inc_mat, int r, int c, instance_t *ins, node_t *matrix){
    node_t *header;
    header = (node_t *) malloc(sizeof(node_t));

    for (int i = 0; i < c; i++) {
        (matrix + i)->nodes_counter = 0;
    }

    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++){
            if (*((inc_mat + i * c) + j)) {
                if (i) {
                    ((matrix + j)->nodes_counter)++;
                }
                ((matrix + i * c) + j)->col_header = (matrix + j);
                ((matrix + i * c) + j)->rowID = i;
                ((matrix + i * c) + j)->colID = j;
                int x = i, y = j;
                // link nodes with their neighbors
                //top
                do {x = go_up(x, ins);} while (!(*((inc_mat + x * c) + y)) && x != i);
                ((matrix + i * c) + j)->top = ((matrix + x * c) + j);
                // right
                x = i;
                y = j;
                do {y = go_right(y, ins);} while (!(*((inc_mat + x * c) + y)) && y != j);
                ((matrix + i * c) + j)->right = ((matrix + i * c) + y);
                // bottom
                x = i;
                y = j;
                do {x = go_down(x, ins);} while (!(*((inc_mat + x * c) + y)) && x != i);
                ((matrix + i * c) + j)->bottom = ((matrix + x * c) + j);
                // left
                x = i;
                y = j;
                do {y = go_left(y, ins);} while (!(*((inc_mat + x * c) + y)) && y != j);
                ((matrix + i * c) + j)->left = ((matrix + i * c) + y);
            }
        }
    }

    // linking header with columns
    // first
    header->right = matrix;
    //last
    header->left = (matrix + (c - 1));

    matrix->left = header;
    (matrix + (c - 1))->right = header;
    return header;
}

int filtered_lenght(instance_t *ins){

    int f_len = 0;

    for (unsigned int i = 0; i < strlen(ins->row); i++) {
        if (ins->row[i] == '+')
            f_len++;
    }

    return f_len;
}

void print_solution(result_t *res, instance_t *ins, int len){

    char tmp_str[strlen(ins->row) + 1];
    tmp_str[strlen(ins->row)] = '\0';
    result_t *ptr_res = res;
    instance_t *ptr_ins = ins;

    while (ptr_res != NULL) {
        while (ptr_ins->index != ptr_res->set) {
            ptr_ins = ptr_ins->next;
        }
        for (unsigned int it = 0; it < strlen(ins->row); it++) {
            if (ptr_ins->row[it] != '_') {
                tmp_str[it] = ptr_ins->row[it];
            }
        }
        ptr_res = ptr_res->next;
        ptr_ins = ins;
    }

    char fin_str[len + 1];

    for (int i = 0; i < len + 1; i++) {
        fin_str[i] = '\0';
    }

    int i = 0;

    for (unsigned int it = 0; it < strlen(ins->row); it++) {
        if (ins->row[it] == '+') {
            fin_str[i] = tmp_str[it];
            i++;
        }
    }

    for (unsigned int it = 0; it < strlen(fin_str); it++) {
            printf("%c", fin_str[it]);
    }

    printf("\n");
}

void hide_node(node_t *node, node_t *matrix){
    node_t *col_of_node;

    col_of_node = node->col_header;
    col_of_node->left->right = col_of_node->right;
    col_of_node->right->left = col_of_node->left;

    for (node_t *row = col_of_node->bottom; row != col_of_node; row = row->bottom){
        for (node_t *right_n = row->right; right_n != row; right_n = right_n->right) {
            right_n->top->bottom = right_n->bottom;
            right_n->bottom->top = right_n->top;

            ((matrix + (right_n->colID))->nodes_counter)--;
        }
    }
}

void reveal_node(node_t *node, node_t *matrix){

    node_t *col_of_node;
    col_of_node = node->col_header;

    for (node_t *row = col_of_node->top; row != col_of_node; row = row->top) {
        for (node_t *left_n = row->left; left_n != row; left_n = left_n->left) {
            left_n->top->bottom = left_n;
            left_n->bottom->top = left_n;

            ((matrix + (left_n->colID))->nodes_counter)++;
        }
    }

    col_of_node->left->right = col_of_node;
    col_of_node->right->left = col_of_node;
}

void solve(node_t *header, node_t *matrix, result_t *res, instance_t *ins, int len){

    node_t *col;

    if (header->right == header) {
        print_solution(res, ins, len);
    }
    else {
        col = header->right;
        hide_node(col, matrix);

        for (node_t *row = col->bottom; row != col; row = row->bottom) {
            append_set(res, row->rowID);
            for (node_t *right_n = row->right; right_n != row; right_n = right_n->right) {
                hide_node(right_n, matrix);
            }
            solve(header, matrix, res, ins, len);
            remove_last_set(res);

            col = row->col_header;
            for (node_t *left_n = row->left; left_n != row; left_n = left_n->left) {
                reveal_node(left_n, matrix);
            }
        }
        reveal_node(col, matrix);
    }
}

int main(void) {

    instance_t *ins;
    result_t *res;

    res = (result_t*) malloc(sizeof(result_t));
    res->set = EMPTY;
    res->next = NULL;
    ins = (instance_t*) malloc(sizeof(instance_t));
    ins->row = NULL;
    ins->next = NULL;
    read(ins);

    if (ins->row == NULL)
        return 0;

    int r = hm_rows(ins);
    int c = (int) strlen(ins->row);
    int inc_mat[r][c];

    incidence_matrix(ins, *inc_mat, r, c);

    node_t matrix[r][c];

    node_t *header = make_graph(*inc_mat, r, c, ins, *matrix);

    int len = filtered_lenght(ins);

    solve(header, *matrix, res, ins, len);

    while(ins) {
        instance_t *n = ins->next;
        free(ins->row);
        free(ins);
        ins = n;
    }
    free(res);
    free(header);
}
