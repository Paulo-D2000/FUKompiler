#include <iostream>
#include <vector>
#include <string>

#include "Common.h"
#include "Lexer.h"
#include "Parser.h"

int parse_functions_from_string(const std::string& contents, std::vector<Func>& out, const std::string& inputName){
    Lexer lexer(contents, inputName);
    Token token;
    Func func;
    int ret = parse_function(lexer, token, func);
    while(ret == 1){
        out.push_back(func);
        ret = parse_function(lexer, token, func);
    }
    return ret;
}


struct TestCase {
    const char* name;
    const char* code;
    bool expect_ok;
};

int main(){
    std::vector<TestCase> cases = {
        {"test_void_fun", "void test_void_fun(){\n    // Should pass... \n}\n", true},
        {"test_char_fun", "char test_char_fun(){\n    // Should fail... \n}\n", false},
        {"test_int_fun", "int test_int_fun(){\n    // Should fail... \n}\n", false},
        {"test_float_fun", "float test_float_fun(){\n    // Should fail... \n}\n", false},
        {"test_void_fun_ret", "void test_void_fun_ret(){\n    return; // should pass...\n}\n", true},
        {"test_void_fun_ret_val", "void test_void_fun_ret_val(){\n    return 0; // should fail...\n}\n", false},
        {"test_char_fun_ret", "char test_char_fun_ret(){\n    return 0; // should pass...\n}\n", true},
        {"test_int_fun_ret", "char test_int_fun_ret(){\n    return 0; // should pass...\n}\n", true},
        {"test_void_fun_ret_arg", "int test_void_fun_ret_arg(int arg){\n    // Should pass... \n    return 0;\n}\n", true},
        {"test_void_fun_ret_void_arg", "int test_void_fun_ret_void_arg(void arg){\n    // Should fail... \n    return 0;\n}\n", false},
        {"test_void_fun_ret_same_type_args", "int test_void_fun_ret_same_type_args(int arg0, int arg1, int arg2){\n    // Should pass... \n    return 0;\n}\n", true},
        {"test_void_fun_ret_diff_type_args", "int test_void_fun_ret_diff_type_args(int arg0, float arg1, char arg2){\n    // Should pass... \n    return 0;\n}\n", true},
        {"test_unary_ops", "int test_unary_ops(int a){\n    int paren_lit = (0); // should pass\n    int paren_var = (a); // should pass\n    return 0;\n}\n", true},
        {"test_binaryops", "int test_binaryops(int a, int b){\n    int _add = a + b;\n    int _sub = a - b;\n    int _mul = a * b;\n    int _div = a / b;\n    int _or = a | b;\n    int _and = a & b;\n    int _xor = a ^ b;\n    int gt = a > b;\n    int lt = a < b;\n    int gteq = a >= b;\n    int leq = a <= b;\n    int eq = a == b;\n    return 0;\n}\n", true},
        {"test_binaryop_chain", "int test_binaryop_chain(){\n    return 1 + 2 + 3 + 4 + 5;\n}\n", true}
    };

    int failures = 0;

    for(size_t i=0;i<cases.size();++i){
        const auto &tc = cases[i];
        std::vector<Func> funcs;
        int ret = parse_functions_from_string(std::string(tc.code), funcs, std::string(tc.name));
        bool ok = (ret != -1) && (funcs.size() > 0);
        if(ok != tc.expect_ok){
            ++failures;
            std::cout << "[FAIL] " << tc.name << " (expected " << (tc.expect_ok?"OK":"FAIL") << ")\n";
        } else {
            std::cout << "[PASS] " << tc.name << "\n";
        }
    }

    if(failures == 0){
        std::cout << "All tests passed.\n";
        return 0;
    } else {
        std::cout << failures << " test(s) failed.\n";
        return 1;
    }
}
