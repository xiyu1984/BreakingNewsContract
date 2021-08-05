#include "BreakingNews.hpp"

void BreakingNews::init()
{

}

std::string& BreakingNews::createNews(const std::string& title,
                                  const std::string& content, 
                                  std::vector<std::string>& image, 
                                  const std::string& createTime)
{
    return "success";
}

std::string& BreakingNews::createViewPoint(const std::string& title,
                                platon::u128 ID,
                                const std::string& content,
                                std::vector<std::string>& image,
                                bool isSupported,
                                const std::string& createTime)
{
    return "success";
}

UserSummary& BreakingNews::getUsers()
{
    return UserSummary();
}

News& BreakingNews::getNews()
{
    return News;
}

//给news（爆料）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该news的相反操作
std::string& BreakingNews::likeNews(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::cancellikeNews(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::dislikeNews(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::canceldislikeNews(platon::u128 newsID)
{
    return "success";
}

//给Viewpoint（观点）点赞的相关操作
//like和dislike操作中，需要先判断是否先前已经有针对该Viewpoint的相反操作
std::string& BreakingNews::likeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::cancellikeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::dislikeViewpoint(platon::u128 newsID)
{
    return "success";
}

std::string& BreakingNews::canceldislikeViewpoint(platon::u128 newsID)
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

