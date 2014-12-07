#ifndef __MESSAGING_SYSTEM
#define __MESSAGING_SYSTEM

#include "PreCompiled.h"
#include "HashedString.h"
#include <vector>

namespace Engine
{
	namespace MessagingSystem
	{
		typedef void (* MessageHandler) (void * pData);

		typedef struct _Messsage
		{
			HashedString mMessage;
			
			std::vector<MessageHandler> mHandlers;
		}Message;

		void CreateMessage(const HashedString &i_Message);
		void RegisterMessageHandler(const HashedString &i_Message, const MessageHandler i_Handler);
		void UnRegisterMessageHandler(const HashedString &i_Message, const MessageHandler i_Handler);
		void BroadcastMessage(const HashedString &i_Message, void *i_Data);
	}
}

#endif //__MESSAGING_SYSTEM