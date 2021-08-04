#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <list>

//普通消息
//struct Message{
//    std::string                         msgauthorAddress;         //消息发布者地址
//    std::string                         msgContent;             //消息内容
//    std::vector<std::string>            msgImages;              //图片链接列表
//    std::vector<std::string>            msgUp;                  //消息点赞者地址列表
//    std::vector<std::string>            msgDown;                //消息踩者地址列表
//
//    uint64_t                            BlockNumber;            //该消息最后观点发布的区块号
//
//    PLATON_SERIALIZE(Message, (msgauthorAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber))
//};

//观点，观点包括支持和反对
struct Viewpoint
{
    //观点消息头
    bool            point;          //支持true，反对false
    platon::u128    ViewpointID;    //观点唯一标识
    platon::u128    NewID;          //该观点对应的爆料标识

    //观点消息体
	std::string                         msgauthorAddress;         //消息发布者地址
	std::string                         msgContent;             //消息内容
	std::vector<std::string>            msgImages;              //图片链接列表
	std::vector<std::string>            msgUp;                  //消息点赞者地址列表
	std::vector<std::string>            msgDown;                //消息踩者地址列表

	uint64_t                            BlockNumber;            //该消息最后观点发布的区块号
    std::string                         createTime;             //该消息创建的时间（非确认时间）

    int16_t                             Credibility;        //用户可信度，满分100，初始分数0，最低为-100？

    PLATON_SERIALIZE(Viewpoint, (point)(ViewpointID)(NewID)(msgauthorAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber)(createTime)(Credibility))
};

//一条爆料
struct News
{
    //爆料消息头
    std::string                             NewTitle;               //爆料标题
    platon::u128                            NewID;                  //爆料唯一标识

	//爆料消息体
	std::string                         msgauthorAddress;         //消息发布者地址
	std::string                         msgContent;             //消息内容
	std::vector<std::string>            msgImages;              //图片链接列表
	std::vector<std::string>            msgUp;                  //消息点赞者地址列表
	std::vector<std::string>            msgDown;                //消息踩者地址列表

	uint64_t                            BlockNumber;            //该消息最后观点发布的区块号
    std::string                         createTime;             //该消息创建的时间（非确认时间）

    //该爆料跟帖的观点
    std::vector<Viewpoint>              Viewpoints;             //这条爆料后面跟帖的观点

    int16_t                             Credibility;        //用户可信度，满分100，初始分数0，最低为-100？

    PLATON_SERIALIZE(News, (NewTitle)(NewID)(msgauthorAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber)(createTime)(Viewpoints)(Credibility))
};

////爆料摘要，用于用户查询时返回数据
//struct NewsSummary
//{
//	std::string                             NewTitle;               //爆料标题
//	platon::u128                            NewID;                  //爆料唯一标识
//    uint16_t                                Supporter;              //支持观点数量
//    uint16_t                                Opponents;              //反对观点数量
//	uint16_t                                Up;                     //点赞数量
//	uint16_t                                Down;                   //踩数量
//
//    PLATON_SERIALIZE(NewsSummary, (NewTitle)(NewID)(Supporter)(Opponents)(Up)(Down))
//};

//用户信息
struct UserInfo {
    std::string                                                     UserAddress;            //用户地址
    std::vector<platon::u128>                                       UserNews;               //用户发布过的爆料的NewID，
    std::vector<platon::u128>                                       UserViewpoints;         //用户发布过的观点ID，
    int16_t                                                         UserCredibility;        //用户可信度，满分100，初始分数0，最低为-100？
    
    PLATON_SERIALIZE(UserInfo, (UserAddress)(UserNews)(UserViewpoints)(UserCredibility))
};

//用户信息，用于输出
struct UserSummary
{
    std::string                                                     UserAddress;            //用户地址
    int16_t                                                         UserCredibility;        //用户可信度，满分100，初始分数0，最低为-100？

    PLATON_SERIALIZE(UserSummary, (UserAddress)(UserCredibility))
};

//历史爆料哈希块
struct NewsHashBlock
{
    platon::u128            nhBlockNum;                 //哈希块号
    platon::bytes           newsHash;                   //哈希值

    PLATON_SERIALIZE(NewsHashBlock, (nhBlockNum)(newsHash))
};

CONTRACT BreakingNews: public platon::Contract
{

public:
    platon::StorageType<"BreakingNews"_n, std::list<std::shared_ptr<News>>>                 mBreakingNews;      //存放breaking news
    platon::StorageType<"Users"_n, std::list<std::shared_ptr<UserInfo>>>                    mUsers;             //存放用户信息，这个后续再考虑下要不要
    platon::StorageType<"NewsCount"_n, platon::u128>                                        mNewsCount;         //当前已发布的news、viewpoint编号，自增用于生成唯一标识
    platon::StorageType<"hashChain"_n, std::list<std::shared_ptr<NewsHashBlock>>>           mNewsHashChain;     //爆料哈希链，后续爆料信息会滚动删除，链上仅存哈希链信息用于验证信息是否真实
    platon::StorageType<"Viewpoints"_n, std::vector<std::shared_ptr<Viewpoint>>>            mVP;                //用于存放观点，观点单独存，便于查找
};

PLATON_DISPATCH(BreakingNews)
