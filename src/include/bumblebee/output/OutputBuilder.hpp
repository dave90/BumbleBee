/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include <iostream>
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/parser/statement/Atom.hpp"
#include "bumblebee/common/Mutex.hpp"

namespace bumblebee{

enum OutputType:uint8_t {
    NONE_OUTPUT = 0,
    TEXT = 1
};

// Empty output builder
struct NoneOutputBuilder {
    static inline void outputAtom(const Atom& atom) {}
    static inline void outputAtoms(DataChunk& chunk, Predicate * predicate) {}
};

struct TextOutputBuilder {
    static inline void outputAtom(const Atom& atom) {
        std::cout << atom.toString() << "."<< std::endl;
    }
    static void outputAtoms(DataChunk& chunk, Predicate * predicate) ;

    DataChunk cached_;
};

// Class that handle the output
class OutputBuilder {
public:
    explicit OutputBuilder(OutputType type): type_(type) {}

    void outputAtom(const Atom& atom) {
        switch(type_) {
            case TEXT: {
                TextOutputBuilder::outputAtom(atom);
                break;
            }
            case NONE_OUTPUT:
                NoneOutputBuilder::outputAtom(atom);
                break;
            default:
                ErrorHandler::errorNotImplemented("Output type not implemented!");
        }

    }
    void outputAtoms(DataChunk& chunk, Predicate * predicate) {
        switch(type_) {
            case TEXT: {
                TextOutputBuilder::outputAtoms(chunk, predicate);
                break;
            }
            case NONE_OUTPUT:
                NoneOutputBuilder::outputAtoms(chunk, predicate);
                break;
            default:
                ErrorHandler::errorNotImplemented("Output type not implemented!");
        }
    }

private:
    OutputType type_;
};

}
