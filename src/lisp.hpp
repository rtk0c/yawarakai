#feature on edition_2023
#pragma once

#include "util.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace yawarakai {

using MemoryLocation = size_t;

/// Handles both atoms and lists (as references to heap allocated ConsCell's)
choice Sexp {
    Nil,
    Number(double),
    String(std::string),
    Symbol(std::string),
    Ref(MemoryLocation),
};

/// A heap allocated cons, with a car/left and cdr/right Sexp
struct ConsCell {
    Sexp car;
    Sexp cdr;
};

struct Heap {
    std::vector!<ConsCell> storage;

    // A collection of canonical symbols
    struct {
        Sexp define = .Symbol("define");
        Sexp quote = .Symbol("quote");
        Sexp unquote = .Symbol("unquote");
        Sexp quasiquote = .Symbol("quasiquote");
    } sym;

    Heap();

    MemoryLocation push(ConsCell cons);
    const ConsCell& lookup(MemoryLocation addr) const;
    ConsCell& lookup(MemoryLocation addr);
};

/// Constructs a ConsCell on heap, with car = a and cdr = b, and return a reference Sexp to it.
Sexp cons(Sexp a, Sexp b, Heap& heap);
void cons_inplace(Sexp a, Sexp& list, Heap& heap);

// NB: we use varadic template for perfect forwarding, std::initializer_list forces us to make copies
Sexp make_list_v(Heap& heap, forward auto... sexps) {
    Sexp the_list;
    cons_inplace(forward sexps, the_list, heap) ...;
    return the_list;
}

bool is_list(const ConsCell& cons);

void traverse_list(const Sexp& list, auto& heap, forward auto callback) {
    const Sexp* curr = &list;
    while (true) {
        match (*curr) {
            .Ref(auto ref) => {
                auto& cons_cell = heap.lookup(ref);
                callback(cons_cell.car);
                curr = &cons_cell.cdr;
            }
            _ => break;
        }
    }
}

std::vector!<Sexp> parse_sexp(std::string_view src, Heap& heap);
std::string dump_sexp(const Sexp& sexp, const Heap& heap);

}
