/*
 * FirstView.cpp
 *
 *  Created on: May 26, 2016
 *      Author: Jason
 */
#include "FirstView.h"
#include "multimediaapp.h"
#include <stdexcept>
#include "mediacontentcontroller.h"


FirstView::FirstView()

{

}

FirstView::~FirstView()
{

}

const char* FirstView::getedcfilename()
{
	return "edje/firstview.edj";
}

void FirstView::decorateview(Evas_Object* box)
{
	m_list.Create(box);

	std::vector<MediaContentItem> contents;
	MediaContentParam param;
	param.mediatype = MC_VIDEO_TYPE | MC_IMAGE_TYPE;
	MediaContent db;
	db.ConnectDB();
	db.GetItem(param, &contents);
	db.DisconnectDB();

	for(int i= 0;i < contents.size(); ++i)
	{
		ListItem item;
		item.media_id = i;
		item.displayname = contents[i].name;
		item.media_path = contents[i].path;
		item.thumbnail_path = contents[i].thumbnail_path;
		m_list.AppendItem(item);
	}


	Evas_Object* btn = elm_button_add(box);
	elm_object_text_set(btn, "btn to move to the second view");
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", clicked_cb, this);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);
	m_btn = btn;
}



void FirstView::destroyremains()
{
	m_list.Destroy();
}

void FirstView::UpdateView()
{

}

void FirstView::clicked_cb(void *data, Evas_Object *obj, void *event_info)
{

	MOVE_NEXTVIEW;
}




