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
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/parser/statement/Atom.h"
#include "bumblebee/common/Mutex.h"

namespace bumblebee{

enum OutputType:uint8_t {
    NONE = 0,
    TEXT = 1
};

// Empty output builder
struct NoneOutputBuilder {
    static inline void outputAtom(const Atom& atom) {}
    static inline void outputAtoms(const DataChunk& chunk, Predicate * predicate) {}
};

struct TextOutputBuilder {
    static inline void outputAtom(const Atom& atom) {
        std::cout << atom.toString() << std::endl;
    }
    static void outputAtoms(const DataChunk& chunk, Predicate * predicate) ;
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
            case NONE:
                NoneOutputBuilder::outputAtom(atom);
                break;
            default:
                ErrorHandler::errorNotImplemented("Output type not implemented!");
        }

    }
    void outputAtoms(const DataChunk& chunk, Predicate * predicate) {
        switch(type_) {
            case TEXT: {
                TextOutputBuilder::outputAtoms(chunk, predicate);
                break;
            }
            case NONE:
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
