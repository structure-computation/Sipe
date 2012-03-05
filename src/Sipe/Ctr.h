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
