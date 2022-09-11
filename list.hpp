//list.hpp
//last version

#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
template<typename T>
class list {
protected:
    class node {
    public:
        T *val;
        node *next,*pre;

        node():pre(NULL),next(NULL),val(nullptr){}
        explicit node  (node * _pre,node *_next,const T & value):pre(_pre),next(_next){
            val=new T(value);
        }
        ~node(){
            if(val)delete val;
        }
    };

protected:
    node *head,*tail;
    long long length;

    node *insert(node *pos, node *cur) {
        cur->pre=pos->pre;
        cur->next=pos;

        pos->pre->next=cur;
        pos->pre=cur; 
        ++length;
        return cur;
    }
    node *erase(node *pos) {
        pos->pre->next=pos->next;
        pos->next->pre=pos->pre;

        pos->next=NULL;
        pos->pre=NULL;
        --length;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    	friend class list<T>;
    	friend class const_iterator;
    private:
        node *cur;
        node *head;
    public:
        iterator(){}
        iterator(node *_cur,node * _head):cur(_cur),head(_head){}
        iterator(const iterator & t){
            cur=t.cur;head=t.head;
        }
        iterator operator++(int) {
            if(cur->next==NULL)throw invalid_iterator();
            cur=cur->next;
            return iterator(cur->pre,head);
        }
        iterator & operator++() {
            if(cur->next==NULL)throw invalid_iterator();
            cur=cur->next;
            return *this;
        }
        iterator operator--(int) {
            if(cur->pre->pre==NULL)throw invalid_iterator();
            cur=cur->pre;
            return iterator(cur->next,head);
        }
        iterator & operator--() {
            if(cur->pre->pre==NULL)throw invalid_iterator();
            cur=cur->pre;
            return *this;
        }
        T & operator *() const {
            if(cur==head||cur->next==NULL)throw invalid_iterator();
            return *(cur->val);
        }
        T * operator ->() const noexcept {
            if(cur==head||cur->next==NULL)throw invalid_iterator();
            return cur->val;
        }
        bool operator==(const iterator &rhs) const {
            return head==rhs.head&&cur==rhs.cur;
        }
        bool operator==(const const_iterator &rhs) const {
            return head==rhs.head&&cur==rhs.cur;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }
    };

    class const_iterator {
    	friend class list<T>;
    	friend class iterator;
    private:
        node *cur;
        node *head;
    public:
        const_iterator(){}
        const_iterator(node *_cur,node * _head):cur(_cur),head(_head){}//假如node没这个构造函数就会够呛
        const_iterator(const const_iterator & t){
            cur=t.cur;head=t.head;
        }
        const_iterator(const iterator &t){
            cur=t.cur;head=t.head;
        }
        const_iterator operator++(int) {
            if(cur->next==NULL)throw invalid_iterator();
            cur=cur->next;
            return const_iterator(cur->pre,head);
        }
        const_iterator & operator++() {
            if(cur->next==NULL)throw invalid_iterator();
            cur=cur->next;
            return *this;
        }
        const_iterator operator--(int) {
            if(cur->pre->pre==NULL)throw invalid_iterator();
            cur=cur->pre;
            return const_iterator(cur->next,head);
        }
        const_iterator & operator--() {
            if(cur->pre->pre==NULL)throw invalid_iterator();
            cur=cur->pre;
            return *this;
        }
        T & operator *() const {
            if(!cur->val)throw invalid_iterator();
            return *(cur->val);
        }
        T * operator ->() const noexcept {
            return cur->val;
        }
        bool operator==(const iterator &rhs) const {
            return head==rhs.head&&cur==rhs.cur;
        }
        bool operator==(const const_iterator &rhs) const {
            return head==rhs.head&&cur==rhs.cur;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }

    };
    list(){
        head=new node;
        tail=new node;
        head->next=tail;
        tail->pre=head;
        length=0;
    }
    list(const list &other) {
        head=new node;
        tail=new node;

        head->next=tail;
        tail->pre=head;
        length=other.length;
        node *tmp=other.head->next;
        node *prev=head;
        node *post=tail;
        for(;tmp!=other.tail;tmp=tmp->next){
            node *add=new node(prev,post,*(tmp->val));
            prev->next=add;
            post->pre=add;

            prev=add;
        }
    }
    virtual ~list() {
        clear();
        length=0;
        delete head;
        delete tail;
    }

    list &operator=(const list &other) {
        if(this == & other)return *this;
        node *tmp=head->next;
        node *temp;
        while(tmp!=tail){
            temp=tmp->next;
            delete tmp;

            tmp=temp;
        }
        head->next=tail;
        tail->pre=head;

        length=other.length;
        tmp=other.head->next;
        node *prev=head;
        node *post=tail;
        for(;tmp!=other.tail;tmp=tmp->next){
            node *add=new node(prev,post,*(tmp->val));
            prev->next=add;
            post->pre=add;

            prev=add;
        }
        return *this;
    }
    const T & front() const {
        if(head->next==tail)throw container_is_empty();
        return *(head->next->val);
    }
    const T & back() const {
        if(head->next==tail)throw container_is_empty();
        return *(tail->pre->val);
    }
    iterator begin() {
        return iterator(head->next,head);
    }
    const_iterator cbegin() const {
        return const_iterator(head->next,head);
    }
    iterator end() {
        return iterator(tail,head);
    }
    const_iterator cend() const {
        return const_iterator(tail,head);
    }
    virtual bool empty() const {
        return (head->next==tail);
    }
    virtual size_t size() const {
        return length;
    }
    virtual void clear() {
        node *tmp=head->next;
        node *temp;
        while(tmp!=tail){
            temp=tmp->next;
            delete tmp;

            tmp=temp;
        }
        length=0;
        head->next=tail;
        tail->pre=head;

    }
    virtual iterator insert(iterator pos, const T &value) {
        if(pos.head!=head)throw invalid_iterator();
        node *inse=new node(pos.cur->pre,pos.cur,value);
        pos.cur->pre->next=inse;
        pos.cur->pre=inse;////////////////
        ++length;
        return iterator(inse,head);
    }
    virtual iterator erase(iterator pos) {
      if(pos==end()||pos.cur==head)throw invalid_iterator();
     if(pos.head->next->next==NULL)throw invalid_iterator();
       iterator t(pos.cur->next,pos.head);
        pos.cur->pre->next=pos.cur->next;
        pos.cur->next->pre=pos.cur->pre;
        pos.cur->next=NULL;
        pos.cur->pre=NULL;
        --length;

       delete pos.cur;
        return t;
    }
    void push_back(const T &value) {
        node * tmp=new node(tail->pre,tail,value);
        tail->pre->next=tmp;
        tail->pre=tmp;
        ++length;
    }
    void pop_back() {
        if(head->next==tail)throw container_is_empty();
        node *tmp=tail->pre;

        tmp->pre->next=tail;
        tail->pre=tmp->pre;

        delete tmp;
        --length;
    }
    void push_front(const T &value) {
        node *tmp=new node(head,head->next,value);
        head->next->pre=tmp;
        head->next=tmp;
        ++length;
    }
    void pop_front() {
        if(head->next==tail)throw container_is_empty();
        node *tmp=head->next;

        head->next=tmp->next;
        tmp->next->pre=head;
        delete tmp;
        --length;

    }
    void sort() {
        T *sta;
            auto cmp = [](const T &a, const T& b)->bool
            {
                return a < b;
            };
        sta=(T*)malloc(sizeof(T)*length);
        node *tmp=head->next;
        for(int i=0;i<length;++i){
            new(sta+i)T(*(tmp->val));

            tmp->val->~T();
            tmp=tmp->next;
        };
        sjtu::sort< T >(sta,sta+length,cmp);
        long long record=length;
        tmp=head->next;
        length=record;

        for(int i=0;i<length;++i){
           new(tmp->val)T(sta[i]);
           sta[i].~T();
            tmp=tmp->next;
        }
        free(sta);
    }
    void merge(list & other) {
        length=length+other.length;
        other.length=0;
        if(head->next==tail){
            head->next=other.head->next;
            other.head->next->pre=head;
            tail->pre=other.tail->pre;
            other.tail->pre->next=tail;

            other.head->next=other.tail;
            other.tail->pre=other.head;
            return ;
        }

        node *this_cur,*other_cur;
        this_cur=head->next;other_cur=other.head->next;
        if(other_cur==other.tail)return ;
        while(this_cur!=tail){
        	
            if(*(this_cur->val)<*(other_cur->val)){
                  this_cur=this_cur->next;
                  continue;
            }
            else if(*(this_cur->val)==*(other_cur->val)){
                  this_cur=this_cur->next;
                  continue;
            }
            else  {
                //insert before and next;
                node *tmp=other_cur->next;
                other_cur->pre=this_cur->pre;
                other_cur->next=this_cur;

                this_cur->pre->next=other_cur;
                this_cur->pre=other_cur;

                if(tmp!=other.tail)other_cur=tmp;
                else   {other_cur=other.tail;break;}
            }
        }
        if(this_cur==tail){
        		node *tmp=other_cur;
            while(tmp!=other.tail){
                tmp=other_cur->next;
                other_cur->pre=this_cur->pre;
                other_cur->next=this_cur;
                this_cur->pre->next=other_cur;
                this_cur->pre=other_cur;

                other_cur=tmp;

            }
        }
        other.head->next=other.tail;
        other.tail->pre=other.head;

    }
    void reverse() {
        if(head->next==tail)return ;
        node *tmp=head->next;
        node **tp=(node **)malloc(sizeof(node *)*length);
        for(long long i=0;i<length;++i){
            tp[i]=tmp;
            tmp=tmp->next;
        }
        node *prev=head;
        for(long long i=length-1;i>=0;--i){
            tp[i]->pre=prev;
            tp[i]->next=tail;
            prev->next=tp[i];
            tail->pre=tp[i];

            prev=tp[i];
        }
        free(tp);

        return ;
    }
    void unique() {
       node *tmp=head->next;
       if(head->next==tail)return ;
       while(tmp->next!=tail){
           if(*(tmp->val)==*(tmp->next->val)){
               node *del=tmp->next;
               tmp->next=del->next;
               del->next->pre=tmp;

               delete del;
               --length;
               continue;
           }
           else {
               tmp=tmp->next;
           }
       }
       return ;
    }
};

}

#endif //SJTU_LIST_HPP

//record:
/*
   the changes from start to AC;

   1.malloc the meaning of using malloc

   2.node 1node();    delete

   3.head is not an element of this container

   4.length;

   5.literator's default deconstructor can't delete new space!!!!!,and in real context,there is need to delete its cur's space!!!!
   so there is no need for ~literator();

   6.insert no delete head!!!!

   7.sort   if parameter is transported in value forms,the time may be TLE;
   so using &

   when there is no defalult **or malloc!!!!!!!;

   how is the delaration organized???
   bool ->?????

   8.merge()   store tmp->next before it is changed!!!

   9.reverse    don't forget to free

   10.reverse():   be clear about when there is need for new ,when not;

   11.the first time I hand in,the de way in sort is a bit wrong.(head->next?,tail->pre?? !

   12 try to use tool function to eliminate you code!!!!

*/