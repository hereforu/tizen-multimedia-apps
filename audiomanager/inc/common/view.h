/*
 * view.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <Elementary.h>
#include "model.h"
#include <string>

class View
{
public:
	View();
	virtual ~View();

	void Create(Evas_Object* naviframe, Evas_Object* conformant);
	virtual void UpdateView() = 0; // ex. view_1 -> view_2 -> view_1, update view_1 if needed
	void Destroy();
	bool IsCreated();

protected:
	virtual void decorateview(Evas_Object* box) = 0;
	//do not delete elm objects which are attached to the box
	virtual void destroyremains()=0;
	virtual const char* getedcfilename() = 0;
private:
	Evas_Object* createbox(Evas_Object* naviframe, Evas_Object* conformant);
protected:
	Evas_Object* m_box;

private:
	Evas_Object* m_naviframe;
	Elm_Object_Item * m_Naviitem;
};


#endif /* VIEW_H_ */
