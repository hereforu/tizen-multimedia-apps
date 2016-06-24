/*
 * mainview.h
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#ifndef MAINVIEW_H_
#define MAINVIEW_H_

#include "view.h"
#include "audioroom.h"
#include "buttonpack.h"

#define NUM_OBJECT 6

class MainView : public View
{
public:
	MainView();
	~MainView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();

private:
	void handlesourcebtn(int btnid);
	void handleplaybtn();
	void handlesrcselbtn();

private:
	static void clicked_source1_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source2_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source3_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source4_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source5_cb(void *data, Evas_Object *obj, void *event_info);

	static void clicked_srcsel_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_exit_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_play_cb(void *data, Evas_Object *obj, void *event_info);
private:
	Evas_Object* m_label;
	AudioRoom* m_room;
	ButtonPack* m_pack;
};



#endif /* MAINVIEW_H_ */
