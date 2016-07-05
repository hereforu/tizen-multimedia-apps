/*
 * infoview.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#ifndef INFOVIEW_H_
#define INFOVIEW_H_


#include "view.h"
#include "listctrl.h"
#include "transcodermodel.h"
#include "buttonpack.h"
#include "videoplayer.h"
#include "popupprogress.h"
#include "transcodingengine.h"


class InfoView : public View
{
public:
	InfoView();
	~InfoView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();



private:
	void getresolutionbycode(unsigned int code, int& width, int& height);
	void getoriginalvideoinfo(const char* path, VideoInfo& info);
	Evas_Object* createdisplay(Evas_Object* box);
	std::string getresiconpath(const char* iconname);
	void add_defaultbtns(ButtonPack& btnpack);
	void setinfo_tolist(ListCtrl& list, const MediaContentItem& content);
	void starttranscoding();
	void change_optionview(int id);
	void move_prev();
	void ontime();


	void update_progress();
	void long_func_transcoding(Ecore_Thread *thread);
	void end_func_transcoding(Ecore_Thread *thread);
	void cancel_func_transcoding(Ecore_Thread *thread);

	static void clicked_prev_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_start_cb(void *data, Evas_Object *obj, void *event_info);
	static void change_optionview_cb(void *data, int id);
	static void cancel_cb(void *data);
	static Eina_Bool timer_cb(void *data);

	//thread functions
	static void update_progress_cb(void* data);
	static void long_func_transcoding_cb(void *data, Ecore_Thread *thread);
	static void end_func_transcoding_cb(void *data, Ecore_Thread *thread);
	static void cancel_func_transcoding_cb	(void *data, Ecore_Thread *thread);
private:
	Ecore_Timer* m_timer;
	Ecore_Thread* m_transcodingthread;
	ListCtrl m_list;
	ButtonPack m_btnpack;
	PopupProgress m_pbpopup;
	TranscodingEngine m_transcodingengine;

#ifdef SUPPORT_PLAYER
	Evas_Object* m_display;
	VideoPlayer m_player;
#endif
};


#endif /* INFOVIEW_H_ */
