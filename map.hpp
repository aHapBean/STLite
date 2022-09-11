/**
 * implement a container like std::map
 */
//map.hpp 6.10 18.00 - 6.11 16.00 wwwwwwwwwwwwwwwwww
//wuwuwuwuwuwuwwuuwuwuwuwuwwuwuwuwuwuwuwuwuwuw
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//www
//www 
//AC.last version

#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less<Key>// 1 if(add,rt)放在左边！
> class map {
public:
	friend class iterator;
	friend class const_iterator;
    typedef pair<const Key, T> value_type;
private:
    size_t max(size_t a,size_t b){
        if(a > b)return a;
        return b;
    }
	struct node{
        value_type *data;
        node *l,*r;
        size_t h;
        node(){
        	l = r = nullptr;
        	h = 0;
            data = nullptr;
		}
        node(const value_type & ele,node *lc,node *rc,node *fa,size_t _h = 1){
            data = new value_type(ele);
            l = lc,r = rc;
            h = _h;
        }
        node(const node &other){
            data = new value_type(*other.data);
            l = other.l;
            r = other.r;
            h = other.h;
        }
        ~node(){
            if(data)delete data;
        }
    };

    Compare cmp;
    bool sml(const Key &a,const Key &b)const{
        return Compare()(a,b);
    }
    bool e(const Key &a,const Key &b)const{//3 const !!!!!!!!!!!!!!!很多地方有限制
        return !(Compare()(a,b)||Compare()(b,a));//2 沙雕！！！！！！！！！！！！！！ 
    }
    size_t h(node *t){
        return (t == nullptr) ? 0 : t->h;
    }
    node *rt,*nil;
    size_t sz;

private:
    void del(node *rt){//4 判空莫忘 ??????????????????????????????????????????????
        if(rt == nullptr)return ;
        if(rt->l)del(rt->l);
        if(rt->r)del(rt->r);
        delete rt;
    }

    void copy(const map & t){
        sz = t.sz;
        copy(rt,t.rt);
    }

    void copy(node * & rt,node * ot){//5 nil是一个标记用的空指针，与map没有关系
        if(ot == nullptr)return ;
        rt = new node(*ot->data,nullptr,nullptr,nullptr,ot->h);
        if(ot->l)copy(rt->l,ot->l);
        if(ot->r)copy(rt->r,ot->r);
    }
    void LL(node * &t){
        node *t1 = t->l;
        t->l = t1->r;
        t1->r = t;
        t->h = max(h(t->l),h(t->r)) + 1;
        t1->h = max(h(t1->l),h(t)) + 1;
        t = t1;
    }
    void RR(node * &t){
        node * t1 = t->r;
        t->r = t1->l;
        t1->l = t;
        t->h = max(h(t->l),h(t->r)) + 1;
        t1->h = max(h(t1->r),h(t)) + 1;
        t = t1;//6 代码小漏洞决定生死。。???????????????????????????????????????????????????????????????????????????????????????????????????
    }
    void LR(node * &t){
        RR(t->l);
        LL(t);
    }
    void RL(node * &t){
        LL(t->r);
        RR(t);
    }
//7 const 函数不能传给一个不是const的函数！！！！！！！！！！！！！！！！！！！！！！ 
    node* pre(const Key &key,node* x,node *res)const{
        node *pos = x;
        while(Compare()(pos->data->first, key)){
            res = pos;
            pos = pos->r;
        }
        if(e(pos->data->first, key)){
            if(pos->l){
                res = pos->l;
                while(res->r != nullptr) res = res->r;
            }
        }
        else res = pre(key, pos->l, res);
        return res;
	}
	//8 当没有。。直接返回nil !
	node* nxt(const Key & key,node* t,node *res)const{
		node *pos = t;
        if(t == nullptr)return res;
        while(pos && Compare()(key, pos->data->first)){
            res = pos;
            pos = pos->l;
        }
        if(e(key, pos->data->first)){
            if(pos->r){
                res = pos->r;
                while(res->l != nullptr) res = res->l;
            }
        }
        else res = nxt(key, pos->r, res);
        return res;
	}
	
	node* fi()const{
		node *ans=rt;
		if(ans == nullptr)return nil;//two
		while (ans->l != nullptr)ans = ans->l;
		return ans;
	}

    node* last()const{
        node *ans = rt;
        if(ans == nullptr)return nil;
        while(ans->r != nullptr)ans = ans->r;
        return ans;
    }
    node *insert(const value_type &x,node * & t){//这样转会把我的这个ans搞掉吗？？
    //17 为什么有些地方加&就错误 是因为是外界传来的吗？？
    	node * ans;
        if(t == nullptr){
			t = new node(x,nullptr,nullptr,nullptr);
			return t;
		}
        else if(sml(x.first,t->data->first)){
            ans = insert(x,t->l);
            if(h(t->l) - h(t->r) == 2)
                if(t->l && t->l->data && sml(x.first,t->l->data->first))LL(t);
                else LR(t);
        }
        else if(sml(t->data->first,x.first)){
            ans = insert(x,t->r);
            if(h(t->r) - h(t->l) == 2)
                if(sml(t->r->data->first,x.first))RR(t);
                else RL(t);
        }
        t->h = max(h(t->l),h(t->r)) + 1;
        return ans;
    }
    
    bool erase(const Key & x,node * &t){
        if(t == nullptr)return true;
        if(e(x,t->data->first)){//找到
            if(t->l == nullptr || t->r == nullptr){//只有一个儿子
                node * od = t;
                t = (t->l != nullptr) ? t->l : t->r;
                delete od;
                return false;
            }
            else {//两个儿子时
            
            node *tmp = t->r, *f = t;
                while(tmp->l != nullptr) f = tmp, tmp = tmp->l;
                node* ne = new node(*tmp->data,nullptr,nullptr,nullptr);
                ne->l = tmp->l;
                ne->r = tmp->r;
                ne->h = tmp->h;

                if(f->l == tmp) f->l = ne;
                else f->r = ne;

                tmp->l = t->l;
                tmp->r = t->r;
                tmp->h = t->h;
                node* del = t;
                t = tmp;
                delete del;
                if(erase(t->data->first, t->r)) return true;
                else return adjust(t, 1);//先复制当替身的节点，用一样的新节点代替它，然后将这个替身放到应该到的位置去
            /*
                node *tmp = t->r;//
                while(tmp->l != nullptr)tmp = tmp->l;
                if(t->data)delete t->data;//three!
                t->data = new value_type(*tmp->data);
                //t->data = tmp->data;//changed!
                if(erase(tmp->data->first,t->r))return true;
                return adjust(t,1);//这个地方修改又不一致，注意一下，以后写代码时！！
                *///????
            }
        }

        if(sml(x,t->data->first)){//查找左子树
            if(erase(x,t->l))return true;
            return adjust(t,0);
        }
        else {//右子树
            if(erase(x,t->r))return true;
            return adjust(t,1);
        }
    }
    bool adjust(node * &t,size_t st){
        if(st){
            if(h(t->l) - h(t->r) == 1)return true;
            if(h(t->r) == h(t->l)){t->h--;return false;}
            if(h(t->l->r) > h(t->l->l)){//???这里会有问题吗
                LR(t);
                return false;
            }
            LL(t);
            if(h(t->r) == h(t->l))return false;
            return true;
        }
        else{
            if(h(t->r) - h(t->l) == 1)return true;
            if(h(t->r) == h(t->l)){t->h--;return false;}
            if(h(t->r->l) > h(t->r->r)){
                RL(t);
                return false;
            }
            RR(t);
            if(h(t->r) == h(t->l))return false;
            return true;
        }
    }

    node * find(const Key & key,node *t)const{
        if(t == nullptr)return nil;// 19 
        const Key & _key = t->data->first;
        if(e(_key,key))return t;
		else {
            if(sml(key,_key))return find(key,t->l);
            return find(key,t->r);
        }
    }
public:
    class const_iterator;
    class iterator {
    	friend class map;
    private:
        map* cur;
        node* nd;

    public:
        iterator() { cur = nullptr, nd = nullptr; }
        iterator(const iterator &other) :cur(other.cur), nd(other.nd) {}

        // iter++
        iterator operator++(int){
            if(cur == nullptr || nd == cur->nil)
                throw invalid_iterator();
            iterator ret = *this;
            nd = cur->nxt(nd->data->first, cur->rt, cur->nil);
            return ret;
        }
        // ++iter
        iterator & operator++(){
            if(cur == nullptr || nd == cur->nil)
                throw invalid_iterator();
            nd = cur->nxt(nd->data->first, cur->rt, cur->nil);
            return *this;
        }
        // iter--
        iterator operator--(int){
            if(cur == nullptr || *this == cur->begin())
                throw invalid_iterator();
            iterator ret = *this;
            if(this->nd == cur->nil) nd = cur->last();
            else nd = cur->pre(nd->data->first, cur->rt, cur->nil);
            return ret;
        }
        // --iter
        iterator & operator--(){
            if(cur == nullptr || *this == cur->begin())
                throw invalid_iterator();
            if(this->nd == cur->nil) nd = cur->last();
            else nd = cur->pre(nd->data->first, cur->rt, cur->nil);
            return *this;
        }

        value_type & operator*() const{
            return *nd->data;
        }

        value_type* operator->() const noexcept{
            return nd->data;
        }

        bool operator==(const iterator &rhs) const { 
            return (cur == rhs.cur && nd == rhs.nd);
        }
        bool operator==(const const_iterator &rhs) const { return (cur == rhs.cur && nd == rhs.nd); }
        bool operator!=(const iterator &rhs) const { return (cur != rhs.cur || nd != rhs.nd); }
        bool operator!=(const const_iterator &rhs) const { return (cur != rhs.cur || nd != rhs.nd); }
    };
    class const_iterator {
    friend class map;

    private:
        const map* cur;
        node* nd;

    public:
        const_iterator() { cur = nullptr, nd = nullptr; }
        const_iterator(const map* _cur, node* _nd) :cur(_cur), nd(_nd) {}
        const_iterator(const const_iterator &other) :cur(other.cur), nd(other.nd) {}
        const_iterator(const iterator &other) :cur(other.cur), nd(other.nd) {}

        // iter++
        const_iterator operator++(int){
            if(cur == nullptr || nd == cur->nil)
                throw invalid_iterator();
            const_iterator ret = *this;
            nd = cur->nxt(nd->data->first, cur->rt, cur->nil);
            return ret;
        }
        // ++iter
        const_iterator & operator++(){
            if(cur == nullptr || nd == cur->nil)
                throw invalid_iterator();
            nd = cur->nxt(nd->data->first, cur->rt, cur->nil);
            return *this;
        }
        // iter--
        const_iterator operator--(int){
            if(cur == nullptr || *this == cur->cbegin())
                throw invalid_iterator();
            const_iterator ret = *this;
            if(this->nd == cur->nil) nd = cur->last();
            else nd = cur->pre(nd->data->first, cur->rt, cur->nil);
            return ret;
        }
        // --iter
        const_iterator & operator--(){
            if(cur == nullptr || *this == cur->cbegin())
                throw invalid_iterator();
            if(this->nd == cur->nil) nd = cur->last();
            else nd = cur->pre(nd->data->first, cur->rt, cur->nil);
            return *this;
        }

        value_type & operator*() const{
            return *nd->data;
        }

        value_type* operator->() const noexcept{
            return nd->data;
        }

        bool operator==(const iterator &rhs) const { return (cur == rhs.cur && nd == rhs.nd); }
        bool operator==(const const_iterator &rhs) const { return (cur == rhs.cur && nd == rhs.nd); }
        bool operator!=(const iterator &rhs) const { return (cur != rhs.cur || nd != rhs.nd); }
        bool operator!=(const const_iterator &rhs) const { return (cur != rhs.cur || nd != rhs.nd); }//10 const !!!再各个地方的限制
    };
    
public:
    map() {
        nil = new node();
        rt = nullptr;
        sz = 0;
    }
    map(const map &other) {
        nil = new node();
        rt = nullptr;//
        copy(other);
    }

    map & operator=(const map &other) {
        if(this == &other)return *this;
        clear();
        rt = nullptr;
        copy(other);
        return *this;//11 return !
    }

    ~map() {
        del(rt);
        delete nil;
    }

    T & at(const Key &key) {
        node *ans = find(key,rt);
        if(ans == nil)throw index_out_of_bound();
        return ans->data->second;
    }
    const T & at(const Key &key) const {
        node *ans = find(key,rt);
        if(ans == nil)throw index_out_of_bound();
        return ans->data->second;
    }

    T & operator[](const Key &key) {//12 这里之前多了个throw!导致不过！
        node *ans = find(key,rt);
        if(ans == nil){
            ++sz;
            ans = insert(value_type(key,T()),rt);
            return ans->data->second;
        }
        return ans->data->second;
    }
    const T & operator[](const Key &key) const {
        node *ans = find(key,rt);
        if(ans == nil)throw index_out_of_bound();
        return ans->data->second;
    }

    iterator begin() {
        node *ans = fi();
        iterator res;res.nd = ans;res.cur = this;//13 const 的问题？？？
        return res;
    }
    const_iterator cbegin() const {
        node *ans = fi();
        const_iterator res(this,ans);//14 这里为什么非得const ????res.nd = ans;res.cur = this;
        return res;//if rt == nullptr reuturn nullptr
    }

    iterator end() {//15 空的元素
        node *ans = nil;
        iterator res;res.nd = ans;res.cur = this;
        return res;
    }
    const_iterator cend() const {
        node *ans = nil;
        const_iterator res(this,ans);
        return res;
    }

    bool empty() const {return (sz == 0);}

    size_t size() const {return sz;}

    void clear() {
        del(rt);//删根
        rt = nullptr;
        sz = 0;//16 sz!
    }

    pair<iterator, bool> insert(const value_type &x) {
        node *ans = find(x.first,rt);
        if(ans != nil){
        	pair<iterator,bool> a;
			a.first.nd = ans;
            a.first.cur = this;
			a.second = false;
			return a;//18 this 是一个const 指针？？
		}
		ans = insert(x,rt);
        ++sz;
        pair<iterator,bool> a;
        a.first.nd = ans;
        a.first.cur = this;
        a.second = true;
        return a;
    }

    void erase(iterator pos) {//??
        if(pos.cur != this || pos.nd == nil)throw invalid_iterator();
        --sz;
        erase(pos.nd->data->first,rt);
    }

    size_t count(const Key &key) const {
        node *ans = find(key,rt);
        if(ans == nil)return 0;
        return 1;
    }

    iterator find(const Key &key) {
        node *ans = find(key,rt);
        if(ans == nil)return end();
        iterator res;res.nd = ans;res.cur = this;
        return res;
    }
    const_iterator find(const Key &key) const {
        node *ans = find(key,rt);
        if(ans == nil)return cend();
        const_iterator res(this,ans);//res.nd = ans;res.cur = this;
        return res;
    }

};

}

#endif