/*
 * infoview.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */


#include "infoview.h"
#include "multimediaapp.h"
#include <stdexcept>
#include "mediacontentcontroller.h"
#include "transcodingengine.h"

//#define SUPPORT_PLAYER
#ifdef __cplusplus
extern "C" {
#endif
void muxer_only();
#ifdef __cplusplus
}
#endif

InfoView::InfoView()
{

}

InfoView::~InfoView()
{

}

const char* InfoView::getedcfilename()
{
	return getmodel()->GetSelectedContent().name.c_str();
}

void InfoView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, change_optionview_cb, (void*)this);
		setinfo_tolist(m_list, getmodel()->GetSelectedContent());
#ifdef SUPPORT_PLAYER
		m_display = createdisplay(box);
		m_player.Create();
		m_player.SetMediaAndPrepare(getmodel()->GetSelectedContent().path.c_str(), m_display);
		m_player.Play();
#endif
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);
		m_pbpopup.Create(box, cancel_cb, (void*)this);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	}
}

void InfoView::destroyremains()
{
	m_list.Destroy();
	m_btnpack.Destroy();
	m_pbpopup.Destroy();
}


Evas_Object* InfoView::createdisplay(Evas_Object* box)
{
	Evas *evas = evas_object_evas_get(box);
	Evas_Object *image = evas_object_image_filled_add(evas);
	evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, 0.7);
	evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_pack_end(box,image);
	evas_object_show(image);
	return image;
}

void InfoView::add_defaultbtns(ButtonPack& btnpack)
{
	std::vector<BTPackParam> functionbtn_params;
	functionbtn_params.push_back(BTPackParam(NULL, "images/exit.png", InfoView::clicked_prev_cb, (void*)this));
	functionbtn_params.push_back(BTPackParam(NULL, "images/transcoder.png", InfoView::clicked_start_cb, (void*)this));
	btnpack.AddPacksHorizontally(functionbtn_params);
}

void InfoView::getoriginalvideoinfo(const char* path, VideoInfo& info)
{
	VideoPlayer player;
	player.Create();
	player.SetMediaAndPrepare(path, NULL);
	info = player.GetVideoCodecInfo();
	player.Destroy();
}


void InfoView::setinfo_tolist(ListCtrl& list, const MediaContentItem& content)
{
	try
	{
		std::vector<GenCtrlItem> items;
		VideoInfo info;
		getoriginalvideoinfo(content.path.c_str(), info);

		unsigned int video_option = getmodel()->GetSelectedOption(VIDEO_CODEC_OPTION);
		unsigned int audio_option = getmodel()->GetSelectedOption(AUDIO_CODEC_OPTION);
		unsigned int resolution_option = getmodel()->GetSelectedOption(RESOLUTION_OPTION);
		std::string video_codec = (video_option == ORIGINAL_FEATURE)?info.video_codec:getmodel()->GetOptionName(VIDEO_CODEC_OPTION, video_option);
		std::string audio_codec = (audio_option == ORIGINAL_FEATURE)?info.audio_codec:getmodel()->GetOptionName(AUDIO_CODEC_OPTION, audio_option);
		std::string resolution = (resolution_option == ORIGINAL_FEATURE)? AppTool::ToString(content.width)+" X "+AppTool::ToString(content.height):getmodel()->GetOptionName(RESOLUTION_OPTION, resolution_option);

		items.push_back(GenCtrlItem(VIDEO_CODEC_OPTION, video_codec.c_str(), "no media", getresiconpath("images/video.png").c_str()));
		items.push_back(GenCtrlItem(AUDIO_CODEC_OPTION, audio_codec.c_str(), "no media", getresiconpath("images/audio.png").c_str()));
		items.push_back(GenCtrlItem(RESOLUTION_OPTION, resolution.c_str(), "no media", getresiconpath("images/resolution.png").c_str()));
		std::string duration = AppTool::ToString(content.duration) + " ms";
		items.push_back(GenCtrlItem(3, duration.c_str(), "no media", getresiconpath("images/duration.png").c_str()));
		list.AppendItems(items);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	}
}

std::string InfoView::getresiconpath(const char* iconname)
{
	return std::string(app_get_resource_path()) + iconname;
}



void InfoView::UpdateView()
{
	m_list.RemoveAllItems();
	setinfo_tolist(m_list, getmodel()->GetSelectedContent());
}

void InfoView::change_optionview(int id)
{
	if(id < MAX_OPTION)
	{
		getmodel()->SelectTypeofOptions((TranscodingOptionType)id);
		MOVE_NEXTVIEW;
	}
}

void InfoView::starttranscoding()
{
	//m_pbpopup.Popup();


	CodecInfo venc, aenc;
	venc.venc.codecid = MEDIACODEC_MPEG4;
	venc.venc.width = 320;
	venc.venc.height = 240;
	venc.venc.fps = 30;
	venc.venc.target_bits = 256000;

	aenc.aenc.codecid = MEDIACODEC_MP3;
	aenc.aenc.channel = 2;
	aenc.aenc.samplerate = 44100;
	aenc.aenc.bit = 16;
	aenc.aenc.bitrate = 128000;

	if(m_transcodingengine.IsCreated())
		m_transcodingengine.Destroy();

	m_transcodingengine.Create(getmodel()->GetSelectedContent().path.c_str(), venc, aenc);
	m_transcodingengine.Start();

}

void InfoView::clicked_prev_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}

void InfoView::change_optionview_cb(void *data, int id)
{
	InfoView* view = (InfoView*)data;
	view->change_optionview(id);
}
void InfoView::clicked_start_cb(void *data, Evas_Object *obj, void *event_info)
{

	InfoView* view = (InfoView*)data;
	view->starttranscoding();
}
void InfoView::cancel_cb(void *data)
{
	//stop trandcoding
}



