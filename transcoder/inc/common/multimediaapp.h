/*
 * multimediaapp.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "frame.h"
#include "model.h"
#include "templatesingleton.h"
#include <vector>

#ifndef MULTIMEDIAAPP_H_
#define MULTIMEDIAAPP_H_

class MultimediaApp : public TemplateSingleton<MultimediaApp>
{
public:
	MultimediaApp();
	virtual ~MultimediaApp();

	//change view
	void MoveNextView();
	void MovePrevView();
	Model* GetModel();

	//event handler
	bool HandlerAppCreate(Model* model, const std::vector<View*>& views);
	void HandlerAppTerminate();
	void HandlerAppPause();
	void HandlerAppResume();
	void HandlerAppControl();
	void HandleLowBatteryEvent();
	void HandleLowMemoryEvent();
	void HandleDeviceOrientationChangedEvent();
	void HandleLanguageChangedEvent();
	void HandleRegionFormatChangedEvent();
private:
	void destroymodel();
	void destroyframe();
private:
	Frame* m_frame;
	Model* m_model;

};

#define MOVE_NEXTVIEW MultimediaApp::GetInstance()->MoveNextView()
#define MOVE_PREVVIEW MultimediaApp::GetInstance()->MovePrevView()

#endif /* MULTIMEDIAAPP_H_ */
