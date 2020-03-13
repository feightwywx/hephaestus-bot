#include <iostream>
#include <set>
#include <sstream>

#include "cqcppsdk/cqcppsdk.h"

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

int i_msgchk = 0; //当前读取的信息计数 [0,2]
string tempmsg[3]{"null", "null", "null"}; //暂存最近3条消息 初始化防止空string参与比较

CQ_INIT {
    on_enable([] { logging::info("启用", "插件已启用"); });

    on_group_message([](const GroupMessageEvent &event) {
        switch (i_msgchk) {
        case 0: {
            tempmsg[0] = event.message;
            if (tempmsg[0] == tempmsg[2] && tempmsg[0] != tempmsg[1]) { //将最新一条消息和更早的两条比对
                try {
                    send_message(event.target, event.message);
                } catch (ApiError &) {}
            }
            i_msgchk++;
            break;
        }

        case 1: {
            tempmsg[1] = event.message;
            if (tempmsg[1] == tempmsg[0] && tempmsg[1] != tempmsg[2]) {
                try {
                    send_message(event.target, event.message);
                } catch (ApiError &) {}
            }
            i_msgchk++;
            break;
        }

        case 2: {
            tempmsg[2] = event.message;
            if (tempmsg[2] == tempmsg[1] && tempmsg[2] != tempmsg[0]) {
                try {
                    send_message(event.target, event.message);
                } catch (ApiError &) {}
            }
            i_msgchk = 0;
            break;
        }
        };
        event.block();
    });

    on_friend_add([](const FriendAddEvent &event) {
        try {
            send_message(event.target, "你好，我是一个复读机");
            send_message(event.target, "接下来我会开始复读你说的话");
        } catch (ApiError &) {}
        event.block();
    });

    on_private_message([](const PrivateMessageEvent &event) {
        if(event.sub_type == PrivateMessageEvent::SubType::FRIEND){ //只接受好友私聊
        try{
            send_message(event.target,event.message);
        } catch(ApiError &){}
        }
        event.block();
    });
}