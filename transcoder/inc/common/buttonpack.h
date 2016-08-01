/*
 * buttonpack.h
 *
 *  Created on: Jun 24, 2016
 *      Author: Jason
 */

#ifndef BUTTONPACK_H_
#define BUTTONPACK_H_

#include <Elementary.h>
#include <string>
#include <vector>


typedef struct _BTPackParam
{
	_BTPackParam(const char* _text, const char* _image_path, Evas_Smart_Cb _cbfunc, void* _data)
	{
		if(_text)
			text = _text;
		if(_image_path)
			image_path = _image_path;
		cbfunc = _cbfunc;
		data = _data;
	}
	_BTPackParam()
	:cbfunc(NULL), data(NULL)
	{

	}
	std::string text;
	std::string image_path;
	Evas_Smart_Cb cbfunc;
	void* data;
}BTPackParam;

typedef struct _BTPack
{
	Evas_Object* box;
	std::vector<Evas_Object*> btns;
}BTPack;


class ButtonPack
{
public:

	ButtonPack();
	~ButtonPack();

	void Create(Evas_Object* parent);
	void Destroy();
	void AddPacksHorizontally(std::vector<BTPackParam>& params);
	void ResetAllButtons();
private:
	Evas_Object* createbutton(Evas_Object* parent, const BTPackParam& param);

private:
	std::vector<BTPack> m_btpacks;
	Evas_Object* m_parent;
};



#endif /* BUTTONPACK_H_ */
