//priority queue.hpp
//last version
#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"
//#define std::min<npl>(x,y) ((x)<(y) ?(x) :(y))
#include<cmath>
namespace sjtu {//命名（字）空间 
template<typename T, class Compare = std::less<T>>//1 max heap,it seems that it's unusual,we can understand it by if(less(add,queue))pushdown

class priority_queue {
    typedef Compare Cmp;
private:
    Cmp cmp;//2 special
    struct node{
        node *l,*r;
        size_t npl;
        T *data;
    
        node():l(nullptr),r(nullptr),data(nullptr),npl(-1){}

        node(node *_l,node *_r,T * _data):l(_l),r(_r)
        {
            if(l==NULL||r==NULL){
                npl=0;
            }
            else npl=std::min<size_t>(l->npl,r->npl)+1;//13 attention namespace!!  why in the vector ,the std::cout<<   cannot be used?
            data=new T(*_data);
        }

        node(node *_l,node *_r,const T &_data):l(_l),r(_r)
        {
            if(l==NULL||r==NULL){
                npl=0;
            }
            else npl=std::min<size_t>(l->npl,r->npl)+1;
            data=new T(_data);
        }

        ~node(){
            if(data)delete data;// 3  delete 后并不变成NULL指针
        }
    };

    node *root;//4 empty rt
    size_t Size;

    void merge(node *other){
        root->l=merge(root->l,other->l);
    }
//5  假如先比较大小，可以规避这个问题，我总是学不会这步优化
    node * merge(node *cur,node *other){
    if(other==NULL)return cur;
    if(cur==NULL)return other;
    if(!cmp(*cur->data,*other->data)){//6 attention here.using example to understand it.
        cur->r=merge(cur->r,other);
        if(cur->l==NULL||cur->r==NULL)cur->npl=0;
        else cur->npl=std::min<size_t>(cur->l->npl,cur->r->npl)+1;
        if(!check(cur)){
            node *tmp=cur->l;
            cur->l=cur->r;
            cur->r=tmp;
        }
        return cur;//7 grasping this kinds of recursive function,it a good type.
        //especial when initial son is uncertail
    }
    else {
        other->r=merge(other->r,cur);
        if(other->l==NULL||other->r==NULL)other->npl=0;
        else other->npl=std::min<size_t>(other->l->npl,other->r->npl)+1;
        if(!check(other)){
            node *tmp=other->l;
            other->l=other->r;
            other->r=tmp;
        }
        return other;
    }//8 the code can be simplified.try to do it!
    }
    bool check(node *tmp){
        if(tmp->r==NULL)return true;
        if(tmp->l==NULL)return false ;//9 
        if(tmp->l->npl>=tmp->r->npl)return true;
        else return false ;
    }
node* copy(node *other){
    if(other==NULL)return NULL;
    node *tmp=new node(copy(other->l),copy(other->r),other->data);//给了空间
    return tmp;
}
//10 when son needs to create new space,using this way.
void del(node *rt){
    if(!rt)return ;
    if(rt->l)del(rt->l);
    if(rt->r)del(rt->r);
    delete rt;// 11   意思是假如我先删除，root的空间都没了，变成野指针了，delete后指针一般不为NULL 
}
public:
	priority_queue() {
        root=new node;
        Size=0;
    }
	priority_queue(const priority_queue &other) {
        root=new node;
        root->l=copy(other.root->l);
        Size=other.Size;
    }
	~priority_queue() {
        del(root);
    }
	priority_queue &operator=(const priority_queue &other) {
        if(this == & other){
            return *this;
        }
        del(root);
        root=new node;
        root->l=copy(other.root->l);
        Size=other.Size;
        return *this;
    }
	const T & top() const {
        if(!root->l)throw container_is_empty();
        return *root->l->data;//
	}
	void push(const T &e) {
        node *add=new node(NULL,NULL,e);//12  const !!!cannot transformed to T automatically
        root->l=merge(root->l,add);//copy constructors......
        ++Size;
	}
	void pop() {
        if(Size==0)throw container_is_empty();
        node *tmp=root->l;
        root->l=merge(root->l->l,root->l->r);
        delete tmp;
        --Size;
	}
	size_t size() const {
        return Size;
	}
	bool empty() const {
        if(Size==0)return true;
        return false;
	}
	void merge(priority_queue &other) {
        merge(other.root);
        other.root->l=NULL;
       // 13 other.del(other.root);空间被你回收了，外面能够调用析构函数，就是~ 而你析构函数里面的就是一个delete 
        Size+=other.Size;
        other.Size=0;
        return ;
	}
};
}
#endif