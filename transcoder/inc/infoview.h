/*
 * infoview.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#ifndef INFOVIEW_H_
#define INFOVIEW_H_


#include "common/view.h"
#include "common/listctrl.h"
#include "transcodermodel.h"
#include "common/buttonpack.h"
#include "popupprogress.h"
#include "transcodingengine.h"
#include "videoplayer.h"
#include <memory>


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
	void fill_encoderinfo(CodecInfo& venc, CodecInfo& aenc);
	void process_after_transcoding(bool iscanceled, const char* outfilename);
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
	void canceltranscoding();
	Evas_Object* createmsgbox(Evas_Object* box);
	void showmsgbox(const char* str);
	void hidemsgbox();


	void update_progress();
	void long_func_transcoding(Ecore_Thread *thread);
	void end_func_transcoding(Ecore_Thread *thread);
	void cancel_func_transcoding(Ecore_Thread *thread);


	static void clicked_prev_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_start_cb(void *data, Evas_Object *obj, void *event_info);
	static void change_optionview_cb(void *data, int id);
	static void cancel_cb(void *data);
	static Eina_Bool timer_cb(void *data);
	static void msgboxtimeout_cb(void *data, Evas_Object *obj, void *event_info);

	//thread functions
	static void update_progress_cb(void* data);
	static void long_func_transcoding_cb(void *data, Ecore_Thread *thread);
	static void end_func_transcoding_cb(void *data, Ecore_Thread *thread);
	static void cancel_func_transcoding_cb	(void *data, Ecore_Thread *thread);
private:
	VideoPlayer* m_videoplayer;
	Evas_Object* m_display;
	Evas_Object* m_msgbox;
	Ecore_Timer* m_timer;
	Ecore_Thread* m_transcodingthread;
	TranscodingEngine* m_transcodingengine;
	ListCtrl m_list;
	ButtonPack m_btnpack;
	PopupProgress m_pbpopup;

};


#endif /* INFOVIEW_H_ */
