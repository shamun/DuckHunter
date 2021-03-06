#include "ClayEyes.h"
#include <opencv2/imgproc.hpp>

clay_eyes::clay_eyes()
{
	_bg = cv::imread(BG_IMGC);
	_safe_mask = cv::imread(SAFE_IMGC, cv::IMREAD_GRAYSCALE);
}
bool clay_eyes::GetContext(game_context* p_context)
{
	if(_frame_source.empty())
		return false;
	cv::Mat frame = _frame_source->nextFrame();
	cv::Mat resized_bg;
	cv::resize(_bg, resized_bg, frame.size());
	cv::Mat diff;
	cv::absdiff(resized_bg, frame, diff);
	cv::cvtColor(diff, diff, CV_BGR2GRAY);
	bitwise_or(diff, _safe_mask, diff);
	cv::threshold(diff, diff, 5, 255, cv::THRESH_BINARY);
	//cv::imshow("Diff", diff);
	std::vector<type_condition> cond;
	cond.resize(1);
	////////!!!!!!!!!!!!!!!!!!
	cond[0].rect[0].width = frame.cols/256 * 3; // original size is 256*224
	cond[0].rect[1].width = frame.cols/256 * 30;
	cond[0].rect[0].height = frame.rows/224 * 2;
	cond[0].rect[1].height = frame.rows/224 * 15;
	cond[0].bounding_size_ratio[0]= 0.3;
	cond[0].bounding_size_ratio[1]= 0.8;
	std::vector<int> check = {FEATURE_CHECK_SIZE | FEATURE_CHECK_BOUNDING_SIZE_RATIO};
	p_context->objects = FindObjects(diff, cond, check, cv::RETR_EXTERNAL, 0, _frame_source->GetOffset());
	for(auto& obj: p_context->objects)
	{
		obj.tag = TAG_CLAY;
	}
	return true;
}