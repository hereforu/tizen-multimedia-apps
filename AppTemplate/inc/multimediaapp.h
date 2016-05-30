/*
 * multimediaapp.h
 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "frame.h"
#include "model.h"
#include "templatesingleton.h"

#ifndef MULTIMEDIAAPP_H_
#define MULTIMEDIAAPP_H_

class MultimediaApp : public TemplateSingleton<MultimediaApp>
{
public:
	MultimediaApp();
	~MultimediaApp();

	//change view
	void MoveNextView();
	void MovePrevView();

	//event handler
	void HandlerAppCreate();
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
	FrameWindow* m_FrameWnd;
	Model* m_Model;

};



#endif /* MULTIMEDIAAPP_H_ */
