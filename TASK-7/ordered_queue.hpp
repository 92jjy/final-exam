#include<iostream>
#include <map>
#include <mutex>
#include <condition_variable>
#include <utility>


template <typename T>
class OrderedQueue {
        // 按 T::id 重排，id 从 0 开始连续
public:
   std::map<int,T>buffer_;//这里使用map，map会根据key自动排序
   int next_id=0;
   bool closed=false;//关闭之后变为true
   std::mutex mutex;//互斥锁，题目要求检测线程的数量为2,避免多个线程同时修改数据起冲突
   std::condition_variable cv;
public://依据map容器的特点自动排序
    void push(T item){
      std::lock_guard<std::mutex> lock(mutex);//添加互斥锁
      if(closed){
        return;
      }

      if(item.id<next_id){
        return;
      }
      //emplaceSTL插入的容器
      buffer_.emplace(item.id,std::move(item));//将检测结果发给buffer并用id作为排序
      cv.notify_one();//通知等待的线程，唤醒跟踪来任务了，不让他在等待
    };// 线程安全；id 小于下一个期望 id 的元素直接丢弃
    
    
    
    //在这里可以认为他是消费者线程
    bool pop(T& out){
      std::unique_lock<std::mutex> lock(mutex);//添加互斥锁
      cv.wait(lock,[&]{
        return buffer_.count(next_id)>0||closed;
      });//等待函数，等待0的到来或者是队列关闭
    auto it=buffer_.find(next_id);//一开始id为0
    //在这里要注意当队列关闭时这个if循环不会立马失效，他仍然会判断有无期望id来处理下一帧
    if(it!=buffer_.end()){
        out=std::move(it->second);
        buffer_.erase(it);
        ++next_id;
        return true;
    }//nextid在按照顺序自增，依次取出销毁
    if(buffer_.empty()){
        return false;
    }
//还有剩余帧但是队列已经关闭
    auto first=buffer_.begin();
    out =std::move(first->second);
    next_id=first->first+1;
    buffer_.erase(first);


    return true;


    };       // 阻塞，按 id 递增的顺序取出；close() 之后取完剩余元素返回 false
   
   
   
    void close(){
        {     std::lock_guard<std::mutex> lock(mutex);

            closed= true;

        }

        cv.notify_all();

    };           // 声明不会再有新元素，让跟踪线程不再继续等待
};