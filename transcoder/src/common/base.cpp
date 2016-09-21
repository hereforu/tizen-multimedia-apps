/*
 * base.cpp

 *
 *  Created on: May 24, 2016
 *      Author: Jason
 */

#include "common/base.h"
#include <assert.h>
#include <stdexcept>


#define _PACKET_LIFE_DBG

#ifdef _PACKET_LIFE_DBG
#include <set>
std::set<int> g_packet_set;
#endif

void AppTool::packet_created_dbg(media_packet_h pkt)
{
#ifdef _PACKET_LIFE_DBG
	 int address = (int)pkt;
	 std::set<int>::iterator iter;
	 iter = g_packet_set.find(address);
	 if(iter != g_packet_set.end())
	 {
		 dlog_print(DLOG_ERROR, "PACKET_DEBUG", "double packets!!![%p][%d]", pkt, address);
	 }
	 else
	 {
		 g_packet_set.insert(address);
	 }
	 dlog_print(DLOG_DEBUG, "PACKET_DEBUG", "number of current packets [%d]", (int)g_packet_set.size());
#endif
}
void AppTool::media_packet_destroy_dbg(media_packet_h pkt)
{
#ifdef _PACKET_LIFE_DBG
	int address = (int)pkt;
	 std::set<int>::iterator iter;
	 iter = g_packet_set.find(address);
	 if(iter != g_packet_set.end())
	 {
		 g_packet_set.erase(iter);
	 }
	 else
	 {
		 dlog_print(DLOG_ERROR, "PACKET_DEBUG", "try to delete uncreated packets!!![%p][%d]", pkt, address);
	 }
	 dlog_print(DLOG_DEBUG, "PACKET_DEBUG", "number of current packets [%d]", (int)g_packet_set.size());
#endif
	media_packet_destroy(pkt);
}


void AppTool::assert_ifnot(bool condition)
{
	assert(condition);
}

void AppTool::iferror_throw(int ret, const char* msg)
{
	if(ret != 0)
		throw std::runtime_error(std::string(msg)+to_string(ret));
}


