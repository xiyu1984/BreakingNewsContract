#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>

//普通消息
struct Message{
    std::string                         msgUserAddress;         //消息发布者地址
    std::string                         msgContent;             //消息内容
    std::vector<std::string>            msgImages;              //图片链接列表
    std::vector<std::string>            msgUp;                  //观点点赞者地址列表
    std::vector<std::string>            msgDown;                //观点踩者地址列表

    uint64_t                            BlockNumber;            //该消息最后观点发布的区块号

    PLATON_SERIALIZE(Message, (msgUserAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber))
};

//观点，观点包括支持和反对
struct Viewpoint : public Message
{
    bool            point;          //支持true，反对false
    platon::u128    ViewpointID;    //观点唯一标识
    platon::u128    NewID;          //该观点对应的爆料标识
    PLATON_SERIALIZE_DERIVED(Viewpoint, Message, (point)(ViewpointID)(NewID))
};

//一条爆料
struct News: public Message
{
    std::string                             NewTitle;               //爆料标题
    platon::u128                            NewID;                  //爆料唯一标识
    std::vector<Viewpoint>                  Viewpoints;             //这条爆料后面跟帖的观点

    PLATON_SERIALIZE_DERIVED(News, Message, (NewTitle)(NewID)(Viewpoints))
};

//爆料摘要
struct NewsSummary
{
	std::string                             NewTitle;               //爆料标题
	platon::u128                            NewID;                  //爆料唯一标识
    uint16_t                                Supporter;              //支持观点数量
    uint16_t                                Opponents;              //反对观点数量
	uint16_t                                Up;                     //点赞数量
	uint16_t                                Down;                   //踩数量

    PLATON_SERIALIZE(NewsSummary, (NewTitle)(NewID)(Supporter)(Opponents)(Up)(Down))
};

struct UserInfo {
    std::string                                                     UserAddress;            //用户地址
    std::vector<NewsSummary>                                        UserNews;               //用户发布过的breaking news title and ID  
    int16_t                                                         UserCredibility;        //用户可信度，满分100，初始分数0，最低为-100？
    
    PLATON_SERIALIZE(UserInfo, (UserAddress)(UserNews)(UserCredibility))
};

CONTRACT BreakingNews: public platon::Contract
{

public:
    platon::StorageType<"BreakingNews"_n, std::list<News>>              mBreakingNews;      //存放breaking news
    platon::StorageType<"Users"_n, std::map<std::string, UserInfo>>     mUsers;             //存放用户信息，这个后续再考虑下要不要
    platon::StorageType<"NewsCount"_n, platon::u128>                    mNewsCount;         //当前已发布的news数量，自增用于生成爆料唯一标识
    platon::StorageType<"hashChain"_n, std::vector<platon::bytes>>      mNewsHashChain;     //爆料哈希链，后续爆料信息会滚动删除，链上仅存哈希链信息用于验证信息是否真实
    platon::StorageType<"Expired_VP"_n, std::vector<Viewpoint>>         mExVP;              //用于存放后续爆料信息滚动删除后，该爆料的支持、反对观点
};

PLATON_DISPATCH(BreakingNews)
