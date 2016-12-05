#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ruby.h"

static VALUE eTimeoutError;

typedef struct _sum_node {
    long sum;
    long length;
    long height;
    long* numbers;
    struct _sum_node *left;
    struct _sum_node *right;
} sum_node;

static long rbss_sum(long* numbers, long length) {
    long i;
    long sum = 0;
    for(i=0; i<length; i++) {
        sum += numbers[i];
    }
    return sum;
}

/*
static void rbss_print(long* numbers, long length) {
    long i;
    long sum = 0;
    printf("[");
    for(i=0; i<length; i++) {
        printf("%ld,", numbers[i]);
    }
    printf("]\n");
    return;
}
*/

static void rbss_free_nodes(sum_node* node) {
    if(node != NULL) {
        rbss_free_nodes(node->left);
        node->left = NULL;
        rbss_free_nodes(node->right);
        node->right = NULL;
        free(node->numbers);
        node->numbers = NULL;
        free(node);
        node = NULL;
    }
}

static sum_node* rbss_new_node(long* numbers, long length) {
    sum_node* node;
    
    if((node = (sum_node*)calloc(1, sizeof(sum_node))) == NULL) {
        return NULL;
    }
    if((node->numbers = (long*)calloc(length, sizeof(long))) == NULL) {
        free(node);
        return NULL;
    }
    node->sum = rbss_sum(numbers, length);
    node->length = length;
    node->height = 0;
    memcpy(node->numbers, numbers, length*sizeof(long));
    node->left = NULL;
    node->right = NULL;
    return node;
}

static long rbss_height(sum_node* node) {
    return node == NULL ? -1 : node->height;
}

static long rbss_max(long h1, long h2) {
    return h1 > h2 ? h1 : h2;
}

static void rbss_reset_height(sum_node* node) {
    node->height = rbss_max(rbss_height(node->left), rbss_height(node->right)) + 1;
}

static sum_node* rbss_rotate_left_1(sum_node* node2) {
    sum_node* node1;
    
    node1 = node2->left;
    node2->left = node1->right;
    node1->right = node2;
    
    rbss_reset_height(node2);
    rbss_reset_height(node1);
    return node1;
}

static sum_node* rbss_rotate_right_1(sum_node* node1) {
    sum_node* node2;
    
    node2 = node1->right;
    node1->right = node2->left;
    node2->left = node1;
    
    rbss_reset_height(node1);
    rbss_reset_height(node2);
    return node2;
}

static sum_node* rbss_rotate_left_2(sum_node* node) {
    node->left = rbss_rotate_right_1(node->left);
    return rbss_rotate_left_1(node);
}

static sum_node* rbss_rotate_right_2(sum_node* node) {
    node->right = rbss_rotate_left_1(node->right);
    return rbss_rotate_right_1(node);
}

/* Insert a new node in the AVL tree */
static sum_node* rbss_insert(sum_node* node, long sum, long* numbers, long length) {
    sum_node* t;
    
    if(node == NULL) {
        node = rbss_new_node(numbers, length);
        return node;
    }
    if(sum < node->sum) {
        if((t = rbss_insert(node->left, sum, numbers, length)) == NULL) {
            return NULL;
        }
        node->left = t;
        if(rbss_height(node->left) - rbss_height(node->right) == 2) {
            if(sum < node->left->sum) {
                node = rbss_rotate_left_1(node);
            } else {
                node = rbss_rotate_left_2(node);
            }
        }
    } else if (sum > node->sum) {
        if((t = rbss_insert(node->right, sum, numbers, length)) == NULL) {
            return NULL;
        }
        node->right = t;
        if(rbss_height(node->right) - rbss_height(node->left) == 2) {
            if(sum > node->right->sum) {
                node = rbss_rotate_right_1(node);
            } else {
                node = rbss_rotate_right_2(node);
            }
        }
    }
    rbss_reset_height(node);
    return node;
}

/* Look in the tree to see if there is a node with the matching sum. */
static sum_node* rbss_lookup(sum_node* node, long sum) {
    if(node == NULL) {
        return NULL;
    }
    return sum == node->sum ? node : rbss_lookup(sum < node->sum ? node->left : node->right, sum);
}

/* Add a node to the tree associating the sum of the numbers with the numbers */
static int rbss_add(sum_node** table, long* numbers, long length, long* temp, long want) {
    sum_node* t;
    t = rbss_insert(*table, rbss_sum(numbers, length), numbers, length);
    if(t == NULL) {
        return -1;
    }
    *table = t;
    return 0;
}

/* Check and see if the difference between the sum of the numbers and the
   number we want is in the tree.  If so, update temp to be the current
   array of numbers plus the matching value in the tree. */
static int rbss_check(sum_node** table, long* numbers, long length, long* temp, long want) {
    long sum;
    long diff;
    sum_node* node;
    
    sum = rbss_sum(numbers, length);
    diff = want - sum;
    if((node = rbss_lookup(*table, diff)) == NULL) {
        return 0;
    }
    memcpy(temp, node->numbers, node->length*sizeof(long));
    memcpy(temp+node->length, numbers, length*sizeof(long));
    return 1;
}

/* For each subset of the array of numbers, execute the function, passing
   the table, subset, length of subset, and the temp and want variables to
   the function. */
static int rbss_subsets(int(*func)(sum_node**, long*, long, long*, long), sum_node** table, long* numbers, long length, long skip, long* temp, long want, time_t start_time, time_t max_seconds) {
    int result;
    time_t current_time;
    long i;
    long limit;
    long* temp_numbers;
    
    limit = length - 1;
    if((result = (*func)(table, numbers, length, temp, want)) != 0) {
        return result;
    }
    if(limit > 0) {
        if(max_seconds){
            if((current_time = time(NULL)) == -1) {
                return -3;
            }
            if(current_time - start_time > max_seconds) {
                return -2;
            }
        }
        if((temp_numbers = (long*)calloc(limit, sizeof(long))) == NULL) {
            rb_raise(rb_eNoMemError, "calloc");
        }
        for(i=limit; i>=skip; i--) {
            memcpy(temp_numbers, numbers, i*sizeof(long));
            memcpy(temp_numbers+i, numbers+i+1, (limit-i)*sizeof(long));
            if((result = rbss_subsets(func, table, temp_numbers, limit, i, temp, want, start_time, max_seconds)) != 0) {
                free(temp_numbers);
                return result;
            }
        }
        free(temp_numbers);
    }
    return 0;
}

/* Build a ruby array of Fixnums from the C array of longs */
static VALUE rbss_array(long* numbers, long length) {
    long i;
    VALUE array;
    
    array = rb_ary_new();
    for(i=0;i<length;i++) {
        if(numbers[i] != 0) {
            rb_ary_push(array, LONG2NUM(numbers[i]));
        }
    }
    return array;
}

/* Raise a ruby exception based on the result of res.
   Should only be run after the all memory has been cleaned up. */
static void rbss_raise_error(int res) {
    if(res == -1) {
        rb_raise(rb_eNoMemError, "calloc");
    }
    if(res == -2) {
        rb_raise(eTimeoutError, "timeout expired");
    }
    if(res == -3) {
        rb_raise(rb_eStandardError, "can't get current time");
    }
}

/*
Document-method: subset_sum

call-seq:
  SubsetSum.subset_sum(array, num) -> subset_of_array || nil

If a subset of the numbers in array sum to num, the subset is returned.
Otherwise, nil is returned.
*/
static VALUE rbss_main(VALUE self, VALUE numbers, VALUE result, VALUE max_seconds) {
    int res;
    time_t start_time = 0;
    time_t c_max_seconds;
    long i;
    long length;
    long half_length;
    long c_result;
    long* c_numbers;
    long* c_numbers_tmp;
    sum_node* table = NULL;
    VALUE answer;
    
    Check_Type(numbers, T_ARRAY);
    
    c_result = NUM2LONG(result);
    c_max_seconds = NUM2INT(max_seconds);
    length = RARRAY_LEN(numbers);
    half_length = length/2;
    
    if(c_max_seconds < 0) {
        c_max_seconds = 0;
    }
    if(c_max_seconds) {
        if((start_time = time(NULL)) == -1) {
            rb_raise(rb_eStandardError, "can't get current time");
        }
    }
    
    if((c_numbers = (long*)calloc(length*2, sizeof(long))) == NULL) {
        rb_raise(rb_eNoMemError, "calloc");
    }
    c_numbers_tmp = c_numbers+length;

    for(i=0; i<length; i++) {
        c_numbers[i] = NUM2LONG(rb_ary_entry(numbers, i));
    }
    
    /* Initialize the tree with an empty subset */
    table = rbss_insert(table, 0, NULL, 0);

    /* Add a node to the tree for each subset of the first half of the set */
    if((res = rbss_subsets(&rbss_add, &table, c_numbers, half_length, 0, \
      c_numbers_tmp, c_result, start_time, c_max_seconds)) != 0) {
        rbss_free_nodes(table);
        free(c_numbers);
        rbss_raise_error(res);
    }

    /* See if one of the subsets in the first half sums to the result */
    if((res = rbss_check(&table, NULL, 0, c_numbers_tmp, c_result)) == 0) {
       /* Check all subsets in the second half, and see if the difference
          between their sum and the result is a sum of one of the subsets
          in the first half */
        res = rbss_subsets(&rbss_check, &table, c_numbers+half_length, \
          length-half_length, 0, c_numbers_tmp, c_result, start_time, \
          c_max_seconds);
    }
    if(res < 1) {
        rbss_free_nodes(table);
        free(c_numbers);
        if(res < 0) {
            rbss_raise_error(res);
        }
        return Qnil;
    }

    rbss_free_nodes(table);
    answer = rbss_array(c_numbers_tmp, length);
    free(c_numbers);
    return answer;
}

#ifndef RB_FIXNUM_P
#define RB_FIXNUM_P(v) FIXNUM_P(v)
#endif

static VALUE rbss_supported(VALUE self, VALUE want, VALUE pos, VALUE neg, VALUE max_seconds) {
    if (RB_FIXNUM_P(want) && RB_FIXNUM_P(pos) && RB_FIXNUM_P(neg) && RB_FIXNUM_P(max_seconds)) {
        return Qtrue;
    }
    return Qfalse;
}

void Init_subset_sum() {
    VALUE SubsetSum;
    SubsetSum = rb_define_module("SubsetSum");
    rb_define_module_function(SubsetSum, "_subset_sum", rbss_main, 3);
    rb_define_module_function(SubsetSum, "_subset_sum_supported?", rbss_supported, 4);
    eTimeoutError = rb_define_class_under(SubsetSum, "TimeoutError", rb_eStandardError);
}
