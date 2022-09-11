//B+tree.hpp
//(11)
#include <functional>
#include <cstddef>
//#include "exception.hpp"
#include <fstream>
#include <cstring>
#include "MemoryRiver.hpp"
#define cp(a,b) (b = (a > b) ? (a) : (b))   //cmp key
namespace sjtu {
    template <class Key, class Value>
    class BTree {
    public:
    	static const int M = 3;
		static const int L = 3;
		struct Treenode{
            public:
            int size,flag,nxt_flag,location;
            int parent,child[M << 1];
			Key key[M << 1];
            
            Treenode(int _flag = 0,int _nxt_flag = 0,int _size = 0){
                size = _size;
                flag = _flag;
                parent = -1;
                nxt_flag = _nxt_flag;
            }
            Treenode(const Treenode & t){
                size = t.size;
                parent = t.parent;
                flag = t.flag;
                nxt_flag = t.nxt_flag;
                for(int i = 0;i < size; ++i){
                    child[i] = t.child[i];
                    if(i != size - 1)key[i] = t.key[i];
                }
            }
		};//tree node

		struct Datanode{
            public:
            int size,nxt,prv,parent,flag,location;
            Key key[L << 1];
            Value data[L << 1];
            Datanode(const Datanode &other){
            	size = other.size;
            	nxt = other.nxt,
				prv = other.prv,
				parent = other.parent,
				flag = other.flag;
				for(int i = 0;i < size; ++i)key[i] = other.key[i],data[i] = other.data[i];
			}
			Datanode &operator=(const Datanode & other){
				size = other.size;
            	nxt = other.nxt,
				prv = other.prv,
				parent = other.parent,
				flag = other.flag;
				for(int i = 0;i < size; ++i)key[i] = other.key[i],data[i] = other.data[i];
				return *this;
			}
			
            Datanode(){prv = -1;nxt = -1;parent = -1;flag = 1;size = 0;}//-1作为空的标记
            Datanode(int prv_ind,int nxt_ind,int _flag = 1,int _parent = -1,int _size = 0):prv(prv_ind),nxt(nxt_ind),parent(_parent),size(_size),flag(_flag){}//flag只能等于1
		};
		struct Pair{
            public:
            int location;
            Datanode f;
            Pair(Datanode  _f,int _location){//seventeen ! &
                location = _location;
                f = _f;
            }
            Pair(const Pair &t){
            	location = t.location;
            	f = t.f;
			}
			Pair & operator=(const Pair & t){
				location = t.location;
            	f = t.f;
			}
            Pair(){}
        };
		Treenode *rt; //根
        int fst,nil;//储存第一个数据块位置 必要??
        int size;//插入数据个数
        Key km;
        fstream tree,data;
        string tfile,dfile;

        MemoryRiver<Treenode>  treeop;
        MemoryRiver<Datanode>  dataop; // 储存了数据块的个数,int 分别存 个数，fst，nil

    public:
        BTree() {
            tfile = "E:/treefile.txt";
            dfile = "E:/nodefile.txt";
            treeop.initialise(tfile);//initialise
            dataop.initialise(dfile);
            size = 0;
            fst = -1;
            rt = nullptr;
            km = 0;
        }

        BTree(const char *fname) {
            tfile = "E:/treefile.txt";
            dfile = "E:/nodefile.txt";
            treeop.initialise(tfile);//initialise
            dataop.initialise(dfile);
            size = 0;
            fst = -1;
            rt = nullptr;
        }

        ~BTree() {
            clear();
            if(rt)delete rt;
        }

        /*寻找key对应的数据块的指针值*/
        /*同时更新父亲指针*/
        /*当要找到在第一块时的特殊情况此处也应该包含*/
        //ten 空间分配问题导致我一直at有问题！！！！
        Pair find_data_block(const Key &key,Treenode &rot,int flocation){//rt 的 location 一定要更新
			//Treenode *tmp = new Treenode;//four !
            Treenode tmp;
            if(flocation == rt->location)treeop.read(rot,flocation);

            int i = 0;
            Datanode f;
            for(;i < rot.size - 1; ++i){//i 是他能加入的儿子 
                if(key < rot.key[i]){
                    if(rot.nxt_flag == 1){
                        dataop.read(f,rot.child[i]);
                        if(f.parent != flocation)f.parent = flocation;//attention here TODO update in the code ! dataop.update(f,rot.child[i]);//这个地方可能不一定要改 6.9
                        int tp = rot.child[i];
                        return Pair(f,tp);
                    }
                    treeop.read(tmp,rot.child[i]);//read tmp!
                    //这里之前的逻辑？？
                    if(tmp.parent != flocation){
                        tmp.parent = flocation;
                        treeop.update(tmp,rot.child[i]);
                    }
                    return find_data_block(key,tmp,rot.child[i]);//two ??TODO change
                }
            }//i is key's index

            //当是最大的时
            if(i == rot.size - 1 && rot.nxt_flag == 1){//five 没找到出来的这里可能有一大块bug ,已修复
                dataop.read(f,rot.child[i]);//fifteen TODO 为什么这里会乱码？？？？ 
                if(f.parent != flocation)f.parent = flocation;
                int tp = rot.child[i];//seven rot 也不能用！ 
                //if(rot != rt)delete rot;
                return Pair(f,tp); 
            }
            if(i == rot.size - 1 && rot.nxt_flag == 0){
                treeop.read(tmp,rot.child[rot.size - 1]);//这里还是可以用tmp的
				if(tmp.parent != flocation){
                    tmp.parent = flocation;
                    treeop.update(tmp,rot.child[i]);
                }
                return find_data_block(key,tmp,rot.child[rot.size - 1]);//tmp的地址
            }
            return Pair();//不可能有没有的情况
        }
//checked

        /*对某一数据节点，加入数据并将数据整体后移*/
        int add(Datanode &ad,const Key &key,const Value &value){//确保存在以后再来改
            for(int i = 0;i < ad.size; ++i){
                if(key < ad.key[i]){
                    ad.size++;
                    for(int j = ad.size - 1;j > i; --j)ad.key[j] = ad.key[j-1],ad.data[j] = ad.data[j-1];
                    ad.data[i] = value;
                    ad.key[i] = key;
                    return i;
                    //ad.size ++
                }
                //看是不是加在第一个
            }
            ad.data[ad.size] = value;
            ad.key[ad.size] = key;
            ad.size++;
            return ad.size - 1;//其实不用return ,共用一块空间
        }
//checked
        void clear() {
            fst = -1;
            size = 0;
            tree.open(tfile,std::ios::out);
            data.open(dfile,std::ios::out);
            tree.close();
            data.close();
        }

        //fst一定不要更新，因为分裂都是在后面，之后erase也注意，留前面一块，则不需更新，注意fst，nxt只需在数据块中进行更改
        
        bool insert(const Key &key, const Value &value) {
            //当插入一个元素时
            cp(key,km);
            if(size == 0){//这个地方可以考虑先不开根节点
                rt = new Treenode;
                rt->flag = 0,rt->nxt_flag = 1;
                rt ->size = 1;
                //dataop.write_info(1,0);//给0为写入个数
                Datanode add;
                add.size = 1;
                add.prv = -1;
                add.nxt = -1;
                add.flag = 1;
                add.key[0] = key;
                add.data[0] = value;
                //rt->key_min = key;

                add.parent = treeop.write(*rt);
                
                rt->child[0] = dataop.write(add);//写入新内容
                rt->location = add.parent;
				rt->parent = -1;
                treeop.update(*rt,add.parent);

                size++;
                nil = fst = rt->child[0];//nil 可以不要
                return true;
            }

            int location;
			Pair f = find_data_block(key,*rt,rt->location);//数据块的Location
			location = f.location;
			if(location == -1)return false;
            Datanode tmp = f.f;

            if(check_insert_datanode(tmp,key))return false;
            int index = add(tmp,key,value);
                                                            //这里没考虑放在第一个对父亲的更改
			//if(index == 0)add_renew_father(tmp,f.location);//补上    one（这次debug用one two three! 
            if(tmp.size > L){
                int cur = tmp.parent;
                Datasplit_and_write_update(tmp,location);//split cur to two parts and return a bool and change the value !
            }//插入不可能到数据块第一个，所以不用改上级
            else dataop.update(tmp,location);
            ++size;
            return true;
        }

        /*插入时检查是否已经存在*/
        bool check_insert_datanode(Datanode & tmp,Key key){
            for(int i = 0;i < tmp.size; ++i){
                if(tmp.key[i] == key)return true;//有一样的
            }
            return false;
        }

        /*数据块的分裂与写入*/
        void Datasplit_and_write_update(Datanode &tmp,int location){//写入并且处理！
            int cursize = tmp.size - (tmp.size + 1) / 2;//后面的大小 
            Datanode nxt;
            Datanode ne(location,tmp.nxt,1,tmp.parent,cursize);//指针式
            int length = (tmp.size + 1) / 2;
            for(int i = (tmp.size + 1) / 2;i < tmp.size; ++i){//前面是0 - tmp.size + 1 >> 1 - 1;个数 (size + 1)/2 个
                ne.key[i - length] = tmp.key[i];tmp.key[i] = Key();//??
                ne.data[i - length] = tmp.data[i];tmp.data[i] = Value();
            }
            tmp.size = length;
            if(tmp.nxt == -1){
            	Datanode t;
                tmp.nxt = dataop.write(ne);
                dataop.read(ne,tmp.nxt);
                dataop.read(t,tmp.nxt);
                dataop.update(tmp,location);
            }
            else{
                dataop.read(nxt,tmp.nxt);
                nxt.prv = dataop.write(ne);
                tmp.nxt = nxt.prv;//已经更新好了
                dataop.update(nxt,ne.nxt);
                dataop.update(tmp,location);//fst 不用更新
            }//更新nxt与fst
            int location1 = location,location2 = tmp.nxt;
            insert_check_father_and_op(tmp,ne,location1,location2);//数据已经更新
            //这个工具函数可以有
        }

        /*两个数据块父亲的检查与操作*/
        /*自身已写入*/
        void insert_check_father_and_op(Datanode & a,Datanode & b,int location1,int location2){//更新对叶子节点的索引，并且check
            if(a.parent == -1)return ;//     这个地方不对！
            Treenode pa;
            treeop.read(pa,a.parent);
            //pa.key_min = 
            pa.nxt_flag = 1;
            pa.flag = 0;
            int location = a.parent;
                //直接加入
                int i = 0;
                for(;i < pa.size - 1; ++i){//key is less than child
                    if(pa.key[i] > a.key[0]){
                        //checked
                        for(int j = pa.size;j > i + 1; --j)pa.child[j] = pa.child[j-1];//child从i + 1 开始，
                        for(int j = pa.size - 1;j > i; --j)pa.key[j] = pa.key[j-1];//key从i开始后移
                        pa.child[i + 1] = location2;//对应关系
                        pa.key[i] = b.key[0];
                        pa.size++;//对儿子的信息已经更新完毕

                        check_recursive_split_and_update(pa,location);
                        return ;
                    }
                }//原来-1是没有东西的
                //checked 6.9

                pa.key[pa.size - 1] = b.key[0];
                pa.child[pa.size] = location2;
                pa.size++;
                check_recursive_split_and_update(pa,location);
                return ;
        }
//注意根的大小特判 

        /*树节点的检查与分裂，同时写入自身update*/
        void check_recursive_split_and_update(Treenode & cur,int location){//six cur need to be renewed !
            if(cur.size <= M){
                treeop.update(cur,location);
                return ;
            }
            //cur.size , 前面 (size + 1)/2 个，0-(size + 1)/2 -1 child
            Treenode add;
            add.nxt_flag = cur.nxt_flag;
            add.flag = cur.flag;
            add.size = cur.size - (cur.size + 1) / 2;//要这么多个孩子
            add.parent = cur.parent;

            int cursize = (cur.size + 1) / 2;
            for(int i = (cur.size + 1)/2; i < cur.size; ++i){//分裂模式基本一样
                add.child[i-cursize] = cur.child[i];
                if(i != cur.size - 1)add.key[i-cursize] = cur.key[i];//key值最后一个没有
            }//自动掉下中间那个值
            //add.key_min = cur.key[cursize - 1];//!!!!
            cur.size = cursize;//不可以先更新！
            int flocation = cur.parent;
            int location1 = treeop.write(add);//可行！，后面直接update即可！，大大提高了某些地方的效率 TODO
            treeop.update(cur,location);

            Key key_add = cur.key[cursize - 1];//之前里面的这个Key已经没用

            if(flocation == -1){//记得修复子代的父亲下标，或者在每次通过父亲找儿子时check下是不是相同，不同则更改写回
                Treenode *virtual_rt = new Treenode;//two !
                virtual_rt->flag = 0,virtual_rt->nxt_flag = cur.flag,virtual_rt->size = 2;
                virtual_rt->child[0] = location;virtual_rt->child[1] = location1;
                virtual_rt->key[0] = key_add;
                delete rt;//rt 不可直接用！
                rt = virtual_rt;
                rt->parent = -1;
                rt->location = treeop.write(*rt);//??出问题的点就在于假如vector中没有那就可能重复写入！
                treeop.update(*rt,rt->location);//更新rt location ! !
            }
            else {//如果上级还有
                //不用key_min,随便哪个key都可以用，都不小于后面的那个
                Treenode pa;
                treeop.read(pa,flocation);
                for(int i = 0;i < pa.size - 1; ++i){
                    if(cur.key[0] < pa.key[i]){
                        for(int j = pa.size;j > i + 1; --j)pa.child[j] = pa.child[j-1];//in a fixed mode
                        for(int j = pa.size - 1;j > i; --j)pa.key[j] = pa.key[j-1];
                        pa.size++;
                        pa.child[i + 1] = location1;
                        pa.key[i] = key_add;
                        //这个非常有用，就是删除后多出来的key
                        //treeop.update(cur,location);//six 上面已经有了
                        check_recursive_split_and_update(pa,flocation);
                        return ;
                    }
                }//没找到时!!!!
                pa.size++;
                pa.child[pa.size - 1] = location1;
                pa.key[pa.size - 2] = key_add;
                //这个非常有用，就是删除后多出来的key
                //treeop.update(cur,location);//six
                check_recursive_split_and_update(pa,flocation);//每一个用key找的地方都应该去这样地特判一下
                return ;
            }//key[cursize - 1]有大用处
        }


//insert part checked !

//目前看起来插入部分没什么问题
        bool modify(const Key &key, const Value &value) {//改变value不改变key ? ?
            Pair receive = find_data_block(key,rt);
            int location = receive.location;
            if(location == -1)return false;
            Datanode tmp = receive.f;
            for(int i = 0;i < tmp.size; ++i){
                if(tmp.key[i] == key){
                    tmp.data[i] = value;
                    dataop.update(tmp,location);//更改完成写入
                    return true;
                }
            }
            return false;
        }

        Value at(const Key &key) {
            Pair receive = find_data_block(key,*rt,rt->location);
            int location = receive.location;
            if(location == -1)return false;
            Datanode tmp = receive.f;
            if(location == -1)return Value();
            for(int i = 0;i < tmp.size; ++i){
                if(tmp.key[i] == key){
                    return tmp.data[i];
                }
            }
            return Value();
        }


//modify & at checked ! 


        /*删除元素，对外接口*///注意之前的fst和nxt在数据块更新时的更新，注意看
        //解决方法，永远和前面合并，如果删除的是第一个，那么特殊处理即可
        bool erase(const Key &key) {
            //同样考虑清楚情况，胜利就在眼前
            //考虑 M + 1 / 2与 M
            if(size == 0 || !(rt))return false;
            Pair receive = find_data_block(key,*rt,rt->location);
            int location = receive.location;
            if(location == -1)return false;
            Datanode cur = receive.f;

            bool ok = false;
            for(int i = 0;i < cur.size; ++i){if(cur.key[i] == key)ok = true;}
            if(!ok)return false;//没有这个元素
            minor(cur,key,location);
            
            size--;

            if(cur.size >= (L + 1) / 2)dataop.update(cur,location);//eleven !
            else{
                Data_borrow_or_union_update(cur,location);
            }
            return true;
        }

        /*处理数据块，借儿子或者合并*/
        void Data_borrow_or_union_update(Datanode & cur,int location){
            //注意fst,nxt的更新与使用
            Datanode tmp;
            if(cur.prv == -1 && cur.nxt == -1){
            	dataop.update(cur,location);
                return ;
			}
            if(cur.prv != -1){
                dataop.read(tmp,cur.prv);
                if(tmp.parent == cur.parent){
                    //借儿子
                    if(tmp.size > (L + 1) / 2){//L !
                        for(int i = cur.size;i > 0; --i)cur.key[i] = cur.key[i - 1];
                        for(int i = cur.size; i > 0; --i)cur.data[i] = cur.data[i - 1];
                        cur.key[0] = tmp.key[tmp.size - 1];//这里有bug大哥，
                        cur.data[0] = tmp.data[tmp.size - 1];
                        tmp.size--;
                        cur.size++;
                        add_renew_father(cur,location);//
                        dataop.update(cur,location);
                        dataop.update(tmp,cur.prv);
                        return ;
                    }
                    //
                    else {
                        Data_union_update_father(tmp,cur,cur.prv,location);//这样改一下好像就行了
                        return ;//twelve !
                    }
                }//两个至少有有一个在同一块
            }
            //借儿子
            if(cur.nxt != -1){
                dataop.read(tmp,cur.nxt);
                if(tmp.parent == cur.parent){
                    if(tmp.size > (L + 1) / 2){
                        Key minor_key = tmp.key[0];
                        cur.key[cur.size] = tmp.key[0];
                        cur.data[cur.size] = tmp.data[0];
                        for(int i = 0;i < tmp.size - 1; ++i)tmp.key[i] = tmp.key[i + 1];
                        for(int i = 0; i < tmp.size - 1; ++i)tmp.data[i] = tmp.data[i + 1];
                        tmp.size--;
                        cur.size++;
                        minor_renew_father(tmp,cur.nxt,minor_key);//这个cur不需要更新
                        dataop.update(cur,location);
                        dataop.update(tmp,cur.nxt);
                        return ;
                    }
                    else {
                        Data_union_update_father(cur,tmp,location,cur.nxt);
                    }
                }//两个至少有有一个在同一块
            }
            return ;
        }
//这两个情况的函数调用不同！
        /*合并两个数据块并且必要时循环更新父亲（含合并，借儿子等等）*/
        //这个是cur 和 nxt 合并
        void Data_union_update_father(Datanode &cur,Datanode & tmp,int location1,int location2){//fst ! ! ! !
            int flocation = cur.parent;
            Treenode pa;
            treeop.read(pa,cur.parent);//cur是前一个 
            for(int i = 0;i < pa.size - 1; ++i){
                if(pa.key[i] == cur.key[0] || cur.key[0] < pa.key[0] && (i = -1)){
                    i = i + 1;
                    cur.nxt = tmp.nxt;
                    for(int j = 0;j < tmp.size; ++j){//datanode
                        cur.key[cur.size + j] = tmp.key[j];
                        cur.data[cur.size + j] = tmp.data[j];
                    }
                    cur.size += tmp.size;
                    for(int j = i; j < pa.size - 2; ++j)pa.key[j] = pa.key[j + 1];//i has been added 1
                    for(int j = i + 1; j < pa.size - 1; ++j)pa.child[j] = pa.child[j + 1];
                    pa.size--;
                    dataop.update(cur,location1);//renew
                    dataop.Delete(location2);
                    erase_recursive_check_father_update(pa,cur.parent);
                    return ;
                }
            }
        	
        }

        /*检查父亲（cur）是否需要分裂*/
        //父亲的数据已经更新
        void erase_recursive_check_father_update(Treenode &cur,int location){
            if(cur.size >= (M + 1) / 2){
                treeop.update(cur,location);//完蛋，我没区分dataop还是treeop!!! 
                return ;
            }
            else {//需要合并或者借儿子
                int flocation = cur.parent;
                if(flocation == -1){//他是根时 
                	treeop.update(cur,location);
					return ;	
				}
                Treenode pa,tmp;
                treeop.read(pa,flocation);
                for(int i = 0;i < pa.size - 1; ++i){
                    if(i == 0 && cur.key[0] < pa.key[0]){//如果是第一个儿子，且需要合并或者借儿子
                        treeop.read(tmp,pa.child[1]);
                        if(tmp.size > (M + 1) / 2){
                            //borrow 从后面借，这个代码和前面的一样
                            Key minor_key = tmp.key[0];
                            cur.key[cur.size - 2] = tmp.key[0];//下标检查！！！ 5.29 
                            cur.child[cur.size - 1] = tmp.child[0];
                            for(int i = 0;i < tmp.size - 2; ++i)tmp.key[i] = tmp.key[i + 1];
                            for(int i = 0; i < tmp.size - 1; ++i)tmp.child[i] = tmp.child[i + 1];
                            tmp.size--;
                            cur.size++;
                            minor_renew_father(tmp,pa.child[1],minor_key);
                            treeop.update(cur,location);
                            treeop.update(tmp,pa.child[1]);
                            return ;
                        }
                        //需要合并
                        dataop.Delete(pa.child[1]);
                        Key b_key_min = pa.key[0];//!!!
                        for(int j = 0;j < pa.size - 2; ++j)pa.key[j] = pa.key[j + 1];//把0挤掉 TODO ？？？？？这里一开始写错了吗
                        for(int j = 1;j < pa.size - 1; ++j)pa.child[j] = pa.child[j + 1]; //把1挤掉
                        uni(cur,tmp,b_key_min);//留下cur
                        pa.size--;
                        treeop.update(pa,cur.parent);
                        treeop.update(cur,location);
                        
                        //treeop.read(tmp,cur.parent);
                        erase_recursive_check_father_update(pa,cur.parent);
                        return ;
                    }
                    //有前驱时，对前驱操作
                    if(i != 0 && cur.key[0] > pa.key[i - 1] && cur.key[0] <= pa.key[i]){// TODO ======!!!
                        treeop.read(tmp,pa.child[i]);//读前面那个
                        if(tmp.size > (M + 1) / 2){//当可以借儿子时
                            //后移
                            for(int i = cur.size - 1;i > 0; --i)cur.key[i] = cur.key[i - 1];
                            for(int i = cur.size; i > 0; --i)cur.child[i] = cur.child[i - 1];
                            cur.key[0] = tmp.key[tmp.size - 2];//这里有bug大哥，   !!!!!!!!!!!!!!!!-2
                            cur.child[0] = tmp.child[tmp.size - 1];
                            tmp.size--;
                            cur.size++;
                            add_renew_father(cur,location);
                            treeop.update(cur,location);
                            treeop.update(tmp,pa.child[i]);
                            return ;
                        }
                        
                        int lc = pa.child[i];
                        Key b_key_min = pa.key[i];//从父亲找！,这里i的含义和下面不同
                        dataop.Delete(pa.child[i + 1]);//delete tmp
                        for(int j = i;j < pa.size - 2; ++j)pa.key[j] = pa.key[j + 1];//把 i挤掉
                        for(int j = i + 1;j < pa.size - 1; ++j)pa.child[j] = pa.child[j + 1]; 
                        uni(tmp,cur,b_key_min);//留下tmp
                        pa.size--;
                        treeop.update(pa,cur.parent);
                        treeop.update(tmp,lc);
                        erase_recursive_check_father_update(pa,cur.parent);//cur 父亲更新完了再check父亲自己
                        return ;
                    }
                }
                //当最大，特殊情况判断
                /*这一堆和上面一样，只是一个特判而已，不过I的含义不太一样*/
                if(cur.key[0] > pa.key[pa.size - 2]){
                    int i = pa.size - 1;
                    treeop.read(tmp,pa.child[i - 1]);
                    if(tmp.size > (M + 1) / 2){//当可以借儿子时
                        for(int i = cur.size - 1;i > 0; --i)cur.key[i] = cur.key[i - 1];
                        for(int i = cur.size; i > 0; --i)cur.child[i] = cur.child[i - 1];//这个地方好像tmp也要移   flag
						// 下标   datanode 和 treenode flag 这里的下标应该是没有问题的 6.9
						
                        cur.key[0] = tmp.key[tmp.size - 2];//这里有bug大哥，
                        cur.child[0] = tmp.child[tmp.size - 1];
                        tmp.size--;
                        cur.size++;
                        add_renew_father(cur,location);
                        treeop.update(cur,location);
                        treeop.update(tmp,pa.child[i - 1]);
                        return ;
                    }
                    //如果不能借儿子
                    int lc = pa.child[i - 1];
                    Key b_key_min = pa.key[i - 1];
                    dataop.Delete(pa.child[i]);
                   // for(int j = i;j < pa.size - 2; ++j)pa.key[j] = pa.key[j + 1];//把 i挤掉
                   // for(int j = i;j < pa.size - 1; ++j)pa.child[j] = pa.child[j + 1]; 
                    uni(tmp,cur,b_key_min);//留下tmp
                    pa.size--;
                    treeop.update(pa,cur.parent);
                    treeop.update(tmp,lc);
                    
                    erase_recursive_check_father_update(pa,cur.parent);
                    return ;
                }
            }
        }


        /*合并两个树节点*/
        void uni(Treenode & a,Treenode & b,Key b_key_min){//bug 补全一个关键字
            a.key[a.size - 1] = b_key_min;
            for(int i = 0;i < b.size - 1; ++i){
                a.key[a.size + i] = b.key[i];
            }
            for(int i = 0;i < b.size; ++i){
                a.child[a.size + i] = b.child[i];
            }
            a.size += b.size;            
        }

        /*从数据块中删除元素*/
        bool minor(Datanode &mi,const Key & key,int location){
            for(int i = 0;i < mi.size; ++i){
                if(key == mi.key[i]){
                    Key minor_key = mi.key[i];
                    for(int j = i;j < mi.size - 1; ++j){
                        mi.key[j] = mi.key[j + 1];
                        mi.data[j] = mi.data[j + 1];
                    }
                    mi.size--;
                    if(i == 0){
                        //renew father;//两种情况，一种在最前面，一种在中间，在最前面就继续往前找就行了，知道不是最前面或者到根
                        //上面插入只有一种情况要更新父节点
                        minor_renew_father(mi,location,minor_key);//但是代码部分是一样的，只是上面只有一种会进到add种
                        //add好像根本不需要renew????根据推导
                    }
                    return true;
                }
            }
            return false; 
        }


        /*当头变小时，（子块中借儿子）后对父亲的更新*/
        void add_renew_father(Datanode & cur,int location){
            int flocation = cur.parent;
            Treenode pa;
            treeop.read(pa,cur.parent);//一定有父亲
            if(pa.size == 1)return ;//不用renew
            //这个add是add在后面一个，必须得改吧
            while(pa.size != 1 && cur.key[0] < pa.key[0]){//往上找到第一个需要改的
                flocation = pa.location;
                if(flocation == -1)return ;//不用改了,遇到了rt
                treeop.read(pa,flocation);
            }
            if(pa.size == 1)return ;//变大是minor的可能性
            //Insert不可能变小，除非在第一个
            for(int i = 1;i < pa.size - 1; ++i){
                if(i < pa.size - 2 && cur.key[0] > pa.key[i - 1] && cur.key[0] < pa.key[i]){
                    pa.key[i] = cur.key[0];
                    treeop.update(pa,flocation);
                    return ;
                }
            }
            if(cur.key[0] < pa.key[pa.size - 2]){//上面有关key的下标可能需要更改 5.28号
                pa.key[pa.size - 2] = cur.key[0];
                treeop.update(pa,flocation);
                return ;
            }
        }
        /*重载*/
        void add_renew_father(Treenode & cur,int location){//实际上两个函数操作对象一样，都是叶子节点
            int flocation = cur.parent;
            if(cur.parent == -1)return ;//不一定有父亲
            Treenode pa;
            treeop.read(pa,cur.parent);
            if(pa.size == 1)return ;//不用renew
            while(pa.size != 1 && cur.key[0] < pa.key[0]){//往上找到第一个需要改的
                flocation = pa.location;
                if(flocation == -1)return ;//不用改了,遇到了rt
                treeop.read(pa,flocation);
            }
            if(pa.size == 1)return ;//变大是minor的可能性
            //Insert不可能变小，除非在第一个
            for(int i = 1;i < pa.size - 1; ++i){
                if(i < pa.size - 2 && cur.key[0] > pa.key[i - 1] && cur.key[0] < pa.key[i]){
                    pa.key[i] = cur.key[0];
                    treeop.update(pa,flocation);
                    return ;
                }
            }
            if(cur.key[0] < pa.key[pa.size - 2]){//上面有关key的下标可能需要更改 5.28号
                pa.key[pa.size - 2] = cur.key[0];
                treeop.update(pa,flocation);
                return ;
            }
            return ;
        }

        /*当头变大时更新父节点*/
        void minor_renew_father(Datanode & cur,int location,Value minor_key){//minor renew
            int flocation = cur.parent;
            Treenode pa;treeop.read(pa,cur.parent);
            if(pa.size == 1)return ;//不用renew
            //我这个方式好像不可能改第一个，不过minor里面有可能
            while(pa.size != 1 && minor_key < pa.key[0]){
                flocation = pa.parent;//thirteen
                if(flocation == -1)return ;//不用改了,遇到了rt
                treeop.read(pa,flocation);
            }
            if(pa.size == 1)return ;//变大是minor的可能性
            
            for(int i = 0;i < pa.size - 2; ++i){
                if(i < pa.size - 2 && cur.key[0] > pa.key[i] && cur.key[0] < pa.key[i + 1]){//TODO = 号？？？
                    pa.key[i] = cur.key[0];
                    treeop.update(pa,flocation);
                    return ;
                }
            }
            //最大！
            if(cur.key[0] > pa.key[pa.size - 2]){
                pa.key[pa.size - 2] = cur.key[0];
                treeop.update(pa,flocation);
                return ;
            }
            //看，之前想的复杂情况是可以分析清楚的，别着急就是了
        }
        //new
        
        /*重载*/
        void minor_renew_father(Treenode & cur,int location,Value minor_key){//minor renew
            int flocation = cur.parent;
            Treenode pa;treeop.read(pa,cur.parent);
            if(pa.size == 1)return ;//不用renew
            //我这个方式好像不可能改第一个，不过minor里面有可能
            while(pa.size != 1 && minor_key < pa.key[0]){
                flocation = pa.location;
                if(flocation == -1)return ;//不用改了,遇到了rt
                treeop.read(pa,flocation);
            }
            if(pa.size == 1)return ;//变大是minor的可能性
            
            for(int i = 0;i < pa.size - 2; ++i){
                if(i < pa.size - 2 && cur.key[0] > pa.key[i] && cur.key[0] < pa.key[i + 1]){//TODO = 号？？？
                    pa.key[i] = cur.key[0];
                    treeop.update(pa,flocation);
                    return ;
                }
            }
            //最大！
            if(cur.key[0] > pa.key[pa.size - 2]){
                pa.key[pa.size - 2] = cur.key[0];
                treeop.update(pa,flocation);
                return ;
            }
            //看，之前想的复杂情况是可以分析清楚的，别着急就是了
        }
//minor and these four checked !
       
        //迭代器在这里是一个能够指向特定数据的一个东西，如果。。则换一个数据块
        //还要储存一个用于invalid判断的
        class iterator {//指向一个树节点或者数据结点
        private:
            // Your private members go here
            int ind;
            Datanode t;
            BTree *cur;
        public:
            iterator() {//它的初值从哪里来??
                ind = -1;
                cur = this;
            }
            iterator(Datanode &_t,int _ind){
                ind = _ind;
                t = _t;
            }
            iterator(const iterator& other) {
                ind = other.ind;
                t = other.key;//迭代器含义能够有效地指出一个元素
                cur = other.cur;
            }

            // modify by iterator
            bool modify(const Value& value) {
                return modify(t.key[ind],t.data[ind]);
            }

            Key getKey() const {
                return t.key[ind];
            }

            Value getValue() const {
                return t.data[ind];
            }
            //x++
            iterator operator++(int) {
                iterator tmp = *this;
                int sz = t.size;
                if(ind < sz - 1){
                    tmp.ind++;
                    return tmp;
                }
                else {
                    Datanode n;
                    if(t.nxt == -1);
                    else dataop.read(n,t.nxt);
                    t = n;
                    ind = 0;
                    return tmp;
                }
            }
            //++x
            iterator& operator++() {
                int sz = t.size;
                if(ind < sz - 1){
                    ind++;
                    return *this;
                }
                else {
                    Datanode n;
                    if(t.nxt == -1);
                    else dataop.read(n,t.nxt);
                    t = n;
                    ind = 0;
                    return *this;
                }
            }
            //x--
            iterator operator--(int) {
                iterator tmp = *this;
                if(ind > 0){
                    ind--;
                    return tmp;
                }
                else{
                    Datanode n;
                    if(t.prv == -1);
                    else dataop.read(n,t.prv);
                    t = n;
                    ind = n.size - 1;
                    return tmp;
                }
            }

            iterator& operator--() {
                if(ind > 0){
                    ind--;
                    return *this;
                }
                else{
                    Datanode n;
                    if(t.prv == -1);
                    else dataop.read(n,t.prv);
                    t = n;
                    ind = n.size - 1;
                    return *this;
                }
            }

            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                if(cur != rhs.cur)return false;
                if(rhs.t.key[rhs.ind] == t.key[ind] && rhs.t.data[rhs.ind] == t.data[ind])return true;
                return false;
            }

            bool operator!=(const iterator& rhs) const {
                return !(*this == rhs); 
            }
        };
        
        iterator begin() {
            Datanode fi;
            dataop.read(fi,fst);
            iterator res(fi,0);
            return res;
        }
        
        // return an iterator to the end(the next element after the last)
        iterator end() {
            Pair receive = find_data_block(km,*rt,rt->location);
            Datanode tmp = receive.f;
            return iterator(tmp,tmp.size - 1);//可能不存在？？？，仍然存在疑问
        }

        iterator find(const Key &key) {
            Pair receive = find_data_block(key,*rt,rt->location);
            Datanode tmp = receive.f;
            for(int i = 0;i < tmp.size; ++i){
                if(tmp.key[i] == key){
                    return iterator(tmp,i);
                }
            }
            return iterator();
        }
        
        // return an iterator whose key is the smallest key greater or equal than 'key'??
        iterator lower_bound(const Key &key) {
            iterator t = find(key);
            if(t.ind == t.t.size-1){
                Datanode res;
                if(t.t.nxt == -1);
                else dataop.read(res,t.t.nxt);
                t.t = res;
                return iterator(res,0);
            }
            else return iterator(t.t,t.ind+1);
        }
    };
}  // namespace sjtu
