/*
 * audioRoomView.h
 *
 *  Created on: June 21, 2016
 *      Author: Hyunjin
 * Description: 3D Audio play room.
 */

#ifndef AUDIOROOMVIEW_H_
#define AUDIOROOMVIEW_H_

#include "view.h"
#include "multimediaapp.h"
#include "apptemplate.h"
#include "audioManagerModel.h"
#include <app_common.h>
#include <stdexcept>
#include <assert.h>

#define TOOLBAR_ICON_SIZE 54
#define FRAME_BORDER 3
#define BOX_PAD 10
typedef std::vector<std::string> StrVec;
typedef struct
{
	unsigned int id; // id by which communicates with model (0: listener)
	int x;
	int y;
	int z;

} PlayItem;

typedef struct
{
	unsigned int id; // id by which communicates with model (0: listener)
	const char* name;
	const char* icon_img;
	Elm_Object_Item * item;

} ToolbarItem;


class AudioRoomView : public View
{
public:
	AudioRoomView();
	~AudioRoomView();

	void updateview();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();

private:
	void getResource(const char *file, char *path_out, int path_max);

	// audio room
	void createAudioRoom(Evas_Object *box);
	void createPlayList();
	void updatePlayItems();
	void setDefaultPos();

	// selection
	void createSelectionFrame(Evas_Object* box);

	// buttons
	void createButtons(Evas_Object* box);
	Evas_Object* createBtn(Evas_Object* box, Evas_Smart_Cb click_func, const char* label);
	static void closeBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	static void selectSrcBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	static void playBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);

	// toolbar
	void createItemList();
	void deleteItemList();
	void appendToolbarItem(Evas_Object* toolbar, int idx, Evas_Smart_Cb func, void *data);
	void createToolbar(Evas_Object* box);
	void addToolbarItems(Evas_Object *toolbar);
	void updateToolbarItems(Evas_Object *toolbar);

	// request data to model
	std::vector<unsigned int> getSelectedSrc(); // make m_sources
	StrVec getSrcNameList(); // get source name -> from m_sources idx로 list 만들기

private:
	int m_selectedNum; 	// 0~5 according to srcSelectionView
	StrVec m_list_selectedSrcName;
	int m_itemNum; 		// length of m_list_item
	ToolbarItem *m_list_item;
	PlayItem* m_list_play;

	Evas_Object *m_room;
	Evas_Object *m_toolbar;
	Evas_Object *m_playBtn;

	//TODO touch event
	Evas_Coord_Point m_start; // Position in which the cursor was pressed
	Evas_Coord_Point m_curr; // Current cursor position
	Evas_Coord_Point m_prev; // Previous cursor position
	bool m_isDragging;

};

#endif /* AUDIOROOMVIEW_H_ */
