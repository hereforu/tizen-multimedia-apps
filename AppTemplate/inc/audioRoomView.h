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
#include <dlog.h>
//else dlog_print(DLOG_ERROR, LOG_TAG, "no m_list_item %d", i); check

#define ICON_SIZE 54
#define FRAME_BORDER 3
#define BOX_PAD 10
typedef std::vector<std::string> StrVec;
typedef std::string String;
typedef struct ROOM
{
	Evas_Object * room;
	int x, y, w, h;

} Room;

typedef struct
{
	unsigned int id; // id by which communicate with model
	int x;
	int y;
	int z;

} PlayItem;

typedef struct
{
	int id; // id by which communicate with model, -1: not a source
	String name;
	String icon_file;
	Elm_Object_Item * item;

} ToolbarItem;


class AudioRoomView : public View
{
public:
	AudioRoomView();
	~AudioRoomView();

	void UpdateView();

protected:
	const char* getedcfilename();
	void decorateview(Evas_Object* box);
	void destroyremains();

private:
	void getResource(const char *file, char *path_out, int path_max);

	// audio room
	void createAudioRoom(Evas_Object *box);
	void setRoomGeometry();
	bool isInRoomArea(int x, int y);

	// play item in audio room
	void createPlayItemList();
	void deletePlayItemList();
	void setDefaultPlayItem(PlayItem* pItem, int idx);

	// operation in audio room
	Evas_Object* createImage(const char* icon_path, Evas_Object* canvas);

	void putPlayItem(int idx);
	void deletePlayItem(int idx); 	//TODO

	// item selection
	void createSelectionFrame(Evas_Object* box);

	// buttons
	void createButtons(Evas_Object* box); // call crateBtn()
	Evas_Object* createBtn(Evas_Object* box, Evas_Smart_Cb click_func, const char* label);

	static void closeBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	static void selectSrcBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);
	static void playBtnclicked_cb(void *data, Evas_Object *obj, void *event_info);

	// toolbar
	void createToolbar(Evas_Object* box);
	void updateToolbar(Evas_Object *toolbar);

	// toolbar item
	void createToolbarItemList();
	void deleteToolbarItemList();
	void setDefaultToolbarItem(int list_idx, int sourceId, String name, String icon_path);

	void addToolbarItems(Evas_Object *toolbar); // call appendToolbarItem()
	void appendToolbarItem(Evas_Object* toolbar, int idx, Evas_Smart_Cb func, void *data);

	// toolbar item cb


	// request data to model: called from createItemList()
	std::vector<unsigned int> getSelectedSrcIdx();
	StrVec getSrcNameList(); // get selected source's name

private:
	int m_playItemNum;		// 0~5 according to srcSelectionView
	PlayItem* m_list_playItem; 	// last item is listener

	int m_toolbarItemNum; 	// length of m_list_toolbarItem
	ToolbarItem *m_list_toolbarItem;

	Room m_room;
	Evas_Object *m_toolbar;
	Evas_Object *m_playBtn;

	//TODO touch event
	Evas_Coord_Point m_start; // Position in which the cursor was pressed
	Evas_Coord_Point m_curr;  // Current cursor position
	Evas_Coord_Point m_prev;  // Previous cursor position
	bool m_isDragging;

};

#endif /* AUDIOROOMVIEW_H_ */
