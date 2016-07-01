/*
 * listctrl.h
 *
 *  Created on: Jun 28, 2016
 *      Author: Jason
 */

#ifndef LISTCTRL_H_
#define LISTCTRL_H_

#include "genctrl.h"

class ListCtrl : public GenCtrl
{
public:
	ListCtrl();
	~ListCtrl();

protected:

	virtual Evas_Object* creategenctrl(Evas_Object* parent);
	virtual Elm_Object_Item* appenditem(Elm_Gen_Item_Class* itc, DataforGenCtrlCB* cbdata, Evas_Smart_Cb selectcb);
	virtual Elm_Gen_Item_Class* createitc(Elm_Gen_Item_Text_Get_Cb textcb, Elm_Gen_Item_Content_Get_Cb contentcb, Elm_Gen_Item_Del_Cb delcb);
	virtual void freeitc(Elm_Gen_Item_Class* itc);
	virtual void removeallitems();

private:

};


#endif /* LISTCTRL_H_ */
