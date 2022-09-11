#ifndef SJTU_LINKED_HASHMAP_HPP_STD
#define SJTU_LINKED_HASHMAP_HPP_STD
//linked_hashmap.hpp
//TLE last version

//the confusing clear() ??
#include <cstddef>
#include <functional>
#include "utility.hpp"
#include "algorithm.hpp"
#include "exceptions.hpp"
#include "list.hpp"
namespace sjtu {
template <//1 four templates and need examplification !
        class Key,
        class Value,
        class Hash = std::hash<Key>,//2 Hash and Equal is given. 
        class Equal = std::equal_to<Key>
>
class linked_hashmap : public list<pair<const Key, Value> > {
public:
    using value_type = pair<const Key, Value>;
    using node_ = typename list<value_type>::node; 
    friend class BucketList;
private:

    using list<value_type>::head;//3 a good way to use the ele in base class!!!
    using list<value_type>::tail;//make a declaration here
    using list<value_type>::nil;

    class Node : public list<value_type>::node {
    public:
        Node *next,*prev;
        Node():list<value_type>::node(){
            next = nullptr;
            prev = nullptr;
        }
        Node(const value_type &value,Node *_prev = nullptr,Node *_next = nullptr,Node *_prv = nullptr,Node*_nxt = nullptr):list<value_type>::node(value){
             node_::nxt = _nxt;
             node_::prv = _prv;
             prev = _prev;
             next = _next; 
        }
        Node(const value_type *value,Node *_prev = nullptr,Node *_next = nullptr,Node *_prv = nullptr,Node*_nxt = nullptr):list<value_type>::node(*value){
             node_::nxt = _nxt;
             node_::prv = _prv;
             prev = _prev;
             next = _next; 
        }
        //4 invoke base ~ ()   automatically. !!!
    };
    class BucketList {
    public:
        Node *head;
        BucketList(){
            head = new Node;
        }
        ~BucketList(){
            delete head;
        }//5 make sure that the following ele are deleted or no need to delete!
        // because the class only holds a ele ,so normally it's not its responsibility to delete the following ele
        //you can design a function to delete the following ele.
        Node * find(const Key &o) {
             Node *tmp = head->next;
             while(tmp != NULL && !Equal()(tmp->val->first,o)){
                 tmp = tmp->next;
             }//6 abandon the tail!
             return tmp;
        }
        Node * insert(Node *add){
        	 if(head->next == nullptr){
        	 	head->next = add;
        	 	add->prev = head;
                add->next = nullptr;//开始我觉得反正新建的节点会有后继为 nullptr 但我忽视了这个resize()的点
        	 	return add;
			 }
             add->next = head->next;
             add->prev = head;
             head->next->prev = add;
             head->next = add;//7 no need to operate other class's ele in this class!!    list<value_type>::insert(nil,add);//看看还有没有别的这种错误 
			 return add;
        }
        Node * insert(const Key &k, const Value & o){
            Node *add = new Node({k,o});
            insert(add);
            return add;
        }
        Node * insert(const value_type &kv) {
            Node *add = new Node(kv);
            insert(add);
            return add;
        }
        Node * erase(const Key &k) {
            Node *tmp = find(k);
            if(tmp){
                tmp->prev->next = tmp->next;
                if(tmp->next!=NULL)tmp->next->prev = tmp->prev;//8 there is no need to operate other class's ele! list<value_type>::erase(tmp);//????
                tmp->next = tmp->prev = nullptr;
            }
            return tmp;
        }
    };

    const float ldfactor = 0.75;
    BucketList **hashtable;
    size_t capacity,sum;

    void resize(){
        capacity *= 2;
        BucketList **tmp = new BucketList*[capacity];
        memset( tmp, 0, capacity * sizeof(BucketList*));//9 memset when creating a pointer group!
        
        for(int i = 0; i < capacity/2 ; ++i){//10 traverse!
            if(hashtable[i]){
                Node *tp = hashtable[i]->head->next,*tpp;
                while(tp != nullptr){
                	tpp = tp->next;
                    size_t hash = Hash()(tp->val->first) % capacity;
                    if(!tmp[hash])tmp[hash] = new BucketList;// 11 create  if(!)  !!
                    tmp[hash]->insert(tp);
                    tp = tpp;
                }
                delete hashtable[i];//12 delete head and withdraw the space
                hashtable[i] = nullptr;
            }
        }
      /*  
       for (typename list<value_type>::node* q = head->nxt; q != nil; q = q->nxt) {
            Node* p = dynamic_cast<Node*>(q);//!!!!!
            size_t id = Hash()(q->val->first) % capacity ;
            if(!tmp[id])tmp[id] = new BucketList;
            Node*& h = tmp[id]->head;
            tmp[id]->insert(p);//插入
        }
        for(int i =0 ;i<capacity/2;++i){
            if(hashtable[i])delete hashtable[i];
        }*/
        delete [] hashtable;// withdraw the space
        hashtable = tmp;
    }
    
   void tabcopy(const linked_hashmap &other){//13 make its function clear that it's used to copy other!
       list<value_type>::head->nxt = list<value_type>::nil;
       list<value_type>::nil->prv = list<value_type>::head;
       list<value_type>::tail = list<value_type>::head;//14 tail is ........
       typename list<value_type>::node* tmp = other.list<value_type>::head->nxt;//15 make the compiler know that it's a typename. This is a common compile question!!
       while(tmp->nxt != NULL){//16 copy but not move 
           Node *add = new Node(tmp->val);
           size_t hash = Hash()(tmp->val->first) % capacity;
           if(!hashtable[hash])hashtable[hash] = new BucketList;//create space!
           hashtable[hash]->insert(add);
           list<value_type>::insert(nil,add);//17 use the base function properly
           tmp = tmp->nxt;
       }
   }//18 try to make your tool function clearly!!!
   // or you won't use it properly
   /*//
   void delspace(){//del container
       for(size_t i = 0;i < capacity; ++i){
           delete hashtable[i];//回收空间,调用析构函数
       }
       list<value_type>::head->nxt = list<value_type>::nil;
       list<value_type>::nil->prv = list<value_type>::head;
       list<value_type>::tail = list<value_type>::head;
       capacity = 0;
       sum = 0;
   }
   */
   void create(){
       hashtable = new BucketList *[capacity];
       memset(hashtable , 0 , capacity * sizeof(BucketList *));//19 no need to create in every index! just memset!
   }
public:
    using iterator = typename list<value_type>::iterator;//20 a good example to use the class in base
    using const_iterator = typename list<value_type>::const_iterator;

    linked_hashmap() {
        capacity = (1<<8);//21 ini size
        sum = 0;
        create();
    }
    linked_hashmap(const linked_hashmap &other) {
        sum = other.sum;
        capacity = other.capacity;
        create();
        tabcopy(other);//only copy!
    }
    linked_hashmap &operator=(const linked_hashmap &other) {
    	if(this == &other)return *this ;// delspace();// delete [] hashtable; // del中没删// capacity = other.capacity;
       clear();//22 the function of clear is clear the container and make the index empty but not delete (**)hashtable

       for(size_t i = 0;i < capacity; ++i){
           if(hashtable[i])delete hashtable[i];
       }
       delete [] hashtable;
       capacity = other.capacity;
       create();

        sum = other.sum;
        tabcopy(other);
        return *this ;
    }

    ~linked_hashmap() {//23 invoke automatically!
    //是不是我子类clear的问题
        for(size_t i = 0;i < capacity ; ++i){
            if(hashtable[i])delete hashtable[i];
        }
        sum = 0;
        capacity = 0;
        delete  [] hashtable;
    }
    Value &at(const Key &key) {
        size_t index = Hash()(key) % capacity;
        if(!hashtable[index])throw index_out_of_bound();
        Node *fid = hashtable[index]->find(key);
        if(fid == NULL)throw index_out_of_bound();
        return fid->val->second;
    }
    const Value &at(const Key &key) const {
        size_t index = Hash()(key) % capacity;
        if(!hashtable[index])throw index_out_of_bound();
        Node *fid = hashtable[index]->find(key);
        if(fid == NULL)throw index_out_of_bound();
        return fid->val->second;
        //return at(key);//23 simplify your code!
    }

    Value &operator[](const Key &key) {
        size_t index = Hash()(key) % capacity;
        if(!hashtable[index]){
        	hashtable[index] = new BucketList;
		}//25 before find ,make sure the space exists!
        Node *fid = hashtable[index]->find(key);
        if(!fid){
            return insert({key,Value()}).first->second;//25 the insertion!! there is need to search ,so just use insert
        }
        return fid->val->second;
    }
    const Value &operator[](const Key &key) const {//26 distinguish these two function ! it's distinguished by the value type??
        size_t index = Hash()(key) % capacity;
        if(!hashtable[index])throw index_out_of_bound();
        Node *fid = hashtable[index]->find(key);
        if(!fid)throw index_out_of_bound();
        return fid->val->second;
    }

    void clear() override{//这个clear的继承关系很复杂，还牵涉到高层的析构函数自动调用底层，底层又用clear 那它用的就是这里的， 
        list<value_type>::clear();// no need to delete again!! because val is in base...
        for(size_t i =0;i < capacity ; ++i)if(hashtable[i])hashtable[i]->head->next = nullptr;//如果不是空指针
        sum = 0;//clear can delete ele but not main?? or can delete main ele but make it clear!
    }

    pair<iterator, bool> insert(const value_type &value) {
        if(sum > capacity*ldfactor)
          resize();
        ++sum;
        
        size_t hash = Hash()(value.first) % capacity;
        if(!hashtable[hash])hashtable[hash] = new BucketList;
        
        Node *tmp = hashtable[hash]->find(value.first);
        if(tmp)return {iterator(tmp,this),false}; //why here??

        tmp = hashtable[hash]->insert(value);
        list<value_type>::insert(nil,tmp);//why???
        return {iterator(tmp,this),true};
    }

    iterator erase(iterator pos) override{
        if (pos.invalid(this)) {//30 this function in the base class.
            throw invalid_iterator();
        }
        --sum;
        size_t hash = Hash()(pos->first) % capacity;

        if(!hashtable[hash])hashtable[hash] = new BucketList;//  no such element
        Node *tmp = hashtable[hash]->find(pos->first);//29 here make it clear how the literator was used!????? iterator 的重载
        if(tmp == NULL)throw invalid_iterator();//28 throw correctly!
        if(tmp->next!=NULL){
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }// change
        else tmp->prev->next = nullptr;
        list<pair<const Key,Value>>::erase(pos);//!! is different from erase tmp!
        return iterator(tmp,this);//31 use the tool function correctly
    }

    size_t count(const Key &key) const {
        size_t hash = Hash()(key) % capacity;
        if(!hashtable[hash])return 0;
        Node *tmp = hashtable[hash]->find(key);
        if(tmp == NULL)return 0;
        return 1;
    }

    iterator find(const Key &key) {
        size_t hash = Hash()(key) % capacity;
        if(!hashtable[hash])return list<value_type>::end();
        Node *tmp = hashtable[hash]->find(key);
        if(tmp == NULL)return list<value_type>::end();//32 see the limit/request clearly
        return iterator(tmp,this);
    }
    const_iterator find(const Key &key) const {
        size_t hash = Hash()(key) % capacity;
        if(!hashtable[hash])return list<value_type>::cend();
        Node *tmp = hashtable[hash]->find(key);
        if(tmp == NULL)return list<value_type>::cend();
        return const_iterator(tmp,this);
    }
};
}
#endif