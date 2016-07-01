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
	void getoriginalvideoinfo(const char* path, VideoInfo& info);
	Evas_Object* createdisplay(Evas_Object* box);
	std::string getresiconpath(const char* iconname);
	void add_defaultbtns(ButtonPack& btnpack);
	void setinfo_tolist(ListCtrl& list, const MediaContentItem& content);

	void starttranscoding();
	void change_optionview(int id);

	static void clicked_prev_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_start_cb(void *data, Evas_Object *obj, void *event_info);
	static void change_optionview_cb(void *data, int id);
	static void cancel_cb(void *data);

private:
	ListCtrl m_list;
	ButtonPack m_btnpack;
	PopupProgress m_pbpopup;

#ifdef SUPPORT_PLAYER
	Evas_Object* m_display;
	VideoPlayer m_player;
#endif
};


#endif /* INFOVIEW_H_ */
