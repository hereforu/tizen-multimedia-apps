/*
 * srcSelectionView.cpp
 *
 *  Created on: June 3, 2016
 *      Author: Hyunjin
 */
#include "srcSelectionView.h"
#include "multimediaapp.h"
#include <stdexcept>

SrcSelectionView::SrcSelectionView()
	:m_list_selectedSrcIdx(NULL), m_maxSelection(5)
{
	m_srcNum = getSrcNum();
	initSelectedSrc();
}

SrcSelectionView::~SrcSelectionView()
{
	delete[] m_list_selectedSrcIdx;

	//elm_genlist_clear()
	//evas_object_del(genlist)
}

/* view */
const char* SrcSelectionView::getedcfilename()
{
	// view title
	return "Select Audio Sources";
}

void SrcSelectionView::decorateview(Evas_Object* box)
{
	//start(box);
	createList(box);
	createBackBtn(box);
}

void SrcSelectionView::destroyremains()
{

}

void SrcSelectionView::initSelectedSrc()
{
	m_list_selectedSrcIdx = new int[m_maxSelection];
	for(int i=0; i<m_maxSelection; i++)
		m_list_selectedSrcIdx[i] = -1;
}

/* source */
int SrcSelectionView::getSrcNum()
{
	// temp, TODO
	return 10;
}

const char* SrcSelectionView::getSrcNameByIdx(int idx)
{
	// temp, TODO
	return "audio";
}

/* list */
/*Evas_Object* SrcSelectionView::createIcon(Evas_Object* box, const Eina_Unicode iconPath)
{
	// create icon
	Evas_Object* ic = elm_icon_add(box);

	// set icon image
	if(eina_unicode_strcmp(iconPath, ""))
		elm_image_file_set(ic, "res/auido.png", NULL);
	else elm_image_file_set(ic, iconPath, NULL);

	return ic;
}*/

char* SrcSelectionView::genlist_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	if (strcmp(part, "elm.text") == 0)
		return strdup((char*)data);
	else return NULL;
}

Evas_Object* SrcSelectionView::genlist_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *checkbox = NULL;

	if (!strcmp(part, "elm.swallow.icon.1")) {
	checkbox = elm_check_add(obj);
	elm_object_style_set(checkbox, "default");//"on&off");
	elm_check_state_set(checkbox, EINA_TRUE);
	elm_check_state_pointer_set(checkbox, EINA_FALSE);//)(g_switch_state));
	//evas_object_smart_callback_add(checkbox, "changed", item_activated_cb, EINA_FALSE);//(g_switch_state));
	evas_object_size_hint_align_set(checkbox, 0.7, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(checkbox, 1, 1);

		//		vc_elm_create_object(checkbox, &voice_h[item_number]);
			//	vc_elm_set_command(voice_h[item_number], "airplane");
				//vc_elm_set_command_hint(voice_h[item_number], "airplane");

	return checkbox;
	}

				if (!strcmp(part, "elm.swallow.icon"))
				   {
				      Evas_Object *img = elm_image_add(obj);
				      elm_image_file_set(img, "audio.png", NULL);

				      return img;
				   }
				   else return NULL;
}

static void genlist_del_cb(void *data, Evas_Object *obj)
{

}

Elm_Genlist_Item_Class* SrcSelectionView::createItc() // manage list items
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	if(itc == NULL)
	{
		throw std::runtime_error("fail to create genlist item class");
	}
	itc->item_style = "type1";
	itc->func.text_get = genlist_text_get_cb;;
	itc->func.content_get = genlist_content_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = genlist_del_cb;

	return itc;
}

void SrcSelectionView::selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	//Eina_List* selectedList = elm_list_selected_item_get(list);
}

void SrcSelectionView::createList(Evas_Object* box)
{
	Elm_Genlist_Item_Class *itc = createItc();

	Evas_Object * list = elm_genlist_add(box);
	if(list == NULL)
	{
		throw std::runtime_error("fail to create list");
	}
	evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

	//elm_list_select_mode_set(list, ELM_OBJECT_SELECT_MODE_ALWAYS);
	//elm_list_multi_select_set(list, EINA_TRUE);

	elm_object_style_set(list, "default");
	elm_genlist_homogeneous_set(list, EINA_TRUE); // make all items the same height (no need to cal)
	elm_genlist_mode_set(list, ELM_LIST_SCROLL);

	for(int i=0; i<m_srcNum; i++)
	{
		const char* str = getSrcNameByIdx(i);
		elm_genlist_item_append(list, itc, (void*)str, NULL, ELM_GENLIST_ITEM_NONE, selected_cb, NULL);
	}

	elm_box_pack_end(box, list);
	evas_object_show(list);
	elm_genlist_item_class_free(itc);
}


/* button */
void SrcSelectionView::backBtnclicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	MOVE_PREVVIEW;
}

void SrcSelectionView::createBackBtn(Evas_Object* box)
{
	Evas_Object* btn = elm_button_add(box);
	if(btn == NULL)
	{
		throw std::runtime_error("fail to create button");
	}
	elm_object_text_set(btn, "Back to Audio Room");
	//evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 0.5);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(btn, "clicked", backBtnclicked_cb, this);
	elm_box_pack_end(box, btn);
	evas_object_show(btn);
}

/* conversion */
char* SrcSelectionView::strToChar(std::string str)
{
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());

	return cstr;
}
