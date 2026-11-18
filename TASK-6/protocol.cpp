#include<iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstring>

#include"protocol.hpp"
//题目要求采取对 SOF 到 DATA 末尾的所有字节计算的 CRC-16/CCITT-FALSE
std::uint16_t crc16(const std::uint8_t* data, std::size_t len){
std::uint16_t crc = 0xFFFF;//题目给出的初始默认值

    for (std::size_t i = 0; i < len; ++i) {
        crc ^= static_cast<std::uint16_t>(data[i]) << 8;//转换为16位向高位左移8位异或

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {//通过0x8000判断第一位是否为1
                crc = static_cast<std::uint16_t>(
                    (crc << 1) ^ 0x1021
                );
            } else {
                crc = static_cast<std::uint16_t>(
                    crc << 1
                );
            }
        }
    }

    return crc;//返回校验，在encodeAPI里最后两位附加校验

};

std::vector<std::uint8_t> encode(const GimbalCmd& cmd){
std::vector<std::uint8_t> frame;
    frame.reserve(18);//18字节

    //  帧头：3字节
    frame.push_back(0xA5);  // SOF
    frame.push_back(13);    // LEN,计算出来n=13
    frame.push_back(0x01);  // CMD

    // 2. time_ms：4字节，小端序
    for (int i = 0; i < 4; ++i) {
        frame.push_back(
            static_cast<std::uint8_t>(
                (cmd.time_ms >> (8 * i)) & 0xFF
            )
        );
    }

    //放入yaw
    std::uint32_t bits;
    static_assert(sizeof(float) == sizeof(bits));//检查四字皆

    std::memcpy(&bits, &cmd.yaw, 4);//复制cmd到bits

    for (int i = 0; i < 4; ++i) {
        frame.push_back(
            static_cast<std::uint8_t>(
                (bits >> (8 * i)) & 0xFF//题目要求使用小端序，底字节放在最前面
            )
        );
    }

    // 放入pitch
    std::memcpy(&bits, &cmd.pitch, 4);

    for (int i = 0; i < 4; ++i) {
        frame.push_back(
            static_cast<std::uint8_t>(
                (bits >> (8 * i)) & 0xFF
            )
        );
    }

    //  放入fire
    frame.push_back(cmd.fire);

    // 调用crc，协议要求计算前16校验
    std::uint16_t crc = crc16(frame.data(), frame.size());

    //  放入校验码
    frame.push_back(static_cast<std::uint8_t>(crc & 0xFF));
    frame.push_back(static_cast<std::uint8_t>((crc >> 8) & 0xFF));

    return frame;

};   // 打包成完整的一帧（共 18 字节）
//上位机接受状态数据，解析状态


//这个feed的长度n随意说明一次性可能录入多帧或者不足一帧，要用后面的next检查完整

    void Decoder:: feed(const std::uint8_t* data, std::size_t len){
          if (len == 0) return;

        buffer_.insert(
            buffer_.end(),
            data,
            data + len
        );

    }; // 喂入一次串口读取得到的字节，长度任意
bool Decoder:: next(GimbalState& out) {

    while(true){
    //  寻找帧头，这里一开始不能先判断大小是否小于22
    while (!buffer_.empty() && buffer_[0] != 0xA5) {
        buffer_.erase(buffer_.begin());
    }
  // 2. 判断数据是否足够
    if (buffer_.size() < 22) {
        return false;
    }
  
    // 检查长度和命令字，是否为17和0x02
    if (buffer_[1] != 17 || buffer_[2] != 0x02) {
           buffer_.erase(buffer_.begin());//有必要删除错误的帧头，不删下一次next还会卡在这里
         continue;
    }

    // CRC16校验
    uint16_t crc = crc16(buffer_.data(), 20);

    uint16_t received_crc =
        buffer_[20] | (buffer_[21] << 8);
//校验是否一致
    if (crc != received_crc) {
        buffer_.erase(buffer_.begin());
        continue;
    }

    // 解析数据
    GimbalState state{};

    // time_ms
    for (int i = 0; i < 4; i++) {
        state.time_ms |=
            uint32_t(buffer_[3 + i]) << (8 * i);
    }

    // yaw、pitch、bullet_speed
    uint32_t bits;

    std::memcpy(&bits, &buffer_[7], 4);
    std::memcpy(&state.yaw, &bits, 4);

    std::memcpy(&bits, &buffer_[11], 4);
    std::memcpy(&state.pitch, &bits, 4);

    std::memcpy(&bits, &buffer_[15], 4);
    std::memcpy(&state.bullet_speed, &bits, 4);

    // enemy_color
    state.enemy_color = buffer_[19];

    //  输出结果
    out = state;

    //  删除已经解析的22字节
    buffer_.erase(buffer_.begin(), buffer_.begin() + 22);

    return true;
}

             }             // 取出一帧解析好的 GimbalState，没有完整的帧时返回 false
