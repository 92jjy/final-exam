
#include "pipeline.hpp"
#include "simulation.hpp"
#include "latest.hpp"
#include "solver.hpp"
#include "ordered_queue.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <atomic>
#include <cstdio>

void run_pipeline(int max_infer_num) {
    Tracker tracker;//跟踪器
    Latest<Target> latest;//最新目标
    OrderedQueue<Detection> results;

    std::atomic<bool> stop{false};

    // 启动解算线程
    std::thread solver(solver_loop,
                       std::cref(latest),
                       std::cref(stop));//改动不大，与旧版本解算一致

    // 创建输入队列
    std::deque<Frame> frames;
    std::mutex mtx;
    std::condition_variable cv;
    bool camera_done = false;

    //  跟踪线程：按顺序处理检测结果
    std::thread tracking([&] {
        Detection d;

        while (results.pop(d)) {
            latest.write(tracker.track(d));
        }
    });//逐个跟踪图进行滤波处理，丢弃的也要传入id
    //  检测线程函数
    auto worker = [&] {
        while (true) {
            Frame f;

            {
                std::unique_lock<std::mutex> lock(mtx);//上锁
                cv.wait(lock, [&] {
                    return !frames.empty() || camera_done;
                });

                if (frames.empty() && camera_done)
                    break;

                f = frames.front();
                frames.pop_front();
            }
            // 执行检测
            Detection d = detect(f);

            // 提交检测结果
            results.push(d);
        }
    };
    // 创建多个检测线程，在这里创建两个
    std::vector<std::thread> workers;
    for (int i = 0; i < max_infer_num; ++i) {
        workers.emplace_back(worker);
    }
    //  相机线程
    std::thread camera([&] {
        auto start = Clock::now();
        for (int i = 0; i < kFrames; ++i) {
            std::this_thread::sleep_until(start + i * kPeriod);
            //产出图像f
            Frame f{i, Clock::now()};
            {
                std::lock_guard<std::mutex> lock(mtx);
                // 队列最多保留2帧，丢弃最旧帧
                if (frames.size() >= 2) {//在这里我选择在相机线程里丢弃，如果在检测线程里丢弃，相机的产出数度太快了，导致检测线程可能还没检测完这一帧就被当作最旧帧
                    Frame old = frames.front();
                    frames.pop_front();

                    // 丢弃帧也要提交占位结果
                    results.push(
                        Detection{old.id, old.stamp, false}
                    );
                }

                frames.push_back(f);
            }
            cv.notify_one();
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            camera_done = true;
        }
        cv.notify_all();
    });

    //  等待线程结束
    camera.join();
    for (auto& t : workers)//检测
        t.join();

    // 关闭结果队列
    results.close();
    tracking.join();
    // 停止解算线程
    stop = true;
    solver.join();
    //  输出结果
    std::printf(0
        "order_errors %d  detected %d/%d\n",
        tracker.order_errors(),
        tracker.detected_count(),
        kFrames
    );
}
