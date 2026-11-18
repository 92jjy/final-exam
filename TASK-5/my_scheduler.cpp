#include "my_scheduler.hpp"

#include <cstdlib>

Action MyScheduler::decide(const View& v, int e){
    const Car& car=v.cars[e];
    const int floor=car.floor;
    int best_distance=kFloors+1;
    int target=-1;  

    for(int f=1;f<=kFloors;++f){
        if(f==floor||car.car_call[f]==-1){
            continue;
        }
        const int distance=std::abs(f-floor);
        if(distance<best_distance){
          best_distance=distance;
          target=f;//根据最小距离找到目标楼层
        } 
        
        
    }
    //接下来考虑如果本层有人要下该怎么办
    if(car.car_call[floor]!=-1){
        if(target!=-1){
            return target>floor? Action::OpenUp :Action::OpenDown;

        }
        return v.hall_down[floor]!=-1?Action::OpenDown : Action::OpenUp;

    }
    //在运行过程中没有人要下的时候到target
    if(target !=-1){
        return target>floor?Action::Up:Action::Down;

    }
    //如果这时候没有target接下来就寻找最近的厅外呼叫
    //在此之前需要遍历六个楼层来判断距离与时间
    int oldest_call=999;
    Action open_direction=Action::OpenUp;//方向
    for(int f=1;f<=kFloors;f++){
         int assign_distance=kFloors+1;
        int best_elevator=-1;
        const int up=v.hall_up[f];
        const int down=v.hall_down[f];
        if(up==-1&&down==-1){
            continue;
        }
        const bool go_up=up!=-1&&(down==-1||up<down);
        const int pressed_at=go_up?up:down;//记录楼层按下的时间，在后续判断上下时还要依据时间的长短作为依据
    //这一段遍历两个电梯，找到距离最短的电梯编号
        for(int i=0;i<kElevators;++i){
                if(v.cars[i].busy) {
        continue;
    }//但是只考虑距离是不够的，如果电梯在运行并且距离更近那么空闲的电梯还是没有事干，显然不合理
        int distance=std::abs(v.cars[i].floor-f);
        if(distance<assign_distance){
            assign_distance=distance;
            best_elevator=i;
        }
    }
     //实际上到这里就可以实现两个电梯的调度，关键就是两个电梯如何相互分配调度，两个电梯公用一个按钮
    //但是原则依旧是距离最短的电梯优先，如果说是距离一样，默认电梯0
    
    if (best_elevator != e) {
    continue;
}
        const int distance=std::abs(f-floor);
        //和之前一样找到最短距离，如果距离一样的话依据时间判断
        if(distance<best_distance||(distance==best_distance&&pressed_at<oldest_call)){
            target=f;
            best_distance=distance;
            oldest_call=pressed_at;
            open_direction=go_up?Action::OpenUp:Action::OpenDown;
        }

    }
    //在此之前已经找到目标
    if(target==floor){
        return open_direction;
    }
    if(target==-1){
        return Action::Idle;//没有目标就接着等
    }
    return target>floor?Action::Up:Action::Down;//走到目标

  


}