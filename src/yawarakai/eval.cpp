module yawarakai;

import std;

#include "util.hpp"

using namespace std::literals;

namespace yawarakai {

#define RETURN_SEXP(v) { Sexp s; s.set(v); return s; }

Sexp builtin_add(const Sexp& params, Environment& env) {
    double res = 0.0;
    traverse_list(params, env, [&res](const Sexp& param) {
        res += param.as_or_error<double>("Error: + cannot accept non-numerical parameters"sv); 
    });
    
    RETURN_SEXP(res);
}

Sexp builtin_sub(const Sexp& params, Environment& env) {
    auto& first_cons = env.lookup(params.as<MemoryLocation>());

    double res = first_cons.car.as<double>();
    traverse_list(first_cons.cdr, env, [&res](const Sexp& param) {
        res -= param.as_or_error<double>("Error: - cannot accept non-numerical parameters"sv); 
    });
    
    RETURN_SEXP(res);
}

Sexp builtin_mul(const Sexp& params, Environment& env) {
    double res = 1.0;
    traverse_list(params, env, [&res](const Sexp& param) {
        res *= param.as_or_error<double>("Error: * cannot accept non-numerical parameters"sv); 
    });
    
    RETURN_SEXP(res);
}

Sexp builtin_div(const Sexp& params, Environment& env) {
    auto& first_cons = env.lookup(params.as<MemoryLocation>());

    double res = first_cons.car.as<double>();
    traverse_list(first_cons.cdr, env, [&res](const Sexp& param) {
        res /= param.as_or_error<double>("Error: / cannot accept non-numerical parameters"sv); 
    });
    
    RETURN_SEXP(res);
}

Sexp builtin_if(const Sexp& params, Environment& env) {
    const Sexp* cond;
    const Sexp* true_case;
    const Sexp* false_case;
    list_nth_at_beg(params, {&cond, &true_case, &false_case}, env);

    Sexp cond_val = eval(*cond, env);
    if (cond_val.is<bool>() && cond_val.as<bool>()) {
        return eval(*true_case, env);
    } else {
        return eval(*false_case, env);
    }
}

Sexp builtin_define(const Sexp& params, Environment& env) {
    // TODO

    return Sexp{};
}

#define ITEM(name, func) { name, BuiltinProc{ name, func } }
const std::map<std::string_view, BuiltinProc> BUILTINS{
    ITEM("+"sv, builtin_add),
    ITEM("-"sv, builtin_sub),
    ITEM("*"sv, builtin_mul),
    ITEM("/"sv, builtin_div),
    ITEM("if"sv, builtin_if),
    ITEM("define"sv, builtin_define),
};

Sexp eval(const Sexp& sexp, Environment& env) {
    using enum Sexp::Type;

    auto& cons_cell = env.lookup(sexp.as<MemoryLocation>());
    auto& sym = cons_cell.car;
    auto& params = cons_cell.cdr;

    if (sym.get_type() != TYPE_SYMBOL) throw "Invalid eval format";
    const auto& proc_name = sym.as<Symbol>().name;

    if (auto iter = BUILTINS.find(proc_name); iter != BUILTINS.end()) {
        auto& builtin_proc = iter->second;
        return builtin_proc.fn(params, env);
    }

    return {}; // TODO
}

}