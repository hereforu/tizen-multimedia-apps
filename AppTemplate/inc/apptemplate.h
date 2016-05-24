#ifndef __apptemplate_H__
#define __apptemplate_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "apptemplate"

#if !defined(PACKAGE)
#define PACKAGE "org.example.apptemplate"
#endif

#define EDJ_FILE "edje/apptemplate.edj"
#define GRP_MAIN "main"


#endif /* __apptemplate_H__ */
