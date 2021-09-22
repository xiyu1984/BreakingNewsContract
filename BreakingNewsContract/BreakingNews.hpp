#pragma once

#include <platon/platon.hpp>

#include <string>
#include <vector>
#include <map>
#include <list>

CONTRACT BreakingNews;
class News;

//用户信息
struct UserInfo {
    std::string                 UserAddress;            //用户地址
    int32_t                     UserCredibility = 0;        //用户可信度，满分100，初始分数0，最低为-100？

    //以下变量用于计算可信度
    int32_t                     Cu_N_author = 0;
    int32_t                     Cu_V_author = 0;
    int32_t                     Cu_N_up_down = 0;
    int32_t                     Cu_V_up_down = 0;

    PLATON_SERIALIZE(UserInfo, (UserAddress)(UserCredibility)
    (Cu_N_author)(Cu_V_author)(Cu_N_up_down)(Cu_V_up_down))

    //UserCredibility校正，其取值范围是(0,+∞)
    void CredibilityAdjust(BreakingNews* bnPtr);

    //create News带来的用户可信度改变，鼓励发帖
    void createNews_update(int32_t C_News, BreakingNews* bnPtr);

    //create viewpoint带来的用户可信度改变，鼓励发帖
    void createView_update(int32_t C_View, BreakingNews* bnPtr);

    //根据ΔCn改变用户可信度
    //author
    void delta_News_updata_author(int32_t delta_Cn, BreakingNews* bnPtr);
    //up/down
    void delta_News_update_up_down(int32_t delta_Cn, int32_t isUp, BreakingNews* bnPtr);
    
    //根据ΔCv改变用户可信度
    //author
    void delta_View_updata_author(int32_t delta_Cv, BreakingNews* bnPtr);
    //up/down
    void delta_View_update_up_down(int32_t delta_Cv, int32_t isUp, BreakingNews* bnPtr);
};

//观点，观点包括支持和反对
struct Viewpoint
{
    //观点消息头
    bool            point;          //支持true，反对false
    uint64_t    ViewpointID;    //观点唯一标识
    platon::u128    NewID;          //该观点对应的爆料标识

    //观点消息体
	std::string                         msgauthorAddress;         //消息发布者地址
	std::string                         msgContent;             //消息内容
	std::vector<std::string>            msgImages;              //图片链接列表
	std::vector<std::string>            msgUp;                  //消息点赞者地址列表
	std::vector<std::string>            msgDown;                //消息踩者地址列表

	uint64_t                            BlockNumber;            //该消息最后观点发布的区块号
    std::string                         createTime;             //该消息创建的时间（非确认时间）

    int32_t                             Credibility = 0;        //用户可信度，满分100，初始分数0，最低为-100？

    //以下变量用于计算可信度，黑客松中暂时放在一起，正式版中需要在数据结构中解耦
    int32_t                             Cv_N = 0;
    int32_t                             Cv_up_down = 0;
    int32_t                             Cv_author = 0;

    //Cv累积更改量
    int32_t                             delta_Cv = 0;

    PLATON_SERIALIZE(Viewpoint, 
        (point)(ViewpointID)(NewID)
    (msgauthorAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber)(createTime)(Credibility)
    (Cv_N)(Cv_up_down)(Cv_author)(delta_Cv))

	//在以下接口中，会计算可信度
	void addLike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr);
	void cancleLike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr);
	void addDislike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr);
	void cancleDislike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr);

    //由点赞、踩带来的可信度改变
    void up_down_CreUpdate(UserInfo* userPtr, News* newsPtr, int32_t coe, BreakingNews* bnPtr);

    //根据ΔCv，更新相关user
    void updateView(BreakingNews* bnPtr);

    //根据ΔCn，更新view，对于ΔCv的改变，只积累，不更新
    void delta_Cn_updata(int32_t delta_Cn, BreakingNews* bnPtr);
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

    int32_t                             Credibility = 0;        //用户可信度，满分100，初始分数0，最低为-100？

    //以下变量用于计算可信度
    int32_t                             Cn_V = 0;
    int32_t                             Cn_up_down = 0;
    int32_t                             Cn_author = 0;

    //Cn累积更改量
    int32_t                             delta_Cn = 0;

    PLATON_SERIALIZE(News, 
        (NewTitle)(NewID)
    (msgauthorAddress)(msgContent)(msgImages)(msgUp)(msgDown)(BlockNumber)(createTime)
    (Viewpoints)(Credibility)
        (Cn_V)(Cn_up_down)(Cn_author)(delta_Cn))

    //在以下接口中，会计算可信度
    void addLike(UserInfo* userPtr, BreakingNews* bnPtr);
    void cancleLike(UserInfo* userPtr, BreakingNews* bnPtr);
    void addDislike(UserInfo* userPtr, BreakingNews* bnPtr);
    void cancleDislike(UserInfo* userPtr, BreakingNews* bnPtr);

    //由于相关viewpoint的改变，带来的news credibility更新
    void updataWithCv(int32_t delta_Cv, int32_t isSupport, BreakingNews* bnPtr);

    //根据delta_Cn，更新相关View、user的可信度
    void updateNews(BreakingNews* bnPtr);

    //由点赞、踩带来的可信度改变
    void up_down_CreUpdate(UserInfo* userPtr, int32_t coe, BreakingNews* bnPtr);
};

//历史爆料哈希块
struct NewsHashBlock
{
    platon::u128            nhBlockNum;                 //哈希块号
    platon::bytes           newsHash;                   //哈希值

    PLATON_SERIALIZE(NewsHashBlock, (nhBlockNum)(newsHash))
};

//保存系统参数，例如用于计算可信度的模型参数
struct sysParams
{
    //output scale
    int32_t             sysNumericalScale = 100;

    //所有比例系数的缩放比例，默认小数点后面4位
    int32_t             Coefficient = 100;
    //计算参数
    //指数平均系数
    int32_t             rho = 9 * Coefficient / 10;

    //viepoints
    int32_t             View_alpha = 3 * Coefficient / 10;
    int32_t             View_beta = 4 * Coefficient / 10;
    int32_t             View_gama = 3 * Coefficient / 10;
    int32_t             View_threshold = 5 * sysNumericalScale;

    //news
    int32_t             News_alpha = 4 * Coefficient / 10;
    int32_t             News_beta = 3 * Coefficient / 10;
    int32_t             News_gama = 3 * Coefficient / 10;
    int32_t             News_threshold = 5 * sysNumericalScale;

    //user
    int32_t             User_init = 51 * sysNumericalScale;

    int32_t             User_alpha = 4 * Coefficient / 10;
    int32_t             User_beta = 3 * Coefficient / 10;
    int32_t             User_gama = 2 * Coefficient / 10;
    int32_t             User_eta = 1 * Coefficient / 10;

    PLATON_SERIALIZE(sysParams, 
        (sysNumericalScale)
        (rho)
        (View_alpha)(View_beta)(View_gama)
        (News_alpha)(News_beta)(News_gama)
        (User_alpha)(User_beta)(User_gama)(User_eta))
};

CONTRACT BreakingNews : public platon::Contract
{
    friend struct Viewpoint;
    friend struct News;
    friend struct UserInfo;
public:

    PLATON_EVENT1(AddNews, std::string, News)
    PLATON_EVENT1(BNMessage, std::string, std::string)

    ACTION void init();

    CONST std::string getOwner();

    ACTION std::string createNews(const std::string& title,
                                  const std::string& content, 
                                  std::vector<std::string>& image, 
                                  const std::string& createTime);

    ACTION std::string createViewPoint(platon::u128 ID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime);

    CONST std::list<UserInfo> getUsers();

    CONST std::list<News> getNews();

    //给news（爆料）点赞的相关操作
    //like和dislike操作中，需要先判断是否先前已经有针对该news的相反操作
    ACTION std::string likeNews(platon::u128 newsID);
    ACTION std::string cancellikeNews(platon::u128 newsID);
    ACTION std::string dislikeNews(platon::u128 newsID);
    ACTION std::string canceldislikeNews(platon::u128 newsID);

    //给Viewpoint（观点）点赞的相关操作
    //like和dislike操作中，需要先判断是否先前已经有针对该Viewpoint的相反操作
    ACTION std::string likeViewpoint(platon::u128 vpID);
    ACTION std::string cancellikeViewpoint(platon::u128 vpID);
    ACTION std::string dislikeViewpoint(platon::u128 vpID);
    ACTION std::string canceldislikeViewpoint(platon::u128 vpID);

    //测试事件
    ACTION bool checkNews();

    //超级权限操作
    //删帖
    ACTION void clear();
    ACTION void clearNews(platon::u128 newsID);
    ACTION void clearViewpoint(platon::u128 vpID);

private:
    //有则返回，无则先创建，此接口返回一定不会为NULL
    UserInfo* _getUser(const std::string& userAddr);

    //获取news，可能返回NULL
    News* _getNews(const platon::u128& newsID);

    //获取viewpoint，可能返回NULL
    Viewpoint* _getViewpoint(const platon::u128& vpID);

    //获取系统参数
    sysParams* _getSysParams();

    //抛出BNMessage事件
    void _emit_bnmessage_event(const std::string& topic, const std::string& msg);

public:
    platon::StorageType<"BreakingNews"_n, std::list<News>>                 mBreakingNews;      //存放breaking news
    platon::StorageType<"Users"_n, std::list<UserInfo>>                    mUsers;             //存放用户信息，这个后续再考虑下要不要
    platon::StorageType<"NewsCount"_n, platon::u128>                       mNewsCount;         //当前已发布的news、viewpoint编号，自增用于生成唯一标识
    platon::StorageType<"hashChain"_n, std::list<NewsHashBlock>>           mNewsHashChain;     //爆料哈希链，后续爆料信息会滚动删除，链上仅存哈希链信息用于验证信息是否真实
    platon::StorageType<"Viewpoints"_n, std::vector<Viewpoint>>            mVP;                //用于存放观点，观点单独存，便于查找

private:
    platon::StorageType<"Owner"_n, std::pair<platon::Address, bool>>       _mOwner;            //合约所有者地址，即部署者，黑客松中留个特殊权限
    platon::StorageType<"sysParam"_n, sysParams>                           _mSysParams;        //系统参数
};

PLATON_DISPATCH(BreakingNews, (init)(getOwner)(createNews)(createViewPoint)(getUsers)(getNews)(likeNews)(cancellikeNews)(dislikeNews)(canceldislikeNews)(likeViewpoint)(cancellikeViewpoint)(dislikeViewpoint)(canceldislikeViewpoint)(checkNews)(clear)(clearNews)(clearViewpoint))
