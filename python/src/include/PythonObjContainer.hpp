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
#include "pybind11/gil.h"

namespace bumblebee::python {

template <typename TGT_PY_TYPE, typename SRC_PY_TYPE>
struct PythonAssignmentFunction {
    typedef void (*assign_t)(TGT_PY_TYPE &, SRC_PY_TYPE &);
};

struct PythonGILWrapper {
    pybind11::gil_scoped_acquire acquire_;
};

// Every Python Object Must be created through our container
// The Container ensures that the GIL is HOLD on Python Object Construction/Destruction/Modification
template <class PY_TYPE>
class PythonObjectContainer {
public:
    PythonObjectContainer() {
    }

    ~PythonObjectContainer() {
        pybind11::gil_scoped_acquire acquire;
        pyObj_.clear();
    }

    std::unique_ptr<PythonGILWrapper> getLock() {
        return std::make_unique<PythonGILWrapper>();
    }

    template <class NEW_PY_TYPE>
    void assignInternal(typename PythonAssignmentFunction<PY_TYPE, NEW_PY_TYPE>::assign_t lambda,
                        NEW_PY_TYPE &new_value, PythonGILWrapper &lock) {
        PY_TYPE obj;
        lambda(obj, new_value);
        pushInternal(lock, obj);
    }

    void pushInternal(PythonGILWrapper &lock, PY_TYPE obj) {
        pyObj_.push_back(obj);
    }

    void push(PY_TYPE obj) {
        auto lock = getLock();
        PushInternal(*lock, move(obj));
    }

    const PY_TYPE *getPointerTop() {
        return &pyObj_.back();
    }

private:
    std::vector<PY_TYPE> pyObj_;
};
}
