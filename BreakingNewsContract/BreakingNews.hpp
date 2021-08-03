#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>

//普通消息
struct Message{
    std::string                         msgUserAddress;         //观点发布者地址
    std::string                         msgContent;             //观点内容
    std::vector<std::string>            msgImages;              //图片链接列表
    std::vector<std::string>            msgSupporter;           //观点点赞者地址列表
    std::vector<std::string>            msgOpponents;           //观点踩者地址列表

    PLATON_SERIALIZE(Message, (msgUserAddress)(msgContent)(msgImages)(msgSupporter)(msgSupporter))
};

//观点，观点包括支持和反对
struct Viewpoint : public Message
{
    bool            point;          //支持true，反对false
    PLATON_SERIALIZE_DERIVED(Viewpoint, Message, (point))
};

//一条爆料
struct News: public Message
{
    std::string                             NewTitle;               //爆料标题
    platon::u128                            NewID;                  //爆料唯一标识
    std::vector<Viewpoint>                  Viewpoints;             //这条爆料后面跟帖的观点

    PLATON_SERIALIZE_DERIVED(News, Message, (NewTitle)(NewID)(Viewpoints))
};

struct UserInfo {
    std::string                                                     UserAddress;            //用户地址
    std::vector<std::pair<std::string, platon::u128>>               UserNews;               //用户发布过的breaking news title and ID  
    int16_t                                                         UserCredibility;        //用户可信度，满分100，初始分数0，最低为-100？
    
    PLATON_SERIALIZE(UserInfo, (UserAddress)(UserNews)(UserCredibility))
};

CONTRACT BreakingNews: public platon::Contract
{

public:
    platon::StorageType<"BreakingNews"_n, std::list<News>>              mBreakingNews;      //存放breaking news
    platon::StorageType<"Users"_n, std::map<std::string, UserInfo>>     mUsers;             //存放用户信息
    platon::StorageType<"NewsCount"_n, platon::u128>                    mNewsCount;         //当前已发布的news数量，自增用于生成爆料唯一标识
    platon::StorageType<"hashChain"_n, std::vector<platon::bytes>>      mNewsHashChain;     //爆料哈希链，后续爆料信息会滚动删除，链上仅存哈希链信息用于验证信息是否真实
};
