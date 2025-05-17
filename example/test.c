#test_pragma
# test_pragma_space

//Test_comment
// Test_comment_space

/*Test_multiline_comment*/
/* Test_multiline_comment_space */

void test_void_fun(){
    // Should pass... 
}

char test_char_fun(){
    // Should fail... 
}

int test_int_fun(){
    // Should fail... 
}

float test_float_fun(){
    // Should fail... 
}

void test_void_fun_ret(){
    return 0; // should fail...
}

char test_char_fun_ret(){
    return 0; // should pass...
}

char test_int_fun_ret(){
    return 0; // should pass...
}

char test_float_fun_ret(){
    return 0; // should pass...
}

int test_void_fun_ret_arg(int arg){
    // Should pass... 
    return 0;
}

int test_void_fun_ret_void_arg(void arg){
    // Should fail... 
    return 0;
}

// OK
/*
int test_void_fun_ret_arg_op(void arg + 9){
    // Should fail... 
    return 0;
}
*/

int test_void_fun_ret_same_type_args(int arg0, int arg1, int arg2){
    // Should pass... 
    return 0;
}

int test_void_fun_ret_diff_type_args(int arg0, float arg1, char arg2){
    // Should pass... 
    return 0;
}

int test_unary_ops(int a){
    int paren_lit = (0); // should pass
    int paren_var = (a); // should pass
    return 0;
}

int test_binaryops(int a, int b){
    int _add = a + b;
    int _sub = a - b;
    int _mul = a * b;
    int _div = a / b;
    int _or = a | b;
    int _and = a & b;
    int gt = a > b;
    int lt = a < b;
    int gteq = a >= b;
    int leq = a <= b;
    int eq = a == b;
    return 0;
}

int test_binaryop_chain(){
    return 1 + 2 + 3 + 4 + 5;
}

/*
Multiline End comment
*/