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

    mBreakingNews.self().push_back(curNews);

    //insert user
    UserInfo* userPtr = _getUser(curNews.msgauthorAddress);
    if (NULL == userPtr)
    {
        UserInfo curUser;
        curUser.UserAddress = curNews.msgauthorAddress;
        curUser.UserCredibility = 0;
        //curUser.UserNews.push_back(curNews.NewID);

        mUsers.self().push_back(curUser);
    }
    //else
    //{
    //    userPtr->UserNews.push_back(curNews.NewID);
    //}

    return "success";
}

std::string BreakingNews::createViewPoint(platon::u128 ID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime)
{
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

    mVP.self().push_back(curVP);

    //insert user
    UserInfo* userPtr = _getUser(curVP.msgauthorAddress);
    if (NULL == userPtr)
    {
        UserInfo curUser;
        curUser.UserAddress = curVP.msgauthorAddress;
        curUser.UserCredibility = 0;
        //curUser.UserNews.push_back(curVP.ViewpointID);

        mUsers.self().push_back(curUser);
    }
    //else
    //{
    //    userPtr->UserViewpoints.push_back(curVP.ViewpointID);
    //}

    //后续加入Viewpoint影响News可信度的代码
    /***********************************/

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

    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            //先消灭disLike中的记录
            newsItr->cancleDislike(userAddrStr);
            
            //再插入like列表中，注意查重
            newsItr->addLike(userAddrStr);
            
            break;
        }
    }

    return "success";
}

std::string BreakingNews::cancellikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            //消灭Like中的记录
            newsItr->cancleLike(userAddrStr);
            
            break;
        }
    }

    return "success";
}

std::string BreakingNews::dislikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            //先消灭Like中的记录
            newsItr->cancleLike(userAddrStr);
            
            //再插入disLike列表中，注意查重
            newsItr->addDislike(userAddrStr);
            
            break;
        }
    }

    return "success";
}

std::string BreakingNews::canceldislikeNews(platon::u128 newsID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto newsItr = mBreakingNews.self().begin(); newsItr != mBreakingNews.self().end(); ++newsItr)
    {
        if (newsItr->NewID == newsID)
        {
            //先消灭disLike中的记录
            newsItr->cancleDislike(userAddrStr);
            
            break;
        }
    }

    return "success";
}

//给Viewpoint（观点）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该Viewpoint的相反操作
std::string BreakingNews::likeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            //先消灭dislike中的记录
            vpItr->cancleDislike(userAddrStr);

            //再加入like中的记录
            vpItr->addLike(userAddrStr);

            break;
        }
    }

    return "success";
}

std::string BreakingNews::cancellikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            //先消灭like中的记录
            vpItr->cancleLike(userAddrStr);

            break;
        }
    }

    return "success";
}

std::string BreakingNews::dislikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            //先消灭Like中的记录
            vpItr->cancleLike(userAddrStr);

            //再加入Dislike中
            vpItr->addDislike(userAddrStr);

            break;
        }
    }

    return "success";
}

std::string BreakingNews::canceldislikeViewpoint(platon::u128 vpID)
{
    auto userAddress = platon::platon_origin();
    std::string userAddrStr = platon::encode(userAddress, hrp);

    for (auto vpItr = mVP.self().begin(); vpItr != mVP.self().end(); ++vpItr)
    {
        if (vpItr->ViewpointID == vpID)
        {
            //先消灭dislike中的记录
            vpItr->cancleDislike(userAddrStr);

            break;
        }
    }

    return "success";
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

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//News
//在以下接口中，会改变news可信度
void News::addLike(const std::string& userAddrStr)
{
	//再插入like列表中，注意查重
	auto sameItr = msgUp.begin();
	bool find = false;
	while (sameItr != msgUp.end())
	{
		if (*sameItr == userAddrStr)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgUp.push_back(userAddrStr);

		//改变可信度
		/*********************************/

	}
}

void News::cancleLike(const std::string& userAddrStr)
{
	//消灭Like中的记录
	auto LikeItr = msgUp.begin();
	while (LikeItr != msgUp.end())
	{
		if (*LikeItr == userAddrStr)
		{
			msgUp.erase(LikeItr);

			//改变可信度
			/*********************************/


			break;
		}

		++LikeItr;
	}
}

void News::addDislike(const std::string& userAddrStr)
{
	//再插入disLike列表中，注意查重
	auto sameItr = msgDown.begin();
	bool find = false;
	while (sameItr != msgDown.end())
	{
		if (*sameItr == userAddrStr)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgDown.push_back(userAddrStr);

        //改变可信度
        /*********************************/

	}
}

void News::cancleDislike(const std::string& userAddrStr)
{
	auto dislikeItr = msgDown.begin();
	while (dislikeItr != msgDown.end())
	{
		if (*dislikeItr == userAddrStr)
		{
			msgDown.erase(dislikeItr);

            //改变可信度
            /*********************************/


			break;
		}

		++dislikeItr;
	}
}


//////////////////////////////////////////////////////////////////////////
//Viewpoints
//在以下接口中，会改变VP可信度
void Viewpoint::addLike(const std::string& userAddrStr)
{
	//再插入like列表中，注意查重
	auto sameItr = msgUp.begin();
	bool find = false;
	while (sameItr != msgUp.end())
	{
		if (*sameItr == userAddrStr)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgUp.push_back(userAddrStr);

		//改变可信度
		/*********************************/

	}
}

void Viewpoint::cancleLike(const std::string& userAddrStr)
{
	//消灭Like中的记录
	auto LikeItr = msgUp.begin();
	while (LikeItr != msgUp.end())
	{
		if (*LikeItr == userAddrStr)
		{
			msgUp.erase(LikeItr);

			//改变可信度
			/*********************************/


			break;
		}

		++LikeItr;
	}
}

void Viewpoint::addDislike(const std::string& userAddrStr)
{
	//再插入disLike列表中，注意查重
	auto sameItr = msgDown.begin();
	bool find = false;
	while (sameItr != msgDown.end())
	{
		if (*sameItr == userAddrStr)
		{
			find = true;
			break;
		}

		++sameItr;
	}

	if (!find)
	{
		msgDown.push_back(userAddrStr);

        //改变可信度
        /*********************************/

	}
}

void Viewpoint::cancleDislike(const std::string& userAddrStr)
{
	auto dislikeItr = msgDown.begin();
	while (dislikeItr != msgDown.end())
	{
		if (*dislikeItr == userAddrStr)
		{
			msgDown.erase(dislikeItr);
			//改变可信度
			/*********************************/


			break;
		}

		++dislikeItr;
	}
}
