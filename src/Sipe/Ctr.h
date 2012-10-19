/*
 Copyright 2012 Structure Computation  www.structure-computation.com
 Copyright 2012 Hugo Leclerc

 This file is part of Sipe.

 Sipe is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Sipe is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Sipe. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef CTR_H
#define CTR_H

/**
  Pointer on an object, or NULL
*/
template<class T>
class Ctr {
public:
    Ctr() {
        ptr = 0;
    }

    Ctr( const T &val ) {
        ptr = new T( val );
    }

    Ctr( const Ctr &val ) {
        ptr = val.ptr ? new T( *val.ptr ) : 0;
    }

    ~Ctr() {
        delete ptr;
    }

    Ctr &operator=( const T &val ) {
        T *old = ptr;
        ptr = new T( val );
        delete old;
        return *this;
    }

    Ctr &operator=( const Ctr &val ) {
        T *old = ptr;
        ptr = val.ptr ? new T( *val.ptr ) : 0;
        delete old;
        return *this;
    }

    operator bool() const { return ptr; }

    void clear() {
        delete ptr;
        ptr = 0;
    }

    const T *operator->() const { return ptr; }
    T *operator->() { return ptr; }

    const T &operator*() const { return *ptr; }
    T &operator*() { return *ptr; }

    const T *get_ptr() const { return ptr; }
    T *get_ptr() { return ptr; }

protected:
    T *ptr;
};


#endif // CTR_H
