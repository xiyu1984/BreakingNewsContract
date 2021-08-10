#include "BreakingNews.hpp"

const std::string hrp = "atp";

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
        curUser.UserNews.push_back(curNews.NewID);

        mUsers.self().push_back(curUser);
    }
    else
    {
        userPtr->UserNews.push_back(curNews.NewID);
    }

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
        curUser.UserNews.push_back(curVP.ViewpointID);

        mUsers.self().push_back(curUser);
    }
    else
    {
        userPtr->UserViewpoints.push_back(curVP.ViewpointID);
    }

    return "success";
}

std::list<UserSummary> BreakingNews::getUsers()
{
    return std::list<UserSummary>();
}

std::list<News> BreakingNews::getNews()
{
    return std::list<News>();
}

//给news（爆料）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该news的相反操作
std::string BreakingNews::likeNews(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::cancellikeNews(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::dislikeNews(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::canceldislikeNews(platon::u128 newsID)
{
    return "success";
}

//给Viewpoint（观点）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该Viewpoint的相反操作
std::string BreakingNews::likeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::cancellikeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::dislikeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string BreakingNews::canceldislikeViewpoint(platon::u128 newsID)
{
    return "success";
}

//超级权限操作
//删帖
void BreakingNews::clear()
{

}

void BreakingNews::clearNews(platon::u128 newsID)
{

}

void BreakingNews::clearViewpoint(platon::u128 vpID)
{

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

