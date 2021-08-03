#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>

//普通消息，可作为观点信息
struct Message{
    std::string                         msgUserAddress;         //观点发布者地址
    std::string                         msgContent;             //观点内容，json格式
    std::vector<std::string>            msgSupporter;           //观点支持者列表
    std::vector<std::string>            msgOpponents;           //观点反对者列表

    PLATON_SERIALIZE(Message, (msgUserAddress)(msgContent)(msgSupporter)(msgSupporter))
};

//一条爆料
struct News: public Message
{
    std::string                         NewTitle;               //爆料标题，内部会拼接成唯一标识
    std::vector<Message>                Viewpoints;             //这条爆料后面跟帖的观点

    PLATON_SERIALIZE_DERIVED(News, Message, (Viewpoints))
};

struct UserInfo {
    std::string                         UserAddress;            //用户地址
    std::vector<std::string>            UserNews;               //用户发布过的breaking news title
};

CONTRACT BreakingNews: public platon::Contract
{

public:
    platon::StorageType<"BreakingNews"_n, std::list<News>>              mBreakingNews;      //存放breaking news
    platon::StorageType<"Users"_n, std::map<std::string, UserInfo>>     mUsers;             //存放用户信息
    platon::StorageType<"NewsCount"_n, platon::u128>                    mNewsCount;         //当前已发布的news数量，用于拼接生成爆料唯名称唯一标识
    platon::StorageType<"hashChain"_n, std::vector<platon::bytes>>      mNewsHashChain;     //爆料哈希链，后续爆料信息会滚动删除，链上仅存哈希链信息用于验证信息是否真实
};
