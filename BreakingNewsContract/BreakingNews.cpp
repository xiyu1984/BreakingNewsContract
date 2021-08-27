#include "BreakingNews.hpp"

const std::string hrp = "lat";

void BreakingNews::init()
{
    _mOwner.self() = std::pair<platon::Address, bool>(platon::platon_origin(), true);
    mNewsCount.self() = 0;
}

std::string BreakingNews::getOwner()
{
    return platon::encode(_mOwner.self().first, hrp);
}

std::string BreakingNews::createNews(const std::string& title,
                                  const std::string& content, 
                                  std::vector<std::string>& image, 
                                  const std::string& createTime)
{
    ++mNewsCount.self();

    //insert news
    News curNews;
    curNews.NewTitle = title;
    curNews.NewID = mNewsCount.self();
    auto authorAddress = platon::platon_origin();
    curNews.msgauthorAddress = platon::encode(authorAddress, hrp);
    curNews.msgContent = content;
    curNews.msgImages = image;
    curNews.BlockNumber = platon_block_number();
    curNews.createTime = createTime;
    curNews.Credibility = 0;

    //get user
    UserInfo* userPtr = _getUser(curNews.msgauthorAddress);
    //下面这个判断主要是为了调试
    if (NULL == userPtr)
    {
        PLATON_EMIT_EVENT1(AddNews, "Error: NULL when _getUser", curNews);
        return "error";
    }
    //后续加入计算News可信度的代码
    /***********************************/
    curNews.Cn_author = _mSysParams.self().News_gama * userPtr->UserCredibility / _mSysParams.self().Coefficient;
    curNews.Credibility = curNews.Cn_author;

    userPtr->createNews_update(curNews.Credibility, this);

    mBreakingNews.self().push_back(curNews);
    PLATON_EMIT_EVENT1(AddNews, "Create News" , curNews);

    return "success";
}

std::string BreakingNews::createViewPoint(platon::u128 ID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime)
{
    //先判断news是否存在
    bool isFound = false;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == ID)
        {
            isFound = true;

            ++mNewsCount.self();

            //insert viewpoint
            Viewpoint curVP;
            curVP.point = isSupported;
            curVP.ViewpointID = mNewsCount.self();
            curVP.NewID = ID;
            auto authorAddress = platon::platon_origin();
            curVP.msgauthorAddress = platon::encode(authorAddress, hrp);
            curVP.msgContent = content;
            curVP.msgImages = image;
            curVP.BlockNumber = platon_block_number();
            curVP.createTime = createTime;
            curVP.Credibility = 0;

            //get user
            UserInfo* userPtr = _getUser(curVP.msgauthorAddress);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "Create Viewpoint", "error: NULL when _getUser");
                return "error: NULL when _getUser!";
            }
            //后续加入计算vp可信度、Viewpoint影响News可信度的代码
            /***********************************/
            int32_t isSupport = curVP.point ? 1 : -1;
            curVP.Cv_author = isSupport * _mSysParams.self().View_alpha * newsItr->Credibility / _mSysParams.self().Coefficient;
            curVP.Cv_N = _mSysParams.self().View_gama * userPtr->UserCredibility / _mSysParams.self().Coefficient;
            curVP.Credibility = curVP.Cv_author + curVP.Cv_N;

            userPtr->createView_update(curVP.Credibility, this);

            int32_t beforeCreNews_V = newsItr->Cn_V;
            newsItr->Cn_V = _mSysParams.self().rho * newsItr->Cn_V / _mSysParams.self().Coefficient +
                _mSysParams.self().News_alpha * curVP.Credibility * isSupport * (1 * _mSysParams.self().Coefficient - _mSysParams.self().rho) / _mSysParams.self().Coefficient;
            int32_t delta_Cn_V = newsItr->Cn_V - beforeCreNews_V;
            newsItr->Credibility += delta_Cn_V;
            newsItr->delta_Cn += delta_Cn_V;

            if ((newsItr->delta_Cn >= _mSysParams.self().News_threshold) ||
                (newsItr->delta_Cn <= -_mSysParams.self().News_threshold))
            {
                newsItr->updateNews(this);
            }

            mVP.self().push_back(curVP);
            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "Create Viewpoint" , "error: news not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "Create Viewpoint" , "success");
    return "success";
}

std::list<UserInfo> BreakingNews::getUsers()
{
    return mUsers.self();
}

std::list<News> BreakingNews::getNews()
{
    std::list<News> News_Output;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        News curNews = *newsItr;

        for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
        {
            if (curNews.NewID == vpItr->NewID)
            {
                curNews.Viewpoints.push_back(*vpItr);
            }
        }

        News_Output.push_back(curNews);
    }

    return News_Output;
}

//给news（爆料）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该news的相反操作
std::string BreakingNews::likeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }
            isFound = true;
            //先消灭disLike中的记录
            newsItr->cancleDislike(userPtr, this);
            
            //再插入like列表中，注意查重
            newsItr->addLike(userPtr, this);

            //判断news该变量是否累积到位
			if ((newsItr->delta_Cn >= _mSysParams.self().News_threshold) ||
				(newsItr->delta_Cn <= -_mSysParams.self().News_threshold))
			{
				newsItr->updateNews(this);
			}
            
            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "like news" , "error: news not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "like new" , "success");
    return "success";
}

std::string BreakingNews::cancellikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }
            isFound = true;
            //消灭Like中的记录
            newsItr->cancleLike(userPtr, this);

			//判断news该变量是否累积到位
			if ((newsItr->delta_Cn >= _mSysParams.self().News_threshold) ||
				(newsItr->delta_Cn <= -_mSysParams.self().News_threshold))
			{
				newsItr->updateNews(this);
			}
            
            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "cancel like news" , "error: news not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "cancel like news" , "success");
    return "success";
}

std::string BreakingNews::dislikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;
            //先消灭Like中的记录
            newsItr->cancleLike(userPtr, this);
            
            //再插入disLike列表中，注意查重
            newsItr->addDislike(userPtr, this);

			//判断news该变量是否累积到位
			if ((newsItr->delta_Cn >= _mSysParams.self().News_threshold) ||
				(newsItr->delta_Cn <= -_mSysParams.self().News_threshold))
			{
				newsItr->updateNews(this);
			}
            
            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "dislike news" , "error: news not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "dislike news" , "success");
    return "success";
}

std::string BreakingNews::canceldislikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;
            //先消灭disLike中的记录
            newsItr->cancleDislike(userPtr, this);

			//判断news该变量是否累积到位
			if ((newsItr->delta_Cn >= _mSysParams.self().News_threshold) ||
				(newsItr->delta_Cn <= -_mSysParams.self().News_threshold))
			{
				newsItr->updateNews(this);
			}
            
            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "cancel dislike news" , "error: news not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "cancel dislike news" , "success");
    return "success";
}

//给Viewpoint（观点）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该Viewpoint的相反操作
std::string BreakingNews::likeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like viewpoint", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;

            News* newsPtr = _getNews(vpItr->NewID);
            //下面这个判断主要是为了调试
            if (NULL == newsPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like viewpoint", "error: NULL when _getNews");
                return "error: NULL when _getNews";
            }

            //先消灭dislike中的记录
            vpItr->cancleDislike(userPtr, newsPtr, this);

            //再加入like中的记录
            vpItr->addLike(userPtr, newsPtr, this);

            //根据ΔCv累积量，判断是否更新相关user
            if ((vpItr->delta_Cv >= _mSysParams.self().View_threshold) || 
                (vpItr->delta_Cv <= -_mSysParams.self().View_threshold))
            {
                vpItr->updateView(this);
            }

            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "like Viewpoint" , "error: Viewpoint not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "like Viewpoint" , "success");
    return "success";
}

std::string BreakingNews::cancellikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;

            News* newsPtr = _getNews(vpItr->NewID);
            //下面这个判断主要是为了调试
            if (NULL == newsPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "cancel like viewpoint", "error: NULL when _getNews");
                return "error: NULL when _getNews";
            }

            //先消灭like中的记录
            vpItr->cancleLike(userPtr, newsPtr, this);

            //根据ΔCv累积量，判断是否更新相关user
            if ((vpItr->delta_Cv >= _mSysParams.self().View_threshold) ||
                (vpItr->delta_Cv <= -_mSysParams.self().View_threshold))
            {
                vpItr->updateView(this);
            }

            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "cancel like Viewpoint" , "error: Viewpoint not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "cancel like Viewpoint" , "success");
    return "success";
}

std::string BreakingNews::dislikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;

            News* newsPtr = _getNews(vpItr->NewID);
            //下面这个判断主要是为了调试
            if (NULL == newsPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "dislike viewpoint", "error: NULL when _getNews");
                return "error: NULL when _getNews";
            }

            //先消灭Like中的记录
            vpItr->cancleLike(userPtr, newsPtr, this);

            //再加入Dislike中
            vpItr->addDislike(userPtr, newsPtr, this);

            //根据ΔCv累积量，判断是否更新相关user
            if ((vpItr->delta_Cv >= _mSysParams.self().View_threshold) ||
                (vpItr->delta_Cv <= -_mSysParams.self().View_threshold))
            {
                vpItr->updateView(this);
            }

            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "dislike Viewpoint" , "error: Viewpoint not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "dislike Viewpoint" , "success");
    return "success";
}

std::string BreakingNews::canceldislikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    bool isFound = false;
    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            UserInfo* userPtr = _getUser(userAddrStr);
            //下面这个判断主要是为了调试
            if (NULL == userPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "like news", "error: NULL when _getUser");
                return "error: NULL when _getUser";
            }

            isFound = true;

            News* newsPtr = _getNews(vpItr->NewID);
            //下面这个判断主要是为了调试
            if (NULL == newsPtr)
            {
                PLATON_EMIT_EVENT1(BNMessage, "cancel dislike viewpoint", "error: NULL when _getNews");
                return "error: NULL when _getNews";
            }

            //先消灭dislike中的记录
            vpItr->cancleDislike(userPtr, newsPtr, this);

            //根据ΔCv累积量，判断是否更新相关user
            if ((vpItr->delta_Cv >= _mSysParams.self().View_threshold) ||
                (vpItr->delta_Cv <= -_mSysParams.self().View_threshold))
            {
                vpItr->updateView(this);
            }

            break;
        }
    }

    if (!isFound)
    {
        PLATON_EMIT_EVENT1(BNMessage, "cancel dislike Viewpoint" , "error: Viewpoint not found!");
        return "error: news not found!";
    }

    PLATON_EMIT_EVENT1(BNMessage, "cancel dislike Viewpoint" , "success");
    return "success";
}

//测试事件
bool BreakingNews::checkNews()
{
    std::list<News> News_Output;
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        News curNews = *newsItr;

        for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
        {
            if (curNews.NewID == vpItr->NewID)
            {
                curNews.Viewpoints.push_back(*vpItr);
            }
        }

        News_Output.push_back(curNews);
    }

    auto outPutNewsItr = News_Output.begin();
    if (News_Output.end() != outPutNewsItr)
    {
        PLATON_EMIT_EVENT1(AddNews, "Create News" , *outPutNewsItr);
    }
    else
    {
        PLATON_EMIT_EVENT1(AddNews, "Create News" , News());
    }

    return true;
}

//超级权限操作
//删帖
void BreakingNews::clear()
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        return;
    }

    mBreakingNews.self().clear();
    mUsers.self().clear();
    mNewsCount.self() = 0;
    mNewsHashChain.self().clear();
    mVP.self().clear();
}

void BreakingNews::clearNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        return;
    }

    auto newsItr = mBreakingNews.self().begin();
    while (newsItr != mBreakingNews.self().end())
    {
        if (newsItr->NewID == newsID)
        {
            mBreakingNews.self().erase(newsItr);
            break;
        }

        ++newsItr;
    }
}

void BreakingNews::clearViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    if (_mOwner.self().first != userAddress)
    {
        return;
    }

    auto vpItr = mVP.self().begin();
    while (vpItr != mVP.self().end())
    {
        if (vpItr->ViewpointID == vpID)
        {
            mVP.self().erase(vpItr);
            break;
        }

        ++vpItr;
    }
}

UserInfo* BreakingNews::_getUser(const std::string& userAddr)
{
    auto userItr = mUsers.self().begin();
    while (mUsers.self().end() != userItr)
    {
        if (userItr->UserAddress == userAddr)
        {
            return &(*userItr);
        }

        ++userItr;
    }

    //没有找到，则创建一个
    UserInfo curUser;
    curUser.UserAddress = userAddr;
    curUser.UserCredibility = _mSysParams.self().User_init;
    mUsers.self().push_back(curUser);
    auto r_usrItr = mUsers.self().rbegin();

    return &(*r_usrItr);
}

//BreakingNews class add interface
News* BreakingNews::_getNews(const platon::u128& newsID)
{
    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            return &(*newsItr);
        }
    }

    return NULL;
}

Viewpoint* BreakingNews::_getViewpoint(const platon::u128& vpID)
{
    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            return &(*vpItr);
        }
    }

    return NULL;    //后续有空都改成nullptr
}

sysParams* BreakingNews::_getSysParams()
{
    return &(_mSysParams.self());
}

void BreakingNews::_emit_bnmessage_event(const std::string& topic, const std::string& msg)
{
    PLATON_EMIT_EVENT1(BNMessage, topic, msg);
}

//////////////////////////////////////////////////////////////////////////
//News
//在以下接口中，会改变news可信度
void News::addLike(UserInfo* userPtr, BreakingNews* bnPtr)
{
	//再插入like列表中，注意查重
	auto sameItr = msgUp.begin();
	bool find = false;
	while (sameItr != msgUp.end())
	{
		if (*sameItr == userPtr->UserAddress)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgUp.push_back(userPtr->UserAddress);

		//改变可信度
		/*********************************/
        up_down_CreUpdate(userPtr, 1, bnPtr);
	}
}

void News::cancleLike(UserInfo* userPtr, BreakingNews* bnPtr)
{
	//消灭Like中的记录
	auto LikeItr = msgUp.begin();
	while (LikeItr != msgUp.end())
	{
		if (*LikeItr == userPtr->UserAddress)
		{
			msgUp.erase(LikeItr);

			//改变可信度
			/*********************************/
            up_down_CreUpdate(userPtr, -1, bnPtr);

			break;
		}

		++LikeItr;
	}
}

void News::addDislike(UserInfo* userPtr, BreakingNews* bnPtr)
{
	//再插入disLike列表中，注意查重
	auto sameItr = msgDown.begin();
	bool find = false;
	while (sameItr != msgDown.end())
	{
		if (*sameItr == userPtr->UserAddress)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgDown.push_back(userPtr->UserAddress);

        //改变可信度
        /*********************************/
        up_down_CreUpdate(userPtr, -1, bnPtr);
	}
}

void News::cancleDislike(UserInfo* userPtr, BreakingNews* bnPtr)
{
	auto dislikeItr = msgDown.begin();
	while (dislikeItr != msgDown.end())
	{
		if (*dislikeItr == userPtr->UserAddress)
		{
			msgDown.erase(dislikeItr);

            //改变可信度
            /*********************************/
            up_down_CreUpdate(userPtr, 1, bnPtr);

			break;
		}

		++dislikeItr;
	}
}

void News::updataWithCv(int32_t delta_Cv, int32_t isSupport, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t delta_Cn_by_Cv = spPtr->News_alpha * isSupport * delta_Cv * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;
    Cn_V += delta_Cn_by_Cv;
    Credibility += delta_Cn_by_Cv;
    delta_Cn += delta_Cn_by_Cv;
}

void News::updateNews(BreakingNews* bnPtr)
{
    //更新view
    for (auto vpItr = bnPtr->mVP.self().begin(); vpItr != bnPtr->mVP.self().end(); ++vpItr)
    {
        if (vpItr->NewID == NewID)
        {
            vpItr->delta_Cn_updata(delta_Cn, bnPtr);
        }
    }

    //更新user，只更新跟news直接相关的user
    //author
    UserInfo* authorPtr = bnPtr->_getUser(msgauthorAddress);
    //下面这个判断主要是为了调试
    if (NULL == authorPtr)
    {
        bnPtr->_emit_bnmessage_event("updateNews", "error: NULL when _getUser");
        return;
    }
    authorPtr->delta_News_updata_author(delta_Cn, bnPtr);

    //up users
    for (auto userAddrItr = msgUp.begin(); userAddrItr != msgUp.end(); ++userAddrItr)
    {
        UserInfo* userPtr = bnPtr->_getUser(*userAddrItr);
        //下面这个判断主要是为了调试
        if (NULL == userPtr)
        {
            bnPtr->_emit_bnmessage_event("updateNews", "error: NULL when _getUser");
            return;
        }
        userPtr->delta_News_update_up_down(delta_Cn, 1, bnPtr);
    }

    //down users
    for (auto userAddrItr = msgDown.begin(); userAddrItr != msgDown.end(); ++userAddrItr)
    {
        UserInfo* userPtr = bnPtr->_getUser(*userAddrItr);
        //下面这个判断主要是为了调试
        if (NULL == userPtr)
        {
            bnPtr->_emit_bnmessage_event("updateNews", "error: NULL when _getUser");
            return;
        }
        userPtr->delta_News_update_up_down(delta_Cn, -1, bnPtr);
    }

    delta_Cn = 0;
}

void News::up_down_CreUpdate(UserInfo* userPtr, int32_t coe, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cn_up_down = Cn_up_down;

    Cn_up_down = Cn_up_down * spPtr->rho / spPtr->Coefficient + 
        coe * spPtr->News_beta * userPtr->UserCredibility * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cn_up_down = Cn_up_down - before_Cn_up_down;
    Credibility += delta_Cn_up_down;
    delta_Cn += delta_Cn_up_down;
}


//////////////////////////////////////////////////////////////////////////
//Viewpoints
//在以下接口中，会改变VP可信度
void Viewpoint::addLike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr)
{
	//再插入like列表中，注意查重
	auto sameItr = msgUp.begin();
	bool find = false;
	while (sameItr != msgUp.end())
	{
		if (*sameItr == userPtr->UserAddress)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgUp.push_back(userPtr->UserAddress);

		//改变可信度
		/*********************************/
        up_down_CreUpdate(userPtr, newsPtr, 1, bnPtr);
	}
}

void Viewpoint::cancleLike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr)
{
	//消灭Like中的记录
	auto LikeItr = msgUp.begin();
	while (LikeItr != msgUp.end())
	{
		if (*LikeItr == userPtr->UserAddress)
		{
			msgUp.erase(LikeItr);

			//改变可信度
			/*********************************/
            up_down_CreUpdate(userPtr, newsPtr, -1, bnPtr);

			break;
		}

		++LikeItr;
	}
}

void Viewpoint::addDislike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr)
{
	//再插入disLike列表中，注意查重
	auto sameItr = msgDown.begin();
	bool find = false;
	while (sameItr != msgDown.end())
	{
		if (*sameItr == userPtr->UserAddress)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgDown.push_back(userPtr->UserAddress);

        //改变可信度
        /*********************************/
        up_down_CreUpdate(userPtr, newsPtr, -1, bnPtr);
	}
}

void Viewpoint::cancleDislike(UserInfo* userPtr, News* newsPtr, BreakingNews* bnPtr)
{
	auto dislikeItr = msgDown.begin();
	while (dislikeItr != msgDown.end())
	{
		if (*dislikeItr == userPtr->UserAddress)
		{
			msgDown.erase(dislikeItr);
			//改变可信度
			/*********************************/
            up_down_CreUpdate(userPtr, newsPtr, 1, bnPtr);

			break;
		}

		++dislikeItr;
	}
}

void Viewpoint::up_down_CreUpdate(UserInfo* userPtr, News* newsPtr, int32_t coe, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cv_up_down = Cv_up_down;
    Cv_up_down = Cv_up_down * spPtr->rho / spPtr->Coefficient +
        spPtr->View_beta * userPtr->UserCredibility * coe * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cv_up_down = Cv_up_down - before_Cv_up_down;
    Credibility += delta_Cv_up_down;
    delta_Cv += delta_Cv_up_down;

    //更新相关的news
    int32_t coe_isSupport = point ? 1 : -1;
    newsPtr->updataWithCv(delta_Cv_up_down, coe_isSupport, bnPtr);
}

void Viewpoint::updateView(BreakingNews* bnPtr)
{
    //update related users
    //author
    UserInfo* authorPtr = bnPtr->_getUser(msgauthorAddress);
    //下面这个判断主要是为了调试
    if (NULL == authorPtr)
    {
        bnPtr->_emit_bnmessage_event("updateView", "error: NULL when _getUser");
        return;
    }
    authorPtr->delta_View_updata_author(delta_Cv, bnPtr);

    //up
    for (auto userAddrItr = msgUp.begin(); userAddrItr != msgUp.end(); ++userAddrItr)
    {
        UserInfo* userPtr = bnPtr->_getUser(*userAddrItr);
        //下面这个判断主要是为了调试
        if (NULL == userPtr)
        {
            bnPtr->_emit_bnmessage_event("updateView", "error: NULL when _getUser");
            return;
        }
        userPtr->delta_View_update_up_down(delta_Cv, 1, bnPtr);
    }

    //down
    for (auto userAddrItr = msgDown.begin(); userAddrItr != msgDown.end(); ++userAddrItr)
    {
        UserInfo* userPtr = bnPtr->_getUser(*userAddrItr);
        //下面这个判断主要是为了调试
        if (NULL == userPtr)
        {
            bnPtr->_emit_bnmessage_event("updateView", "error: NULL when _getUser");
            return;
        }
        userPtr->delta_View_update_up_down(delta_Cv, -1, bnPtr);
    }

    delta_Cv = 0;
}

void Viewpoint::delta_Cn_updata(int32_t delta_Cn, BreakingNews* bnPtr)
{
    int32_t before_Cv_N = Cv_N;

    int32_t coe = point ? 1 : -1;
    sysParams* spPtr = bnPtr->_getSysParams();

    Cv_N += coe * delta_Cn * spPtr->View_alpha / spPtr->Coefficient;
    int32_t delta_Cv_N = Cv_N - before_Cv_N;

    Credibility += delta_Cv_N;
    delta_Cv += delta_Cv_N;
}

////////////////////////////////////////////////////////////////////////////////////
//UserInfo methods
void UserInfo::CredibilityAdjust(BreakingNews* bnPtr)
{
    if (UserCredibility <= 0)
    {
        //校正为output精度范围内小数点最后一位是1的数
        UserCredibility = 1;
    }
}

void UserInfo::createNews_update(int32_t C_News, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_N_author = Cu_N_author;
    Cu_N_author = Cu_N_author * spPtr->rho / spPtr->Coefficient +
        spPtr->User_alpha * C_News * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cu_Nauthor = Cu_N_author - before_Cu_N_author;
    UserCredibility += delta_Cu_Nauthor;

    CredibilityAdjust(bnPtr);
}

void UserInfo::createView_update(int32_t C_View, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_V_author = Cu_V_author;
    Cu_V_author = Cu_V_author * spPtr->rho / spPtr->Coefficient +
        spPtr->User_beta * C_View * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cu_Vauthor = Cu_V_author - before_Cu_V_author;
    UserCredibility += delta_Cu_Vauthor;

    CredibilityAdjust(bnPtr);
}

void UserInfo::delta_News_updata_author(int32_t delta_Cn, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_N_author = Cu_N_author;
    Cu_N_author += spPtr->User_alpha * delta_Cn * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;
    int32_t delta_Cu_Nauthor = Cu_N_author - before_Cu_N_author;

    UserCredibility += delta_Cu_Nauthor;

    CredibilityAdjust(bnPtr);
}

void UserInfo::delta_News_update_up_down(int32_t delta_Cn, int32_t isUp, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_N_up_down = Cu_N_up_down;
    Cu_N_up_down += spPtr->User_gama * isUp * delta_Cn * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cu_Nupdown = Cu_N_up_down - before_Cu_N_up_down;

    UserCredibility += delta_Cu_Nupdown;

    CredibilityAdjust(bnPtr);
}

void UserInfo::delta_View_updata_author(int32_t delta_Cv, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_V_author = Cu_V_author;
    Cu_V_author += spPtr->User_beta * delta_Cv * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;
    int32_t delta_Cu_Vauthor = Cu_V_author - before_Cu_V_author;

    UserCredibility += delta_Cu_Vauthor;

    CredibilityAdjust(bnPtr);
}

void UserInfo::delta_View_update_up_down(int32_t delta_Cv, int32_t isUp, BreakingNews* bnPtr)
{
    sysParams* spPtr = bnPtr->_getSysParams();

    int32_t before_Cu_V_up_down = Cu_V_up_down;
    Cu_V_up_down += spPtr->User_eta * isUp * delta_Cv * (1 * spPtr->Coefficient - spPtr->rho) / spPtr->Coefficient;

    int32_t delta_Cu_Vupdown = Cu_V_up_down - before_Cu_V_up_down;

    UserCredibility += delta_Cu_Vupdown;

    CredibilityAdjust(bnPtr);
}
