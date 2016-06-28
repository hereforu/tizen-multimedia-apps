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
#include "audioManagerModel.h"


typedef struct _ButtonIndexMap
{
	Evas_Smart_Cb cbfunc;
	int index;
}ButtonIndexMap;

#define MAX_SOURCE_BTN 5

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

	void updateview(const std::vector<RoomSourceProperty>& sources);

	void configure_room(const std::vector<RoomSourceProperty>& sources);
	void reset_allbtns();
	void add_defaultbtns();
	void add_sourcebtns(const std::vector<RoomSourceProperty>& sources);

	void handlesourcebtn(int btnid);
	void handleplaybtn();
	void handlestopbtn();
	void handlesrcselbtn();

	void locateobjects();
	void handletimer();

	AudioManagerModel* getModel();
	Posf convertrelativepos(const EvasCoordRect& rect, const Pos& pos);
private:
	static void clicked_source0_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source1_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source2_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source3_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_source4_cb(void *data, Evas_Object *obj, void *event_info);


	static void clicked_srcsel_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_exit_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_play_cb(void *data, Evas_Object *obj, void *event_info);
	static void clicked_stop_cb(void *data, Evas_Object *obj, void *event_info);

	static Eina_Bool timer_cb(void* data);
private:

	ButtonIndexMap m_srcbtnindexmap[MAX_SOURCE_BTN];
	AudioRoom* m_room;
	ButtonPack* m_pack;
	Ecore_Timer* m_timer;
};



#endif /* MAINVIEW_H_ */
