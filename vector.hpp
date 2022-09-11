//vector.experiment.cpp
#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
namespace sjtu {
template<typename T>
class vector {
public:
    long long length,capacity;
    T *sta;
public:
    class const_iterator;
    class iterator {
    public:
        T *head;
        long long index;

    public:
        iterator(){}
        iterator(T *_head,long long _index ):head(_head),index(_index){}

        iterator(const iterator &other){
            head=other.head;
            index=other.index;
        }

        iterator operator+(const int &n) const {
            return iterator(head,index+n);
        }

        iterator operator-(const int &n) const {
            return iterator(head,index-n);
        }
        int operator-(const iterator &rhs) const {
            if(rhs.head!=head)throw invalid_iterator();
            return index-rhs.index;
        }
        iterator& operator+=(const int &n) {
            index+=n;
            return *this;
        }
        iterator& operator-=(const int &n) {         
            index-=n;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(head,index);
            index++;
            return tmp;
        }
        iterator& operator++() {
            ++index;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp(head,index);
            index--;
            return tmp;
        }
        iterator& operator--() {
            --index;
            return *this;
        }

        T& operator*() const{return head[index];}


        bool operator==(const iterator &rhs) const {
            return index==rhs.index&&head==rhs.head;
        }
        bool operator==(const const_iterator &rhs) const {
            return index==rhs.index&&head==rhs.head;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
           return !(*this==rhs);
        }

    };
    class const_iterator {
        private:
          
          T *head;
          long long index;

        public:
        const_iterator(){}
        const_iterator(T *_head,long long _index):head(_head),index(_index){}
        const_iterator(const const_iterator &other)=default;

        const_iterator operator+(const int &n) const {
            return const_iterator(head,index+n);
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(head,index-n);
        }

        const_iterator& operator+=(const int &n) {
            index+=n;
            return *this;
        }
        const_iterator& operator-=(const int &n) {         
            index-=n;
            return *this;
        }

        int operator-(const const_iterator &rhs) const {//?
            if(head!=rhs.head)throw invalid_iterator();
            return index-rhs.index;
        }

        const_iterator operator++(int) {
            const_iterator tmp(head,index);
            index++;
            return tmp;
        }
        const_iterator& operator++() {
            ++index;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp(head,index);
            index--;
            return tmp;
        }
        const_iterator& operator--() {
            --index;
            return *this;
        }

        T& operator*() const{
            return head[index];
        }
        bool operator==(const iterator &rhs) const {
            return index==rhs.index&&head==rhs.head;
        }
        bool operator==(const const_iterator &rhs) const {
            return index==rhs.index&&head==rhs.head;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }
    };
    vector(){
        length=0;
        sta=(T*)malloc(sizeof(T)*10);
        capacity=10;
    }

    vector(const vector &other) {
        length=other.length;
        capacity=other.capacity;

        sta=(T*)malloc(sizeof(T)*capacity);
        for(int i=0;i<length;++i){
            new(sta+i)T(other.sta[i]);//调用构造函数
        }
    }

    ~vector() {
        for(int i=0;i<length;++i)sta[i].~T();
       free(sta);
    }

    vector &operator=(const vector &other) {
         if(this==&other)return *this;
        for(int i=0;i<length;++i)sta[i].~T();
        free(sta);

         length=other.length;
         capacity=other.capacity;
        
        sta=(T*)malloc(sizeof(T)*capacity);//指出指针类型???

        for(int i=0;i<length;++i)new(sta+i)T(other.sta[i]);/////////////////
        return *this;
    }
    T & at(const size_t &pos) {
        if(pos<0||pos>=length)throw index_out_of_bound();
        return sta[pos];
    }
    const T & at(const size_t &pos) const {
        if(pos<0||pos>=length)throw index_out_of_bound();
        return sta[pos];
    }
    T & operator[](const size_t &pos) {
        if(pos<0||pos>=length)throw index_out_of_bound();
        return sta[pos];
    }
    const T & operator[](const size_t &pos) const {
        if(pos<0||pos>=length)throw index_out_of_bound();
        else return sta[pos];
    }
    const T & front() const {
        if(length==0)throw container_is_empty();
        return sta[0];
    }
    const T & back() const {
        if(length==0)throw container_is_empty();
        return sta[length-1];
    }
    iterator begin() {
      return iterator(sta,0);
    }
    const_iterator cbegin() const {
      return const_iterator(sta,0);
    }
    iterator end() {
       return iterator(sta,length);
    }
    const_iterator cend() const {
       return const_iterator(sta,length);
    }

    bool empty() const {
       return length==0;
    }

    size_t size() const {
        return length;
    }

    void clear() {
         for(int i=0;i<length;++i)sta[i].~T();
         length=0;
    }
    iterator insert(iterator pos, const T &value) {
        ++length;
        if(length>=capacity)doublesize();

        for(int i=length-1;i>pos.index;--i){
           // new(sta+i)T(sta[i-1]);//这里可以吗？？
            sta[i]=sta[i-1];//等号默认可以值传递
        }
        sta[pos.index]=value;
      //  new(sta+pos.index)T(value);

        return iterator(sta,pos.index);
    }
    iterator insert(const size_t &ind, const T &value) {
        if(ind<0||ind>length)throw index_out_of_bound();

        ++length;if(length>=capacity)doublesize();

        for(int i=length-1;i>ind;--i)
        {
           // new(sta+i)T(sta[i-1]);
           sta[i]=sta[i-1];
        }
       // new(sta+ind)T(value);
       sta[ind]=value;

        return iterator(sta,ind);
    }

    iterator erase(iterator pos) {
        --length;
        for(int i=pos.index;i<length;++i)
        {
           //new(sta+i)T(sta[i+1]);
           sta[i]=sta[i+1];
        }
        return iterator(sta,pos.index);
    }
    iterator erase(const size_t &ind) {
        if(ind<0||ind>=length)throw index_out_of_bound();
        
        --length;
        for(int i=ind;i<length;++i){
            //new(sta+i)T(sta[i+1]);调用构造函数并不会分配空间，这个不要紧
            sta[i]=sta[i+1];
        }
        return iterator(sta,ind);
    }

    void push_back(const T &value) {
        ++length;
        if(length>=capacity)doublesize();

        new(sta+length-1)T(value);
    }
    void pop_back() {
        if(length==0)throw container_is_empty();
       sta[length-1].~T();
        --length;
    }
    void doublesize(){
            capacity*=2;
            T * tmp=(T*)malloc(sizeof(T)*capacity);
            for(int i=0;i<length-1;++i){///////////////////////////////
                new (tmp+i)T(sta[i]);
                sta[i].~T();
            }
           // operator delete(tmp,sizeof(T)*(capacity/2));
           free(sta);
           sta=tmp;
    }

};
}
#endif