/*
 * infoview.cpp
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */


#include "infoview.h"
#include "common/multimediaapp.h"
#include <stdexcept>
#include "common/mediacontentcontroller.h"
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
:m_msgbox(NULL), m_timer(NULL), m_transcodingthread(NULL)
{

}

InfoView::~InfoView()
{

}

const char* InfoView::getedcfilename()
{
	return ((TranscoderModel*)((TranscoderModel*)getmodel()))->GetSelectedContent().name.c_str();
}

void InfoView::decorateview(Evas_Object* box)
{
	try
	{
		m_list.Create(box, change_optionview_cb, (void*)this);
		setinfo_tolist(m_list, ((TranscoderModel*)getmodel())->GetSelectedContent());
#ifdef SUPPORT_PLAYER
		m_display = createdisplay(box);
		m_player.Create();
		m_player.SetMediaAndPrepare(((TranscoderModel*)getmodel())->GetSelectedContent().path.c_str(), m_display);
		m_player.Play();
#endif
		m_btnpack.Create(box);
		add_defaultbtns(m_btnpack);
		m_pbpopup.Create(box, cancel_cb, (void*)this);
		m_timer = ecore_timer_add(1.0, InfoView::timer_cb, (void*)this);
		ecore_timer_freeze(m_timer);
		m_msgbox = createmsgbox(box);
	}
	catch(const std::runtime_error& e)
	{
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	}
}

void InfoView::destroyremains()
{
	if(m_msgbox)
	{
		evas_object_del(m_msgbox);
		m_msgbox = NULL;
	}
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #1");
	ecore_timer_del(m_timer);
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #2");
	if(m_transcodingengine.IsCreated())
	{
		m_transcodingengine.Destroy();
	}
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #3");
	m_list.Destroy();
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #4");
	m_btnpack.Destroy();
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #5");
	m_pbpopup.Destroy();
	dlog_print(DLOG_DEBUG, "InfoView", "destroyremains #6");
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

		unsigned int video_option = ((TranscoderModel*)getmodel())->GetSelectedOption(VIDEO_CODEC_OPTION);
		unsigned int audio_option = ((TranscoderModel*)getmodel())->GetSelectedOption(AUDIO_CODEC_OPTION);
		unsigned int resolution_option = ((TranscoderModel*)getmodel())->GetSelectedOption(RESOLUTION_OPTION);
		std::string video_codec = (video_option == ORIGINAL_FEATURE)?info.video_codec:((TranscoderModel*)getmodel())->GetOptionName(VIDEO_CODEC_OPTION, video_option);
		std::string audio_codec = (audio_option == ORIGINAL_FEATURE)?info.audio_codec:((TranscoderModel*)getmodel())->GetOptionName(AUDIO_CODEC_OPTION, audio_option);
		std::string resolution = (resolution_option == ORIGINAL_FEATURE)? to_string(content.width)+" X "+to_string(content.height):((TranscoderModel*)getmodel())->GetOptionName(RESOLUTION_OPTION, resolution_option);

		items.push_back(GenCtrlItem(VIDEO_CODEC_OPTION, video_codec.c_str(), "no media", getresiconpath("images/video.png").c_str()));
		items.push_back(GenCtrlItem(AUDIO_CODEC_OPTION, audio_codec.c_str(), "no media", getresiconpath("images/audio.png").c_str()));
		items.push_back(GenCtrlItem(RESOLUTION_OPTION, resolution.c_str(), "no media", getresiconpath("images/resolution.png").c_str()));
		std::string duration = to_string(content.duration) + " ms";
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
	setinfo_tolist(m_list, ((TranscoderModel*)getmodel())->GetSelectedContent());
}

void InfoView::change_optionview(int id)
{
	if(id < MAX_OPTION)
	{
		((TranscoderModel*)getmodel())->SelectTypeofOptions((TranscodingOptionType)id);
		MOVE_NEXTVIEW;
	}
}
void InfoView::move_prev()
{
	if(m_transcodingthread == NULL)
		MOVE_PREVVIEW;
}
void InfoView::ontime()
{
	double progress = m_transcodingengine.GetProgress();
	dlog_print(DLOG_DEBUG, "InfoView", "ontime is called and current position is %f", (float)progress);
	m_pbpopup.SetValue(progress);
}

void InfoView::canceltranscoding()
{
	dlog_print(DLOG_DEBUG, "InfoView", "cancel button is pressed");
	m_transcodingengine.Cancel();
}

void InfoView::update_progress()
{

}

void InfoView::getresolutionbycode(unsigned int code, int& width, int& height)
{
	if(code == ORIGINAL_FEATURE)
	{
		width = ((TranscoderModel*)getmodel())->GetSelectedContent().width;
		height = ((TranscoderModel*)getmodel())->GetSelectedContent().height;
	}
	else
	{
		int multiplier = (code & 0xFFFF)*10;
		width = (code & RATIO_16_9)?16*multiplier:4*multiplier;
		height = (code & RATIO_16_9)?9*multiplier:3*multiplier;
	}
}

void InfoView::long_func_transcoding(Ecore_Thread *thread)
{
	dlog_print(DLOG_DEBUG, "InfoView", "long_func_transcoding has been called");
	CodecInfo venc, aenc;
	venc.venc.codecid = (mediacodec_codec_type_e)((TranscoderModel*)getmodel())->GetSelectedOption(VIDEO_CODEC_OPTION);
	getresolutionbycode(((TranscoderModel*)getmodel())->GetSelectedOption(RESOLUTION_OPTION), venc.venc.width, venc.venc.height);
	venc.venc.fps = 30;
	venc.venc.target_bits =(int)((double)venc.venc.width*(double)venc.venc.height*(double)venc.venc.fps*0.20);


	aenc.aenc.codecid = (mediacodec_codec_type_e)((TranscoderModel*)getmodel())->GetSelectedOption(AUDIO_CODEC_OPTION);
	aenc.aenc.channel = ORIGINAL_FEATURE;
	aenc.aenc.samplerate = ORIGINAL_FEATURE;
	aenc.aenc.bit = ORIGINAL_FEATURE;
	aenc.aenc.bitrate = ORIGINAL_FEATURE;

	try
	{
		m_transcodingengine.Create(((TranscoderModel*)getmodel())->GetSelectedContent().path.c_str(), ((TranscoderModel*)getmodel())->GetSelectedContent().duration, venc, aenc);
		ecore_timer_thaw(m_timer);
		m_transcodingengine.Transcoding();
		ecore_timer_freeze(m_timer);
		if(m_transcodingengine.IsCanceled()==false)
		{
			((TranscoderModel*)getmodel())->AddFileToDB(m_transcodingengine.GetDstFileName());
		}
		else
		{
			if(access(m_transcodingengine.GetDstFileName(), F_OK) != -1)
			{
				remove(m_transcodingengine.GetDstFileName());
			}
		}
		m_transcodingengine.Destroy();
		m_pbpopup.Hide();

		//if succeed
		//media_content_scan_file (const char *path)
	}
	catch(const std::runtime_error& e)
	{
		m_transcodingengine.Destroy();
		m_pbpopup.Hide();
		dlog_print(DLOG_ERROR, "InfoView", e.what());
	//	showmsgbox(e.what());
	}


}
void InfoView::end_func_transcoding(Ecore_Thread *thread)
{
	dlog_print(DLOG_DEBUG, "InfoView", "end_func_transcoding has been called");
	ecore_timer_freeze(m_timer);
	m_transcodingthread = NULL;
}
void InfoView::cancel_func_transcoding(Ecore_Thread *thread)
{
	dlog_print(DLOG_DEBUG, "InfoView", "cancel_func_transcoding has been called");
	ecore_timer_freeze(m_timer);
}

void InfoView::clicked_prev_cb(void *data, Evas_Object *obj, void *event_info)
{
	InfoView* view = (InfoView*)data;
	view->move_prev();
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
	InfoView* view = (InfoView*)data;
	view->canceltranscoding();
}
Eina_Bool InfoView::timer_cb(void *data)
{
	InfoView* view = (InfoView*)data;
	view->ontime();
	return EINA_TRUE;
}

void InfoView::update_progress_cb(void* data)
{
	InfoView* view = (InfoView*)data;
	view->update_progress();
}
void InfoView::long_func_transcoding_cb(void *data, Ecore_Thread *thread)
{
	InfoView* view = (InfoView*)data;
	view->long_func_transcoding(thread);
}
void InfoView::end_func_transcoding_cb(void *data, Ecore_Thread *thread)
{
	InfoView* view = (InfoView*)data;
	view->end_func_transcoding(thread);
}
void InfoView::cancel_func_transcoding_cb(void *data, Ecore_Thread *thread)
{
	InfoView* view = (InfoView*)data;
	view->cancel_func_transcoding(thread);
}

void InfoView::starttranscoding()
{

	if(m_transcodingthread)
	{
		dlog_print(DLOG_ERROR, "InfoView", "transcoding thread is alive!!");
		return;
	}
	m_pbpopup.Popup();
	m_transcodingthread = ecore_thread_run(InfoView::long_func_transcoding_cb, InfoView::end_func_transcoding_cb
			, InfoView::cancel_func_transcoding_cb, (void*)this);
	dlog_print(DLOG_DEBUG, "InfoView", "ecore_thread_run [%p]", m_transcodingthread);
}


void InfoView::msgboxtimeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	InfoView* view = (InfoView*)data;
	view->hidemsgbox();
}

Evas_Object* InfoView::createmsgbox(Evas_Object* box)
{
	Evas_Object* popup = elm_popup_add(box);
	if(popup == NULL)
	{
		throw std::runtime_error("fail to create popalarm ");
	}
	elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
	elm_object_style_set(popup, "toast");
	evas_object_smart_callback_add(m_msgbox, "timeout", msgboxtimeout_cb, this);
	return popup;
}

void InfoView::showmsgbox(const char* str)
{
	elm_popup_timeout_set(m_msgbox, 2.0);
	elm_object_part_text_set(m_msgbox, "default", str);
	evas_object_show(m_msgbox);
}
void InfoView::hidemsgbox()
{
	if(m_msgbox)
		evas_object_hide(m_msgbox);
}





