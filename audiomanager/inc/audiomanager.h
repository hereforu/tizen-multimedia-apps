#ifndef __audiomanager_H__
#define __audiomanager_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "audiomanager"

#define MAX_DIST_PER_AXIS 5.0f

#if !defined(PACKAGE)
#define PACKAGE "org.example.audiomanager"
#endif

#endif /* __audiomanager_H__ */
